//
// Created by ns on 9/20/25.
//

#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <memory>
#include <thread>
#include <chrono>

#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"

#include "AudioCore.h"
#include "Constants.h"
#include "Mixer.h"
#include "ThreadStatics.h"

#include "../AppState.h"
#include "../enums/Result.h"

namespace Gj {
namespace Audio {

using namespace caf;

struct AudioPlayer {

  long threadId;
  actor_system& actorSystem;
  AudioCore* audioCore;
  Mixer* mixer;
  AppState* gAppState;
  std::shared_ptr<JackClient> jackClient;

  bool jackClientIsActive = false;
  float effectsChannelsSettings[2 * MAX_EFFECTS_CHANNELS]{};
  sf_count_t playbackSettingsToAudioThread[PlaybackSettingsToAudioThread_Count]{};
  sf_count_t playbackSettingsFromAudioThread[PlaybackSettingsFromAudioThread_Count]{};
  float fft_eq_buffer[FFT_EQ_RING_BUFFER_SIZE]{};
  jack_ringbuffer_t* fft_eq_ring_buffer_out;
  float vu_buffer[VU_RING_BUFFER_SIZE]{};
  jack_ringbuffer_t* vu_ring_buffer_out;

  AudioPlayer(actor_system& actorSystem, AudioCore* audioCore, Mixer* mixer, AppState* gAppState)
    : threadId(ThreadStatics::incrThreadId())
    , actorSystem(actorSystem)
    , audioCore(audioCore)
    , mixer(mixer)
    , gAppState(gAppState)
    , jackClient(mixer->getGjJackClient())
    {
    if (jackClient == nullptr) {
      Logging::write(
        Error,
        "Audio::AudioPlayer::AudioPlayer",
        "jackClient is null"
      );
      throw std::runtime_error(
        "Unable to instantiate AudioPlayer - null jackClient"
      );
    }

    if (const int audioCoreRes = setupAudioCore(); audioCoreRes > 0) {
      Logging::write(
        Error,
        "Audio::AudioPlayer::AudioPlayer",
        "Unable to setup AudioCore - status: " + std::to_string(audioCoreRes)
      );
      throw std::runtime_error(
        "Unable to instantiate AudioPlayer - AudioCore status: " + std::to_string(audioCoreRes)
      );
    }

    if (jackClient->activate(audioCore) != OK) {
      Logging::write(
        Error,
        "Audio::AudioPlayer::AudioPlayer",
        "Unable to activate Jack"
      );
      throw std::runtime_error(
        "Unable to instantiate AudioPlayer"
      );
    }

    jackClientIsActive = true;

    Logging::write(
      Info,
      "Audio::AudioPlayer::AudioPlayer",
      "AudioPlayer initialized"
    );

  }

  ~AudioPlayer() {
    if (jackClientIsActive) {
      if (jackClient->deactivate() != OK) {
        Logging::write(
          Error,
          "Audio::AudioPlayer::cleanup",
          "Unable to deactivate jackClient"
        );
      }
    }
    jack_ringbuffer_free(vu_ring_buffer_out);
    jack_ringbuffer_free(fft_eq_ring_buffer_out);
    jackClientIsActive = false;
  }

