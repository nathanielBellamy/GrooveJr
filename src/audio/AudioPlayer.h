//
// Created by ns on 9/20/25.
//

#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <array>
#include <memory>
#include <thread>
#include <chrono>

#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"

#include "AudioCore.h"
#include "Constants.h"
#include "mixer/Core.h"
#include "ThreadStatics.h"
#include "BufferIndeces.h"

#include "../AppState.h"
#include "../enums/Result.h"

namespace Gj {
namespace Audio {
using namespace caf;

struct AudioPlayer {
  long threadId;

  actor_system& actorSystem;

  AudioCore* audioCore;

  Mixer::Core* mixer;

  AppState* gAppState;

  std::shared_ptr<JackClient> jackClient;

  bool jackClientIsActive = false;

  float mixerChannelsSettings[2 * MAX_MIXER_CHANNELS]{};

  sf_count_t playbackSettingsToAudioThread[PlaybackSettingsToAudioThread_Count]{};

  sf_count_t playbackSettingsFromAudioThread[PlaybackSettingsFromAudioThread_Count]{};

  float fft_eq_buffer[FFT_EQ_RING_BUFFER_SIZE]{};

  jack_ringbuffer_t* fft_eq_ring_buffer_out;

  float vu_buffer[VU_RING_BUFFER_SIZE]{};

  jack_ringbuffer_t* vu_ring_buffer_out;

  Mixer::ChannelProcessData mixerChannelsProcessData[MAX_MIXER_CHANNELS]{};

  AudioPlayer(actor_system& actorSystem, AudioCore* audioCore, Mixer::Core* mixer, AppState* gAppState)
  : threadId(ThreadStatics::incrThreadId())
    , actorSystem(actorSystem)
    , audioCore(audioCore)
    , mixer(mixer)
    , gAppState(gAppState)
    , jackClient(mixer->getGjJackClient()) {
    audioCore->clearBuffers();
    populateMixerChannelsProcessData();

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
      "Setting up AudioCore for threadId: " + std::to_string(threadId)
    );

    createRingBuffers();

    if (threadId != ThreadStatics::getThreadId()) {
      Logging::write(
        Warning,
        "Audio::AudioPlayer::setupAudioCore",
        "Attempted to create duplicate AudioThread and thus duplicate AudioCore with ThreadID: " +
        std::to_string(threadId)
      );
      return 1;
    }

    // update plugins with info about audio to be processed
    const SF_INFO sfInfo = audioCore->currentDeck().decoratedAudioFile->audioFile.sfInfo;
    if (mixer->setSampleRate(sfInfo.samplerate) != OK)
      Logging::write(
        Warning,
        "Audio::AudioPlayer::setupAudioCore",
        "Unable to set sample rate: " + std::to_string(sfInfo.samplerate)
      );

    ThreadStatics::setFrames(audioCore->currentDeck().frames);

    audioCore->fillPlaybackBuffer = &JackClient::fillPlaybackBuffer;
    audioCore->crossfade = gAppState->getCrossfade();

    Logging::write(
      Info,
      "Audio::AudioPlayer::setupAudioCore",
      "Successfully setup AudioData buffers."
    );

    Logging::write(
      Info,
      "Audio::AudioPlayer::setupAudioCore",
      "Setting up AudioCore Plugin processing..."
    );

    audioCore->setChannelCount(mixer->getTotalChannelsCount());

    Logging::write(
      Info,
      "Audio::AudioPlayer::setupAudioCore",
      "Setup AudioCore Plugin processing. Successfully setup AudioCore."
    );

    return 0;
  }

