#ifndef GJAUDIOAUDIODATA_H
#define GJAUDIOAUDIODATA_H

#include <atomic>

#include <fftw3.h>

#include <sndfile.hh>
#include "../enums/PlayState.h"
#include "./effects/EffectsChannelProcessData.h"
#include "./Constants.h"
#include "./AudioDeck.h"
#include "ThreadStatics.h"

#include <jack/ringbuffer.h>

#include "Cassette.h"
#include "../AppState.h"
#include "../Logging.h"
#include "../enums/Result.h"

namespace Gj {
namespace Audio {
  constexpr int AUDIO_CORE_DECK_COUNT = 3;

struct AudioCore {
  long                             threadId;
  AppState*                        gAppState;
  sf_count_t                       crossfade = 0;
  AudioDeck                        decks[AUDIO_CORE_DECK_COUNT]{ AudioDeck(0, gAppState), AudioDeck(1, gAppState), AudioDeck(2, gAppState) };
  int                              deckIndex = 0;
  sf_count_t                       frameAdvance;
  std::atomic<PlayState>           playState = STOP;
  float                            playbackSpeed;
  float                            fft_eq_time[2][FFT_EQ_TIME_SIZE]{ 0.0 };
  fftwf_complex                    fft_eq_freq[2][FFT_EQ_FREQ_SIZE]{};
  float                            fft_eq_write_out_buffer[2 * FFT_EQ_FREQ_SIZE]{};
  fftwf_plan                       fft_eq_0_plan_r2c;
  fftwf_plan                       fft_eq_1_plan_r2c;
  jack_ringbuffer_t*               fft_eq_ring_buffer{nullptr};
  float                            fft_pv_time[FFT_PV_TIME_SIZE]{};
  float                            fft_pv_ola_buffer[2][FFT_PV_OLA_BUFFER_SIZE]{};
  fftwf_complex                    fft_pv_freq[FFT_PV_FREQ_SIZE]{};
  fftwf_complex                    fft_pv_freq_shift[FFT_PV_FREQ_SIZE]{};
  float                            fft_pv_phase_sum[2][FFT_PV_FREQ_SIZE]{};
  float                            fft_pv_phase_prev[2][FFT_PV_FREQ_SIZE]{};
  float                            fft_pv_phase_prev_init[2][FFT_PV_FREQ_SIZE]{};
  fftwf_plan                       fft_pv_plan_r2c;
  fftwf_plan                       fft_pv_plan_c2r;
  float                            vu_buffer_in[VU_RING_BUFFER_SIZE]{ 0.0f };
  float                            vu_buffer[VU_RING_BUFFER_SIZE]{ 0.0f };
  jack_ringbuffer_t*               vu_ring_buffer{nullptr};
  float*                           processBuffers[2]{nullptr, nullptr};
  float                            processBuffersBuffer[MAX_AUDIO_FRAMES_PER_BUFFER * 2]{0.0f};
  float                            playbackBuffersPre[2][MAX_AUDIO_FRAMES_PER_BUFFER]{};
  float*                           playbackBuffers[2]{nullptr, nullptr};
  float                            playbackBuffersBuffer[MAX_AUDIO_FRAMES_PER_BUFFER * 2]{0.0f};
  float                            fftFreqBuffersBuffer[MAX_AUDIO_FRAMES_PER_BUFFER * 2]{0.0f};
  float                            channelCount;
  int                              effectsChannelCount;
  float                            effectsChannelsWriteOutBuffer[2 * MAX_AUDIO_FRAMES_PER_BUFFER * MAX_EFFECTS_CHANNELS]{0.0f};
  std::array<Effects::EffectsChannelProcessData, MAX_EFFECTS_CHANNELS> effectsChannelsProcessData{};
                                   // eCS[4k]   = {factorLL channel k}
                                   // eCS[4k+1] = {factorLR channel k}
                                   // eCS[4k+2] = {factorRL channel k}
                                   // eCS[4k+3] = {factorRR channel k}
  float                            effectsChannelsSettings[MAX_EFFECTS_CHANNELS * 4]{0.0f};
  jack_ringbuffer_t*               effectsChannelsSettingsRB{nullptr};
  sf_count_t                       playbackSettingsToAudioThread[PlaybackSettingsToAudioThread_Count]{0};
  jack_ringbuffer_t*               playbackSettingsToAudioThreadRB{nullptr};
                                   // pSTATRB[0] = userSettingFrameId bool
                                   // pSTATRB[1] = newFrameId         sf_count_t
                                   // pSTATRB[2] = newPlaybackSpeed   sf_count_t
  sf_count_t                       playbackSettingsFromAudioThread[PlaybackSettingsFromAudioThread_Count]{0};
  jack_ringbuffer_t*               playbackSettingsFromAudioThreadRB{nullptr};
                                   // pSFATRB[0] = unused (debug)
                                   // pSFATRB[1] = frameId sf_count_t
  float*                           effectsChannelsWriteOut[MAX_EFFECTS_CHANNELS][2]{};
  std::function<int(AudioCore*, sf_count_t, jack_nframes_t)> fillPlaybackBuffer;