  int setupAudioCore() {
    Logging::write(
      Info,
      "Audio::AudioPlayer::setupAudioCore",
      "Setting up AudioCore"
    );

    // update plugin effects with info about audio to be processed
    // if (mixer->setSampleRate(sfInfo.samplerate) != OK) {
    //   Logging::write(
    //     Warning,
    //     "Audio::AudioPlayer::setupAudioCore",
    //     "Unable to set sample rate: " + std::to_string(sfInfo.samplerate)
    //   );
    // }

    ThreadStatics::setFrames(audioCore->currentDeck().frames);

    audioCore->fillPlaybackBuffer = &JackClient::fillPlaybackBuffer;
    audioCore->crossfade = gAppState->getCrossfade();

    // audioCore->inputBuffers[0] = audioCore->decks[audioCore->deckIndex].cassette->inputBuffers[0];
    // audioCore->inputBuffers[1] = audioCore->decks[audioCore->deckIndex].cassette->inputBuffers[1];

    Logging::write(
      Info,
      "Audio::AudioPlayer::setupAudioCore",
      "Successfully setup AudioData buffers."
    );

    // populate initial channel settings
    for (ChannelIndex i = 0; i < mixer->getTotalChannelsCount(); i++) {
      effectsChannelsSettings[4 * i]     = mixer->getEffectsChannel(i)->getGain();
      effectsChannelsSettings[4 * i + 1] = mixer->getEffectsChannel(i)->getMute();
      effectsChannelsSettings[4 * i + 2] = mixer->getEffectsChannel(i)->getSolo();
      effectsChannelsSettings[4 * i + 3] = mixer->getEffectsChannel(i)->getPan();
    }

    Logging::write(
      Info,
      "Audio::AudioPlayer::setupAudioCore",
      "Populated initial effectsSettings into buffer"
    );

    Logging::write(
      Info,
      "Audio::AudioPlayer::setupAudioCore",
      "Setting up AudioCore effects processing..."
    );

    audioCore->setChannelCount(mixer->getTotalChannelsCount());
    for (const auto ch : mixer->getEffectsChannels()) {
      const auto chIndex = ch->getIndex();
      audioCore->effectsChannelsSettings[2 * chIndex] = ch->getGain();
      audioCore->effectsChannelsSettings[2 * chIndex + 1] = ch->getPan();

      audioCore->effectsChannelsProcessData[chIndex].effectCount = ch->effectCount();
      for (EffectIndex pluginIdx = 0; pluginIdx < ch->effectCount(); pluginIdx++) {
        const auto plugin = ch->getPluginAtIdx(pluginIdx);
        audioCore->effectsChannelsProcessData[chIndex].processFuncs[pluginIdx] =
          [audioClient = plugin->audioHost->audioClient](auto && buffers, auto && continuousFrames) {
            return audioClient->process(
              std::forward<decltype(buffers)>(buffers),
              std::forward<decltype(continuousFrames)>(continuousFrames)
            );
          };

        audioCore->effectsChannelsProcessData[chIndex].buffers[pluginIdx] =
          getPluginBuffers(ch, chIndex, pluginIdx);
      }
    }

    Logging::write(
      Info,
      "Audio::AudioPlayer::setupAudioCore",
      "Setup AudioCore Effects processing. Successfully setup AudioCore."
    );

    return 0;
  }

  IAudioClient::Buffers getPluginBuffers(const Effects::EffectsChannel* effectsChannel, const ChannelIndex channelIdx, const EffectIndex pluginIdx) const {
    const auto audioFramesPerBuffer = static_cast<int32_t>(gAppState->getAudioFramesPerBuffer());

    // - each channel gets processed into it's effectChannelWriteOut
    // - the channels are then summed down into the processBuffers
    // - the main channel then acts upon the processBuffers
    float** inputBuffers = audioCore->processBuffers;
    float** outputBuffers = audioCore->processBuffers;

    if (channelIdx > 0 && pluginIdx == 0)
      inputBuffers = audioCore->playbackBuffers;

    if (const size_t effectsCount = effectsChannel->effectCount(); channelIdx > 0 && pluginIdx == effectsCount - 1)
      outputBuffers = const_cast<float**>(audioCore->effectsChannelsWriteOut[channelIdx]);

    return {
      inputBuffers,
      2,
      outputBuffers,
      2,
      audioFramesPerBuffer
    };
  }

