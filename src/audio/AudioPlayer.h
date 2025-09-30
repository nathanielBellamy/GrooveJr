//
// Created by ns on 9/20/25.
//

#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <memory>
#include <thread>
#include <chrono>

#include "AudioCore.h"
#include "Constants.h"
#include "Mixer.h"
#include "ThreadStatics.h"

#include "../AppState.h"
#include "../enums/Result.h"

namespace Gj {
namespace Audio {

struct AudioPlayer {

  long threadId;
  AudioCore* audioCore;
  Mixer* mixer;
  AppState* gAppState;
  std::shared_ptr<JackClient> jackClient;

  bool jackClientIsActive = false;
  float* effectsChannelsWriteOutBuffer;
  float effectsChannelsSettings[2 * MAX_EFFECTS_CHANNELS]{};
  sf_count_t playbackSettingsToAudioThread[PlaybackSettingsToAudioThread_Count]{};
  sf_count_t playbackSettingsFromAudioThread[PlaybackSettingsFromAudioThread_Count]{};
  float fft_eq_buffer[FFT_EQ_RING_BUFFER_SIZE]{};
  float vu_buffer[VU_RING_BUFFER_SIZE]{};

  AudioPlayer(AudioCore* audioCore, Mixer* mixer, AppState* gAppState)
    : threadId(ThreadStatics::incrThreadId())
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
    delete[] effectsChannelsWriteOutBuffer;

    if (jackClientIsActive) {
      if (jackClient->deactivate() != OK) {
        Logging::write(
          Error,
          "Audio::AudioPlayer::cleanup",
          "Unable to deactivate jackClient"
        );
      }
    }
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
    for (int i = 0; i < mixer->getEffectsChannelsCount() + 1; i++) {
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

    audioCore->setChannelCount(
      static_cast<float>(mixer->getEffectsChannelsCount())
    );
    for (const auto effectsChannel : mixer->getEffectsChannels()) {
      const auto effectsChannelIdx = effectsChannel->getIndex();
      audioCore->effectsChannelsProcessData[effectsChannelIdx].effectCount = effectsChannel->effectCount();
      audioCore->effectsChannelsSettings[2 * effectsChannelIdx] = effectsChannel->getGain();
      audioCore->effectsChannelsSettings[2 * effectsChannelIdx + 1] = effectsChannel->getPan();

      for (int pluginIdx = 0; pluginIdx < effectsChannel->effectCount(); pluginIdx++) {
        const auto plugin = effectsChannel->getPluginAtIdx(pluginIdx);
        audioCore->effectsChannelsProcessData[effectsChannelIdx].processFuncs[pluginIdx] =
          [ObjectPtr = plugin->audioHost->audioClient](auto && PH1, auto && PH2) { return ObjectPtr->process(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2)); };

        if (effectsChannelIdx == 0) { // main channel
          audioCore->effectsChannelsProcessData[0].buffers[pluginIdx] = {
            audioCore->processBuffers,
            2,
            audioCore->processBuffers,
            2,
            static_cast<int32_t>(gAppState->getAudioFramesPerBuffer())
          };
        } else {
          audioCore->effectsChannelsProcessData[effectsChannelIdx].buffers[pluginIdx] = getPluginBuffers(effectsChannel, effectsChannelIdx, pluginIdx, audioCore);
        }
      }
    }

    Logging::write(
      Info,
      "Audio::AudioPlayer::setupAudioCore",
      "Setup AudioCore Effects processing."
    );

    Logging::write(
      Info,
      "Audio::AudioPlayer::setupAudioCore",
      "Successfully setup AudioCore."
    );

    return 0;
  }

  IAudioClient::Buffers getPluginBuffers(const Effects::EffectsChannel* effectsChannel, const int channelIdx, const int pluginIdx, AudioCore* audioCore) const {
    const auto audioFramesPerBuffer = static_cast<int32_t>(gAppState->getAudioFramesPerBuffer());

    // NOTE: input buffers will be updated to audioCore->playbackBuffers when pluginIdx = 0 in JackClient::processCallback
    if (const int effectsCount = effectsChannel->effectCount(); pluginIdx == effectsCount - 1) {
      const auto writeOut = const_cast<float**>(audioCore->effectsChannelsWriteOut[channelIdx]);
      return {
        audioCore->processBuffers,
        2,
        writeOut,
        2,
        audioFramesPerBuffer
      };
    }

    return {
      audioCore->processBuffers,
      2,
      audioCore->processBuffers,
      2,
      audioFramesPerBuffer
    };
  }

