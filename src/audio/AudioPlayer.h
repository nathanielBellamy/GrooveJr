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

#include "../enums/Result.h"

namespace Gj {
namespace Audio {

struct AudioPlayer {

  AudioCore& audioCore;
  Mixer* mixer;
  std::shared_ptr<JackClient> jackClient;

  float effectsChannelsSettings[2 * MAX_EFFECTS_CHANNELS]{};
  sf_count_t playbackSettingsToAudioThread[PlaybackSettingsToAudioThread_Count]{};
  sf_count_t playbackSettingsFromAudioThread[PlaybackSettingsFromAudioThread_Count]{};
  float fft_eq_buffer[FFT_EQ_RING_BUFFER_SIZE]{};
  float vu_buffer[VU_RING_BUFFER_SIZE]{};

  AudioPlayer(AudioCore& audioCore, Mixer* mixer)
    : audioCore(audioCore)
    , mixer(mixer)
    , jackClient(mixer->getGjJackClient())
    {}

  Result updateAudioDataFromMixer(
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
  mixer->getUpdateProgressBarFunc()(audioCore.frames, currentFrameId);

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

  Result play() {
    Logging::write(
      Info,
      "Audio::AudioPlayer::play",
      "Playing"
    );

    const int channelCount = mixer->getEffectsChannelsCount() + 1;

    audioCore.effectsChannelsSettingsRB = jack_ringbuffer_create(EffectsSettings_RB_SIZE);
    audioCore.playbackSettingsToAudioThreadRB = jack_ringbuffer_create(PlaybackSettingsToAudioThread_RB_SIZE);
    audioCore.playbackSettingsFromAudioThreadRB = jack_ringbuffer_create(PlaybackSettingsFromAudioThread_RB_SIZE);

    audioCore.fft_eq_ring_buffer = jack_ringbuffer_create(FFT_EQ_RING_BUFFER_SIZE);

    jack_ringbuffer_t* fft_eq_ring_buffer_out = jack_ringbuffer_create(FFT_EQ_RING_BUFFER_SIZE);
    mixer->getSetEqRingBufferFunc()(fft_eq_ring_buffer_out);

    audioCore.vu_ring_buffer = jack_ringbuffer_create(2 * MAX_EFFECTS_CHANNELS);

    jack_ringbuffer_t* vu_ring_buffer_out = jack_ringbuffer_create(2 * MAX_EFFECTS_CHANNELS);
    mixer->getSetVuRingBufferFunc()(vu_ring_buffer_out);

    ThreadStatics::setReadComplete(false);

    while(
            audioCore.playState != STOP
              && audioCore.playState != PAUSE
              && audioCore.frameId > -1
              // && frameId < sfInfo.frames
    ) {
      // hold thread open until stopped

      // here is our chance to pull data out of the application
      // and
      // make it accessible to our running audio callback through the audioData obj

      // TODO: pass readComplete thru ring buffer
      if (audioCore.readComplete) { // reached end of input file
          ThreadStatics::setPlayState(STOP);
          Logging::write(
            Info,
            "Audio::Cassette::play",
            "Read complete"
          );
          break;
      }

      // if (audioData.fadeIn > 0.01) {
      //     audioData.fadeIn -= 0.01;
      //     audioData.volume += 0.01;
      // }

      updateAudioDataFromMixer(
        audioCore.effectsChannelsSettingsRB,
        audioCore.playbackSettingsToAudioThreadRB,
        audioCore.playbackSettingsFromAudioThreadRB,
        audioCore.fft_eq_ring_buffer,
        fft_eq_ring_buffer_out,
        audioCore.vu_ring_buffer,
        vu_ring_buffer_out,
        channelCount
      );

      if ( audioCore.threadId != ThreadStatics::getThreadId() ) { // fadeout, break + cleanup
        // if (audioData.fadeOut < 0.01) { // break + cleanup
        //     break;
        // } else { // continue fading out
        //     audioData.volume -= 0.001;
        //     audioData.fadeOut -= 0.001;
        // }
      } else {
        audioCore.playbackSpeed = ThreadStatics::getPlaybackSpeed();
        audioCore.playState = ThreadStatics::getPlayState();
        ThreadStatics::setFrameId( audioCore.frameId );
      }

      std::this_thread::sleep_for( std::chrono::milliseconds(10) );
    } // end of while loop

    if ( audioCore.threadId == ThreadStatics::getThreadId() ) { // current audio thread has reached natural end of file
      if (audioCore.playState == PLAY)
          ThreadStatics::setPlayState(STOP);
      else
          ThreadStatics::setPlayState(audioCore.playState);
      ThreadStatics::setReadComplete(true);
    }

    // if (jackClientIsActive) {
    //   if (jackClient->deactivate() != OK) {
    //     Logging::write(
    //       Error,
    //       "Audio::Cassette::play",
    //       "An error occurred deactivating the JackClient"
    //     );
    //   }
    // }

    // jackClientIsActive = false;

    if (ThreadStatics::getPlayState() == STOP) {
      mixer->getUpdateProgressBarFunc()(audioCore.frames, 0);
      mixer->getSetEqRingBufferFunc()(nullptr);
      mixer->getSetVuRingBufferFunc()(nullptr);
    }

    return OK;
  };
};

} // Audio
} // gj

#endif //AUDIOPLAYER_H