  Result updateRingBuffers() {
    // read vu_ring_buffer
    if (jack_ringbuffer_read_space(audioCore->vu_ring_buffer) > VU_RING_BUFFER_SIZE - 2) {
      jack_ringbuffer_read(
        audioCore->vu_ring_buffer,
        reinterpret_cast<char*>(vu_buffer),
        VU_RING_BUFFER_SIZE
      );
    }

    if (jack_ringbuffer_write_space(vu_ring_buffer_out) > VU_RING_BUFFER_SIZE - 2) {
      jack_ringbuffer_write(
        vu_ring_buffer_out,
        reinterpret_cast<char*>(vu_buffer),
        VU_RING_BUFFER_SIZE
      );
    }

    // read fft_eq_ring_buffer
    if (jack_ringbuffer_read_space(audioCore->fft_eq_ring_buffer) > FFT_EQ_RING_BUFFER_SIZE - 2) {
      jack_ringbuffer_read(
        audioCore->fft_eq_ring_buffer,
        reinterpret_cast<char*>(fft_eq_buffer),
        FFT_EQ_RING_BUFFER_SIZE
      );
    }

    if (jack_ringbuffer_write_space(fft_eq_ring_buffer_out) > FFT_EQ_RING_BUFFER_SIZE - 2) {
      jack_ringbuffer_write(
        fft_eq_ring_buffer_out,
        reinterpret_cast<char*>(fft_eq_buffer),
        FFT_EQ_RING_BUFFER_SIZE
      );
    }

    // read playbackSettingsFromAudioThread ring buffer
    if (jack_ringbuffer_read_space(audioCore->playbackSettingsFromAudioThreadRB) > PlaybackSettingsFromAudioThread_RB_SIZE - 2)
      jack_ringbuffer_read(
        audioCore->playbackSettingsFromAudioThreadRB,
        reinterpret_cast<char*>(playbackSettingsFromAudioThread),
        PlaybackSettingsFromAudioThread_RB_SIZE
      );

    const sf_count_t currentFrameId = playbackSettingsFromAudioThread[1];
    mixer->getUpdateProgressBarFunc()(audioCore->currentDeck().frames, currentFrameId);

    playbackSettingsToAudioThread[0] = 0;
    playbackSettingsToAudioThread[1] = 0;
    playbackSettingsToAudioThread[2] = static_cast<sf_count_t>(std::floor(gAppState->getScene().playbackSpeed * 100.0f));

    if (ThreadStatics::getUserSettingFrameId()) {
      const sf_count_t newFrameId = ThreadStatics::getFrameId();
      playbackSettingsToAudioThread[0] = 1;
      playbackSettingsToAudioThread[1] = newFrameId;

      ThreadStatics::setUserSettingFrameId(false);
    }

    // write to playbackSettingsToAudioThread ring buffer
    if (jack_ringbuffer_write_space(audioCore->playbackSettingsToAudioThreadRB) > PlaybackSettingsToAudioThread_RB_SIZE - 2) {
      jack_ringbuffer_write(
        audioCore->playbackSettingsToAudioThreadRB,
        reinterpret_cast<char*>(playbackSettingsToAudioThread),
        PlaybackSettingsToAudioThread_RB_SIZE
      );
    }

    const ChannelIndex channelCount = mixer->getTotalChannelsCount();
    const float channelCountF = static_cast<float>(channelCount);

    Effects::EffectsChannel* effectsChannels[MAX_EFFECTS_CHANNELS] {nullptr};
    float soloVals[MAX_EFFECTS_CHANNELS] {0.0f};
    float soloLVals[MAX_EFFECTS_CHANNELS] {0.0f};
    float soloRVals[MAX_EFFECTS_CHANNELS] {0.0f};
    bool soloEngaged = false;

    // no solos on main
    effectsChannels[0] = mixer->getEffectsChannel(0);
    for (ChannelIndex i = 1; i < channelCount; i++) {
      effectsChannels[i] = mixer->getEffectsChannel(i);
      soloVals[i]  = effectsChannels[i]->getSolo();
      soloLVals[i] = effectsChannels[i]->getSoloL();
      soloRVals[i] = effectsChannels[i]->getSoloR();
      if (!soloEngaged && (soloVals[i] == 1.0f || soloLVals[i] == 1.0f || soloRVals[i] == 1.0f))
        soloEngaged = true;
    }

    // update buffer
    for (ChannelIndex i = 0; i < channelCount; i++) {
      const auto effectsChannel = effectsChannels[i];
      const bool isMain = i == 0;
      const float gain = effectsChannel->getGain();
      const float gainL = effectsChannel->getGainL();
      const float gainR = effectsChannel->getGainR();
      const float mute = effectsChannel->getMute();
      const float muteL = effectsChannel->getMuteL();
      const float muteR = effectsChannel->getMuteR();
      const float solo = isMain // no solo on main
                          ? 1.0f
                          : soloEngaged ? soloVals[i] : 1.0f;
      const float soloL = isMain
                            ? 1.0f
                            : soloEngaged ? soloLVals[i] : 1.0f;
      const float soloR = isMain
                            ? 1.0f
                            : soloEngaged ? soloRVals[i] : 1.0f;
      const float pan  = effectsChannel->getPan();
      const float panL = effectsChannel->getPanL();
      const float panR = effectsChannel->getPanR();

      effectsChannelsSettings[4 * i] = AudioCore::factorLL(
        gain, gainL, gainR,
        mute, muteL, muteR,
        solo, soloL, soloR,
        pan, panL, panR,
        channelCountF
      );
      effectsChannelsSettings[4 * i + 1] = AudioCore::factorLR(
        gain, gainL, gainR,
        mute, muteL, muteR,
        solo, soloL, soloR,
        pan, panL, panR,
        channelCountF
      );
      effectsChannelsSettings[4 * i + 2] = AudioCore::factorRL(
        gain, gainL, gainR,
        mute, muteL, muteR,
        solo, soloL, soloR,
        pan, panL, panR,
        channelCountF
      );
      effectsChannelsSettings[4 * i + 3] = AudioCore::factorRR(
        gain, gainL, gainR,
        mute, muteL, muteR,
        solo, soloL, soloR,
        pan, panL, panR,
        channelCountF
      );
    }

    // std::cout << "effectsChannelSettings = "
    //   << effectsChannelsSettings[0] << std::endl
    //   << effectsChannelsSettings[1] << std::endl
    //   << effectsChannelsSettings[2] << std::endl
    //   << effectsChannelsSettings[3] << std::endl
    //   << effectsChannelsSettings[4] << std::endl
    //   << effectsChannelsSettings[5] << std::endl
    //   << effectsChannelsSettings[6] << std::endl
    //   << effectsChannelsSettings[7] << std::endl
    //   << effectsChannelsSettings[8] << std::endl
    //   << effectsChannelsSettings[9] << std::endl
    //   << effectsChannelsSettings[10] << std::endl
    //   << effectsChannelsSettings[11] << std::endl
    //   << effectsChannelsSettings[12] << std::endl;

    // write to effectsSettings ring buffer
    if (jack_ringbuffer_write_space(audioCore->effectsChannelsSettingsRB) >= EffectsSettings_RB_SIZE - 2) {
      jack_ringbuffer_write(audioCore->effectsChannelsSettingsRB, reinterpret_cast<char*>(effectsChannelsSettings), EffectsSettings_RB_SIZE);
    }

    return OK;
  }