  Result populateMixerChannelsProcessData() {
    return mixer->forEachChannel([this](Mixer::Channel* ch, ChannelIndex chIdx) {
      auto processData = ch->getProcessData();
      processData.pluginCount = ch->pluginCount();

      ch->forEachPlugin(
        [this, &ch, &processData](const Plugins::Vst3::Plugin* plugin,
                                  const PluginIndex pluginIdx) {
          if (processData.processingEnabledFor[pluginIdx]) {
            processData.processFuncs[pluginIdx] =
                [audioClient = plugin->audioHost->audioClient](IAudioClient::Buffers& buffers,
                                                               const jack_nframes_t nFrames) {
                  return audioClient->process(
                    buffers,
                    nFrames
                  );
                };
          } else if (pluginIdx == 0 || pluginIdx == MAX_PLUGINS_PER_CHANNEL - 1) {
            processData.processFuncs[pluginIdx] =
                [](const IAudioClient::Buffers& buffers, const jack_nframes_t nFrames) {
                  for (int audioChannel = 0; audioChannel < 2; ++audioChannel)
                    for (auto i = 0; i < nFrames; ++i)
                      buffers.outputs[audioChannel][i] = buffers.inputs[audioChannel][i];

                  return true;
                };
          } else {
            processData.processFuncs[pluginIdx] =
                [](const IAudioClient::Buffers&, const jack_nframes_t) {
                  return true;
                };
          }

          processData.buffers[pluginIdx] =
              getPluginBuffers(ch, pluginIdx);
          ch->setProcessData(processData);
          mixerChannelsProcessData[ch->getIndex()] = processData;
        });
    });
  }