  AudioCore(AppState* gAppState)
    : gAppState(gAppState)
    {
    Logging::write(
      Info,
      "Audio::AudioCore::AudioCore()",
      "AudioCore ctor"
    );

    init();
  }

  ~AudioCore() {
    Logging::write(
      Info,
      "Audio::AudioCore::~AudioCore",
      "Destroying AudioCore"
    );

    fftwf_destroy_plan(fft_eq_0_plan_r2c);
    fftwf_destroy_plan(fft_eq_1_plan_r2c);
    fftwf_destroy_plan(fft_pv_plan_r2c);
    fftwf_destroy_plan(fft_pv_plan_c2r);

    Logging::write(
      Info,
      "Audio::AudioCore::~AudioCore",
      "Destroyed AudioCore"
    );
  }

  Result init() {
    Logging::write(
      Info,
      "Audio::AudioCore::init",
      "Initializing AudioCore"
    );

    processBuffers[0] = &processBuffersBuffer[0];
    processBuffers[1] = &processBuffersBuffer[MAX_AUDIO_FRAMES_PER_BUFFER];

    playbackBuffers[0] = &playbackBuffersBuffer[0];
    playbackBuffers[1] = &playbackBuffersBuffer[MAX_AUDIO_FRAMES_PER_BUFFER];

    fft_eq_0_plan_r2c = fftwf_plan_dft_r2c_1d(FFT_EQ_TIME_SIZE, fft_eq_time[0], fft_eq_freq[0], FFTW_ESTIMATE);
    fft_eq_1_plan_r2c = fftwf_plan_dft_r2c_1d(FFT_EQ_TIME_SIZE, fft_eq_time[1], fft_eq_freq[1], FFTW_ESTIMATE);

    fft_pv_plan_r2c = fftwf_plan_dft_r2c_1d(FFT_PV_TIME_SIZE, fft_pv_time, fft_pv_freq, FFTW_ESTIMATE);
    fft_pv_plan_c2r = fftwf_plan_dft_c2r_1d(FFT_PV_FREQ_SIZE, fft_pv_freq_shift, fft_pv_time, FFTW_ESTIMATE);

    for (int i = 0; i < MAX_EFFECTS_CHANNELS; i++) {
      effectsChannelsWriteOut[i][0] = effectsChannelsWriteOutBuffer + 2 * i * MAX_AUDIO_FRAMES_PER_BUFFER;
      effectsChannelsWriteOut[i][1] = effectsChannelsWriteOutBuffer + (2 * i + 1) * MAX_AUDIO_FRAMES_PER_BUFFER;
    }

    Logging::write(
      Info,
      "Audio::AudioCore::init",
      "Initialized AudioCore"
    );

    return OK;
  }