  bool continueRun() {
    const PlayState playState = ThreadStatics::getPlayState();
    if (playState == STOP || playState == PAUSE)
      audioCore->setPlayStateAllDecks(playState);

    if (playState == STOP)
      audioCore->setFrameIdAllDecks(0);

    if (!audioCore->currentDeck().hasValidCassetteLoaded())
      return false;

    return  playState != STOP
            && playState != PAUSE;
            //   && audioCore.frameId > -1
              // && frameId < sfInfo.frames
  }

  Result createRingBuffers() {
    fft_eq_ring_buffer_out = jack_ringbuffer_create(FFT_EQ_RING_BUFFER_SIZE);
    mixer->getSetEqRingBufferFunc()(fft_eq_ring_buffer_out);

    vu_ring_buffer_out = jack_ringbuffer_create(2 * MAX_EFFECTS_CHANNELS);
    mixer->getSetVuRingBufferFunc()(vu_ring_buffer_out);

    return OK;
  }

  Result run() {
    Logging::write(
      Info,
      "Audio::AudioPlayer::run",
      "Playing"
    );

    createRingBuffers();

    audioCore->playState = PLAY;
    audioCore->decks[audioCore->deckIndex].playState = PLAY;

    while( continueRun() ) {
      // here is our chance to pull data out of the application
      // and
      // make it accessible to our running audio callback through the audioCore obj
      std::cout << "audioplayer run playb " << audioCore->playbackBuffers[0][100] << std::endl;
      std::cout << "audioplayer run proce " << audioCore->processBuffers[0][100] << std::endl << std::endl;
      std::cout << "audioplayer run fxcha " << audioCore->effectsChannelsWriteOut[1][0][50] << std::endl << std::endl;

      if (audioCore->shouldUpdateDeckIndex()) {
        audioCore->updateDeckIndexToNext();

        const auto appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);
        const scoped_actor self{ actorSystem };
        self->anon_send(
            actor_cast<actor>(appStateManagerPtr),
           hydrate_display_a_v
        );
      }

      updateRingBuffers();

      if ( audioCore->threadId != ThreadStatics::getThreadId() ) { // fadeout, break + cleanup
        // TODO
      } else {
        audioCore->playState = ThreadStatics::getPlayState();
        ThreadStatics::setFrameId( audioCore->currentDeck().frameId );
      }

      std::this_thread::sleep_for( std::chrono::milliseconds(10) );
    } // end of while loop

    Logging::write(
      Info,
      "Audio::AudioPlayer::run",
      "Exited while loop."
    );

    if ( audioCore->threadId == ThreadStatics::getThreadId() ) { // current audio thread has reached natural end of file
      if (audioCore->playState == PLAY)
          ThreadStatics::setPlayState(STOP);
      else
          ThreadStatics::setPlayState(audioCore->playState);
    }

    if (jackClientIsActive) {
      if (jackClient->deactivate() != OK) {
        Logging::write(
          Error,
          "Audio::AudioPlayer::run",
          "An error occurred deactivating the JackClient"
        );
        return ERROR;
      }
    }

    jackClientIsActive = false;

    if (ThreadStatics::getPlayState() == STOP) {
      mixer->getUpdateProgressBarFunc()(audioCore->currentDeck().frames, 0);
      mixer->getSetEqRingBufferFunc()(nullptr);
      mixer->getSetVuRingBufferFunc()(nullptr);
    }

    Logging::write(
      Info,
      "Audio::AudioPlayer::run",
      "Run complete."
    );

    return OK;
  };
};

} // Audio
} // gj

#endif //AUDIOPLAYER_H