  IAudioClient::Buffers getPluginBuffers(
    const Mixer::Channel* channel,
    const PluginIndex pluginIdx
  ) const {
    const auto audioFramesPerBuffer = static_cast<int32_t>(gAppState->getAudioFramesPerBuffer());

    // - each channel gets processed into it's mixerChannelWriteOut
    // - the channels are then summed down into the processBuffers
    // - the main channel then acts upon the processBuffers

    if (channel->getIndex() == 0) // main
      // all plugins on main operate on processBuffers
      return {
        audioCore->processBuffers,
        2,
        audioCore->processBuffers,
        2,
        audioFramesPerBuffer
      };

    // non-main channel from here on
    if (pluginIdx == 0)
      return {
        audioCore->playbackBuffers,
        2,
        audioCore->processBuffers,
        2,
        audioFramesPerBuffer
      };

    if (pluginIdx == MAX_PLUGINS_PER_CHANNEL - 1) {
      const auto writeOut = const_cast<float**>(audioCore->mixerChannelsWriteOut[channel->getIndex()]);
      return {
        audioCore->processBuffers,
        2,
        writeOut,
        2,
        audioFramesPerBuffer
      };
    }

    // non-first and non-last plugin of multiple plugins so goes from process to process
    return {
      audioCore->processBuffers,
      2,
      audioCore->processBuffers,
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
    if (jack_ringbuffer_read_space(audioCore->playbackSettingsFromAudioThreadRB) >
        PlaybackSettingsFromAudioThread_RB_SIZE - 2)
      jack_ringbuffer_read(
        audioCore->playbackSettingsFromAudioThreadRB,
        reinterpret_cast<char*>(playbackSettingsFromAudioThread),
        PlaybackSettingsFromAudioThread_RB_SIZE
      );

    // TODO:
    // - update mixerChannelsProcessData write to audioCore->mixerChannelsSettingsProcessDataRB
    // - read audioCore->mixerChannelsProcessDataRB into audioCore->mixerChannelsProcessData
    // - process
    // - remove disabling of channel/plugin add/remove buttons during playback

    // - ok so we need the mixer to know when the audiothread is done with the func ref
    // - so we need:
    // -   mixer->requestProcessChange()
    // -   audioThread sets flag saying, ok don't need that/those process func/s anymore
    // -   mixer->executeProcessChange()
    // -   mixer sets flag saying processing is good to go
    // -   audioThread re-instates processing
    // updateProcessDataRingBuffer()

    if (jack_ringbuffer_write_space(audioCore->mixerChannelsProcessDataRB) > MixerChannelsProcessData_RB_SIZE - 2)
      jack_ringbuffer_write(
        audioCore->mixerChannelsProcessDataRB,
        reinterpret_cast<char*>(mixerChannelsProcessData),
        MixerChannelsProcessData_RB_SIZE
      );

    // std::cout << " DEBUG VALUE FROM AUDIO THREAD " << playbackSettingsFromAudioThread[BfrIdx::PSFAT::DEBUG_VALUE] << std::endl;
    const sf_count_t currentFrameId = playbackSettingsFromAudioThread[BfrIdx::PSFAT::CURRENT_FRAME_ID];
    mixer->getUpdateProgressBarFunc()(audioCore->currentDeck().frames, currentFrameId);

    playbackSettingsToAudioThread[BfrIdx::PSTAT::USER_SETTING_FRAME_ID_FLAG] = 0;
    playbackSettingsToAudioThread[BfrIdx::PSTAT::NEW_FRAME_ID] = 0;
    playbackSettingsToAudioThread[BfrIdx::PSTAT::PLAYBACK_SPEED] = static_cast<sf_count_t>(
      std::floor(gAppState->getScene().playbackSpeed * 100.0f));

    if (ThreadStatics::getUserSettingFrameId()) {
      const sf_count_t newFrameId = ThreadStatics::getFrameId();
      playbackSettingsToAudioThread[BfrIdx::PSTAT::USER_SETTING_FRAME_ID_FLAG] = 1;
      playbackSettingsToAudioThread[BfrIdx::PSTAT::NEW_FRAME_ID] = newFrameId;

      ThreadStatics::setUserSettingFrameId(false);
    }

    // write to playbackSettingsToAudioThread ring buffer
    if (jack_ringbuffer_write_space(audioCore->playbackSettingsToAudioThreadRB) >
        PlaybackSettingsToAudioThread_RB_SIZE
        - 2) {
      jack_ringbuffer_write(
        audioCore->playbackSettingsToAudioThreadRB,
        reinterpret_cast<char*>(playbackSettingsToAudioThread),
        PlaybackSettingsToAudioThread_RB_SIZE
      );
    }

    const ChannelIndex channelCount = mixer->getTotalChannelsCount();
    if (channelCount < 2) {
      Logging::write(
        Error,
        "Audio::AudioPlayer::updateRingBuffers()",
        "Too few channels."
      );
      return ERROR;
    }
    const float channelCountF = static_cast<float>(channelCount);

    Mixer::Channel* channels[MAX_MIXER_CHANNELS]{nullptr};
    float soloVals[MAX_MIXER_CHANNELS]{0.0f};
    float soloLVals[MAX_MIXER_CHANNELS]{0.0f};
    float soloRVals[MAX_MIXER_CHANNELS]{0.0f};
    bool soloEngaged = false;

    // TODO: mixer->forEachChannel
    // no solos on main
    channels[0] = mixer->getChannel(0).value();
    for (ChannelIndex i = 1; i < channelCount; i++) {
      channels[i] = mixer->getChannel(i).value();
      soloVals[i] = channels[i]->getSolo();
      soloLVals[i] = channels[i]->getSoloL();
      soloRVals[i] = channels[i]->getSoloR();
      if (!soloEngaged && (soloVals[i] == 1.0f || soloLVals[i] == 1.0f || soloRVals[i] == 1.0f))
        soloEngaged = true;
    }

    // update buffer
    for (ChannelIndex chIdx = 0; chIdx < channelCount; chIdx++) {
      const auto channel = channels[chIdx];
      const bool isMain = chIdx == 0;
      const float gain = channel->getGain();
      const float gainL = channel->getGainL();
      const float gainR = channel->getGainR();
      const float mute = channel->getMute();
      const float muteL = channel->getMuteL();
      const float muteR = channel->getMuteR();
      const float solo = isMain // no solo on main
                           ? 1.0f
                           : soloEngaged
                               ? soloVals[chIdx]
                               : 1.0f;
      const float soloL = isMain
                            ? 1.0f
                            : soloEngaged
                                ? soloLVals[chIdx]
                                : 1.0f;
      const float soloR = isMain
                            ? 1.0f
                            : soloEngaged
                                ? soloRVals[chIdx]
                                : 1.0f;
      const float pan = channel->getPan();
      const float panL = channel->getPanL();
      const float panR = channel->getPanR();

      mixerChannelsSettings[BfrIdx::ECS::factorLL(chIdx)] = AudioCore::factorLL(
        gain, gainL, gainR,
        mute, muteL, muteR,
        solo, soloL, soloR,
        pan, panL, panR,
        channelCountF
      );
      mixerChannelsSettings[BfrIdx::ECS::factorLR(chIdx)] = AudioCore::factorLR(
        gain, gainL, gainR,
        mute, muteL, muteR,
        solo, soloL, soloR,
        pan, panL, panR,
        channelCountF
      );
      mixerChannelsSettings[BfrIdx::ECS::factorRL(chIdx)] = AudioCore::factorRL(
        gain, gainL, gainR,
        mute, muteL, muteR,
        solo, soloL, soloR,
        pan, panL, panR,
        channelCountF
      );
      mixerChannelsSettings[BfrIdx::ECS::factorRR(chIdx)] = AudioCore::factorRR(
        gain, gainL, gainR,
        mute, muteL, muteR,
        solo, soloL, soloR,
        pan, panL, panR,
        channelCountF
      );
    }

    // write to mixerChannelSettings ring buffer
    if (jack_ringbuffer_write_space(audioCore->mixerChannelsSettingsRB) >= ChannelsSettings_RB_SIZE - 2) {
      jack_ringbuffer_write(audioCore->mixerChannelsSettingsRB, reinterpret_cast<char*>(mixerChannelsSettings),
                            ChannelsSettings_RB_SIZE);
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

    return playState != STOP
           && playState != PAUSE;
    //   && audioCore.frameId > -1
    // && frameId < sfInfo.frames
  }

  Result createRingBuffers() {
    fft_eq_ring_buffer_out = jack_ringbuffer_create(FFT_EQ_RING_BUFFER_SIZE);
    mixer->getSetEqRingBufferFunc()(fft_eq_ring_buffer_out);

    vu_ring_buffer_out = jack_ringbuffer_create(2 * MAX_MIXER_CHANNELS);
    mixer->getSetVuRingBufferFunc()(vu_ring_buffer_out);

    return OK;
  }

  Result run() {
    Logging::write(
      Info,
      "Audio::AudioPlayer::run",
      "Playing"
    );

    audioCore->decks[audioCore->deckIndex].playState = PLAY;

    while (continueRun()) {
      // std::cout << "audioplayer run playb " << audioCore->playbackBuffers[0][100] << std::endl;
      // std::cout << "audioplayer run proce " << audioCore->processBuffers[0][100] << std::endl << std::endl;
      // std::cout << "audioplayer run fxcha " << audioCore->mixerChannelsChannelsWriteOut[1][0][50] << std::endl << std::endl;

      if (audioCore->shouldUpdateDeckIndex()) {
        audioCore->updateDeckIndexToNext();

        const auto appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);
        const scoped_actor self{actorSystem};
        self->anon_send(
          actor_cast<actor>(appStateManagerPtr),
          hydrate_display_a_v
        );
      }

      populateMixerChannelsProcessData();
      updateRingBuffers();

      if (audioCore->threadId != ThreadStatics::getThreadId()) {
        // fadeout, break + cleanup
        // TODO
      } else {
        ThreadStatics::setFrameId(audioCore->currentDeck().frameId);
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } // end of while loop

    Logging::write(
      Info,
      "Audio::AudioPlayer::run",
      "Exited while loop."
    );

    if (audioCore->threadId == ThreadStatics::getThreadId()) {
      // current audio thread has reached natural end of file
      // TODO:
      // if (audioCore->playState == PLAY)
      //     ThreadStatics::setPlayState(STOP);
      // else
      //     ThreadStatics::setPlayState(audioCore->playState);
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