  static float factorLL(
    const float gain,
    const float gainL,
    const float gainR,
    const float mute,
    const float muteL,
    const float muteR,
    const float solo,
    const float soloL,
    const float soloR,
    const float pan,
    const float panL,
    const float panR,
    const float channelCount
    ) {

    // const float panVal = pan <= 0.0f ? 1.0f : 1.0f - pan;
    const float panLVal = (1.0f - panL) / 2.0f;
    const float soloVal = solo == 1.0f
                            ? 1.0f
                            : soloL;
    return soloVal * (1.0f - mute) * (1.0f - muteL) * panLVal * gain * gainL / channelCount;
  }

  static float factorLR(
    const float gain,
    const float gainL,
    const float gainR,
    const float mute,
    const float muteL,
    const float muteR,
    const float solo,
    const float soloL,
    const float soloR,
    const float pan,
    const float panL,
    const float panR,
    const float channelCount
    ) {

    // const float panVal = panL >= 0.0f ? pan : 0.0;
    const float panLVal = (1.0f + panL) / 2.0f;
    const float soloVal = solo == 1.0f
                            ? 1.0f
                            : soloL;
    return soloVal * (1.0f - mute) * (1.0f - muteL) * panLVal * gain * gainL / channelCount;
  }

  static float factorRL(
    const float gain,
    const float gainL,
    const float gainR,
    const float mute,
    const float muteL,
    const float muteR,
    const float solo,
    const float soloL,
    const float soloR,
    const float pan,
    const float panL,
    const float panR,
    const float channelCount
    ) {

    // const float panVal = pan <= 0.0f ? -pan : 0.0f;
    const float panRVal = (1.0f - panR) / 2.0f;
    const float soloVal = solo == 1.0f
                            ? 1.0f
                            : soloR;
    return soloVal * (1.0f - mute) * (1.0f - muteR) * panRVal * gain * gainR / channelCount;
  }

  static float factorRR(
    const float gain,
    const float gainL,
    const float gainR,
    const float mute,
    const float muteL,
    const float muteR,
    const float solo,
    const float soloL,
    const float soloR,
    const float pan,
    const float panL,
    const float panR,
    const float channelCount
    ) {

    // const float panVal = pan >= 0.0f ? 1.0f : 1.0f + pan;
    const float panRVal = (1.0f + panR) / 2.0f;
    const float soloVal = solo == 1.0f
                            ? 1.0f
                            : soloR;
    return soloVal * (1.0f - mute) * (1.0f - muteR) * panRVal * gain * gainR / channelCount;
  }

  Result addCassetteFromFilePath(const char* filePath) {
    Logging::write(
      Info,
      "Audio::AudioCore::addCassette",
      "Adding cassette to deckIndex " + std::to_string(deckIndex) + " for filePath " + filePath
    );
    const int nextDeckIndex = (deckIndex + 1) % AUDIO_CORE_DECK_COUNT;
    deckIndex = nextDeckIndex;
    decks[deckIndex].setCassetteFromFilePath(filePath);

    Logging::write(
      Info,
      "Audio::AudioCore::addCassette",
      "Set input buffers to cassette input buffers"
    );

    return OK;
  }

  Result setChannelCount(float val) {
    effectsChannelCount = static_cast<int>(val);
    channelCount = val;
    return OK;
  }

  Result setDeckIndex(int val) {
    deckIndex = val;
    decks[deckIndex].active = true;
    return OK;
  }

  Result incrDeckIndex() {
    deckIndex = (deckIndex + 1) % AUDIO_CORE_DECK_COUNT;
    decks[deckIndex].active = true;
    return OK;
  }

  AudioDeck& currentDeck() {
    return decks[deckIndex];
  }

  Result updateCurrentDeckReadComplete(jack_nframes_t nframes) {
    if (decks[deckIndex].frameId >= decks[deckIndex].frames - nframes)
      decks[deckIndex].setReadComplete(true);
    return OK;
  }
};

} // Audio
} // Gj

#endif // GJAUDIOAUDIODATA_H