  Result updateRingBuffers(
    jack_ringbuffer_t* effectsChannelsSettingsRB,
    jack_ringbuffer_t* playbackSettingsToAudioThreadRB,
    jack_ringbuffer_t* playbackSettingsFromAudioThreadRB,
    jack_ringbuffer_t* fft_eq_ring_buffer,
    jack_ringbuffer_t* fft_eq_ring_buffer_out,
    jack_ringbuffer_t* vu_ring_buffer,
    jack_ringbuffer_t* vu_ring_buffer_out,
    int channelCount
  ) {

    // read vu_ring_buffer
    if (jack_ringbuffer_read_space(vu_ring_buffer) > VU_RING_BUFFER_SIZE - 2) {
      jack_ringbuffer_read(
        vu_ring_buffer,
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
    if (jack_ringbuffer_read_space(fft_eq_ring_buffer) > FFT_EQ_RING_BUFFER_SIZE - 2) {
      jack_ringbuffer_read(
        fft_eq_ring_buffer,
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
    if (jack_ringbuffer_read_space(playbackSettingsFromAudioThreadRB) > PlaybackSettingsFromAudioThread_RB_SIZE - 2) {
      jack_ringbuffer_read(
        playbackSettingsFromAudioThreadRB,
        reinterpret_cast<char*>(playbackSettingsFromAudioThread),
        PlaybackSettingsFromAudioThread_RB_SIZE
      );
    }

    const sf_count_t currentFrameId = playbackSettingsFromAudioThread[1];
    mixer->getUpdateProgressBarFunc()(audioCore->currentDeck().frames, currentFrameId);

    playbackSettingsToAudioThread[0] = 0;
    playbackSettingsToAudioThread[1] = 0;
    playbackSettingsToAudioThread[2] = ThreadStatics::getPlaybackSpeed();

    if (ThreadStatics::getUserSettingFrameId()) {
      const sf_count_t newFrameId = ThreadStatics::getFrameId();
      playbackSettingsToAudioThread[0] = 1;
      playbackSettingsToAudioThread[1] = newFrameId;

      ThreadStatics::setUserSettingFrameId(false);
    }

    // write to playbackSettingsToAudioThread ring buffer
    if (jack_ringbuffer_write_space(playbackSettingsToAudioThreadRB) > PlaybackSettingsToAudioThread_RB_SIZE - 2) {
      jack_ringbuffer_write(
        playbackSettingsToAudioThreadRB,
        reinterpret_cast<char*>(playbackSettingsToAudioThread),
        PlaybackSettingsToAudioThread_RB_SIZE
      );
    }

    const float channelCountF = static_cast<float>(channelCount);

    Effects::EffectsChannel* effectsChannels[MAX_EFFECTS_CHANNELS] {nullptr};
    float soloVals[MAX_EFFECTS_CHANNELS] {0.0f};
    float soloLVals[MAX_EFFECTS_CHANNELS] {0.0f};
    float soloRVals[MAX_EFFECTS_CHANNELS] {0.0f};
    bool soloEngaged = false;

    // no solos on main
    effectsChannels[0] = mixer->getEffectsChannel(0);
    for (int i = 1; i < channelCount; i++) {
      effectsChannels[i] = mixer->getEffectsChannel(i);
      soloVals[i]  = effectsChannels[i]->getSolo();
      soloLVals[i] = effectsChannels[i]->getSoloL();
      soloRVals[i] = effectsChannels[i]->getSoloR();
      if (!soloEngaged && (soloVals[i] == 1.0f || soloLVals[i] == 1.0f || soloRVals[i] == 1.0f))
        soloEngaged = true;
    }

    // update buffer
    for (int i = 0; i < channelCount; i++) {
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

    // write to effectsSettings ring buffer
    if (jack_ringbuffer_write_space(effectsChannelsSettingsRB) >= EffectsSettings_RB_SIZE) {
      jack_ringbuffer_write(effectsChannelsSettingsRB, reinterpret_cast<char*>(effectsChannelsSettings), EffectsSettings_RB_SIZE);
    }

    return OK;
  }

  bool continueRun() {
    const PlayState playState = ThreadStatics::getPlayState();
    return  playState != STOP
            && playState != PAUSE;
            //   && audioCore.frameId > -1
              // && frameId < sfInfo.frames
  }

  Result run() {
    Logging::write(
      Info,
      "Audio::AudioPlayer::run",
      "Playing"
    );

    const int channelCount = mixer->getEffectsChannelsCount() + 1;

    audioCore->effectsChannelsSettingsRB = jack_ringbuffer_create(EffectsSettings_RB_SIZE);
    audioCore->playbackSettingsToAudioThreadRB = jack_ringbuffer_create(PlaybackSettingsToAudioThread_RB_SIZE);
    audioCore->playbackSettingsFromAudioThreadRB = jack_ringbuffer_create(PlaybackSettingsFromAudioThread_RB_SIZE);

    audioCore->fft_eq_ring_buffer = jack_ringbuffer_create(FFT_EQ_RING_BUFFER_SIZE);

    jack_ringbuffer_t* fft_eq_ring_buffer_out = jack_ringbuffer_create(FFT_EQ_RING_BUFFER_SIZE);
    mixer->getSetEqRingBufferFunc()(fft_eq_ring_buffer_out);

    audioCore->vu_ring_buffer = jack_ringbuffer_create(2 * MAX_EFFECTS_CHANNELS);

    jack_ringbuffer_t* vu_ring_buffer_out = jack_ringbuffer_create(2 * MAX_EFFECTS_CHANNELS);
    mixer->getSetVuRingBufferFunc()(vu_ring_buffer_out);

    audioCore->playState = PLAY;
    ThreadStatics::setReadComplete(false);

    while( continueRun() ) {
      // here is our chance to pull data out of the application
      // and
      // make it accessible to our running audio callback through the audioCore obj

      AudioDeck& currentDeck = audioCore->currentDeck();
      const int nextDeckIndex = (currentDeck.deckIndex + 1) % AUDIO_CORE_DECK_COUNT;
      const int prevDeckIndex = (currentDeck.deckIndex - 1) % AUDIO_CORE_DECK_COUNT;

      std::cout << "currentDeck index: " << currentDeck.deckIndex << std::endl;

      if (currentDeck.isCrossfadeStart()) {
        audioCore->decks[prevDeckIndex].fadeOut -= 0.01f;
        currentDeck.fadeIn += 0.01f;
      }

      // todo: finesse crossfade
      // todo: pass thru ring buffer, perhaps with readComplete
      if (currentDeck.isCrossfadeEnd()) {
        std::cout << "crossfade end" << std::endl;
        if (!audioCore->decks[nextDeckIndex].active)
          audioCore->decks[nextDeckIndex].active = true;
        audioCore->decks[nextDeckIndex].fadeIn += 0.01f;
        currentDeck.fadeOut -= 0.01f;
      }

      // TODO: pass readComplete thru ring buffer
      if (currentDeck.readComplete) { // reached end of input file
          std::cout << "read complete" << std::endl;
          // currentDeck.setReadComplete(false);
          // audioCore->decks[nextDeckIndex].setReadComplete(false);
          audioCore->incrDeckIndex();
          Logging::write(
            Info,
            "Audio::AudioPlayer::run",
            "Read complete"
          );
      }

      // if (audioCore.fadeIn > 0.01) {
      //     audioCore.fadeIn -= 0.01;
      //     audioCore.volume += 0.01;
      // }

      updateRingBuffers(
        audioCore->effectsChannelsSettingsRB,
        audioCore->playbackSettingsToAudioThreadRB,
        audioCore->playbackSettingsFromAudioThreadRB,
        audioCore->fft_eq_ring_buffer,
        fft_eq_ring_buffer_out,
        audioCore->vu_ring_buffer,
        vu_ring_buffer_out,
        channelCount
      );

      if ( audioCore->threadId != ThreadStatics::getThreadId() ) { // fadeout, break + cleanup
        // if (audioCore.fadeOut < 0.01) { // break + cleanup
        //     break;
        // } else { // continue fading out
        //     audioCore.volume -= 0.001;
        //     audioCore.fadeOut -= 0.001;
        // }
      } else {
        audioCore->playbackSpeed = ThreadStatics::getPlaybackSpeed();
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
      ThreadStatics::setReadComplete(true);
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

    return OK;
  };
};

} // Audio
} // gj

#endif //AUDIOPLAYER_H
