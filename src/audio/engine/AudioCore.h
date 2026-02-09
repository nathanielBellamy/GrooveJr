#ifndef GJAUDIOAUDIODATA_H
#define GJAUDIOAUDIODATA_H

#include <algorithm>
#include <atomic>

#include <fftw3.h>

#include <sndfile.hh>
#include "../../enums/PlayState.h"
#include "../mixer/ChannelProcessData.h"
#include "BufferIndeces.h"
#include "../Constants.h"
#include "AudioDeck.h"

#include <jack/ringbuffer.h>

#include "Cassette.h"
#include "../../state/Core.h"
#include "../Logging.h"
#include "../../enums/Result.h"

namespace Gj {
namespace Audio {
struct AudioCore {
  long threadId;
  State::Core* stateCore;
  sf_count_t crossfade = 0;
  AudioDeck decks[AUDIO_CORE_DECK_COUNT]{AudioDeck(0, stateCore), AudioDeck(1, stateCore), AudioDeck(2, stateCore)};
  DeckIndex deckIndex = 0;
  DeckIndex deckIndexNext = 0;
  sf_count_t frameAdvance;
  float fft_eq_time[AUDIO_CHANNEL_COUNT][FFT_EQ_TIME_SIZE]{};
  fftwf_complex fft_eq_freq[AUDIO_CHANNEL_COUNT][FFT_EQ_FREQ_SIZE]{};
  float fft_eq_write_out_buffer[AUDIO_CHANNEL_COUNT * FFT_EQ_FREQ_SIZE]{};
  fftwf_plan fft_eq_0_plan_r2c;
  fftwf_plan fft_eq_1_plan_r2c;
  jack_ringbuffer_t* fft_eq_ring_buffer{nullptr};
  float fft_pv_time[FFT_PV_TIME_SIZE]{};
  float fft_pv_ola_buffer[AUDIO_CHANNEL_COUNT][FFT_PV_OLA_BUFFER_SIZE]{};
  fftwf_complex fft_pv_freq[FFT_PV_FREQ_SIZE]{};
  fftwf_complex fft_pv_freq_shift[FFT_PV_FREQ_SIZE]{};
  float fft_pv_phase_sum[AUDIO_CHANNEL_COUNT][FFT_PV_FREQ_SIZE]{};
  float fft_pv_phase_prev[AUDIO_CHANNEL_COUNT][FFT_PV_FREQ_SIZE]{};
  float fft_pv_phase_prev_init[AUDIO_CHANNEL_COUNT][FFT_PV_FREQ_SIZE]{};
  fftwf_plan fft_pv_plan_r2c;
  fftwf_plan fft_pv_plan_c2r;
  float vu_buffer_in[VU_BUFFER_SIZE]{};
  float vu_buffer[VU_BUFFER_SIZE]{};
  jack_ringbuffer_t* vu_ring_buffer{nullptr};
  float processBuffersBuffer[AUDIO_FRAMES_PER_BUFFER_MAX * AUDIO_CHANNEL_COUNT]{};
  float* processBuffers[AUDIO_CHANNEL_COUNT]{nullptr, nullptr};
  float playbackBuffersPre[AUDIO_CHANNEL_COUNT][AUDIO_FRAMES_PER_BUFFER_MAX]{};
  float playbackBuffersBuffer[AUDIO_FRAMES_PER_BUFFER_MAX * AUDIO_CHANNEL_COUNT]{};
  float* playbackBuffers[AUDIO_CHANNEL_COUNT]{nullptr, nullptr};
  float fftFreqBuffersBuffer[FFT_FREQ_BUFFERS_BUFFER_SIZE]{};
  sf_count_t playbackSettingsToAudioThread[BfrIdx::PSTAT::SIZE]{};
  jack_ringbuffer_t* playbackSettingsToAudioThreadRB{nullptr};

  sf_count_t playbackSettingsFromAudioThread[BfrIdx::PSFAT::SIZE]{};
  jack_ringbuffer_t* playbackSettingsFromAudioThreadRB{nullptr};
  std::function<int(AudioCore*, sf_count_t, jack_nframes_t)> fillPlaybackBuffer;

  size_t mixerChannelCount;
  jack_ringbuffer_t* mixerChannelsProcessDataRB{};
  Mixer::ChannelProcessData mixerChannelsProcessData[MAX_MIXER_CHANNELS]{};
  float mixerChannelsSettings[MAX_MIXER_CHANNELS_SETTINGS_COUNT]{};
  jack_ringbuffer_t* mixerChannelsSettingsRB{nullptr};
  float mixerChannelsWriteOutBuffer[AUDIO_CHANNEL_COUNT * AUDIO_FRAMES_PER_BUFFER_MAX * MAX_MIXER_CHANNELS]{};
  float* mixerChannelsWriteOut[MAX_MIXER_CHANNELS][AUDIO_CHANNEL_COUNT]{};

  AudioCore(State::Core* stateCore)
  : stateCore(stateCore)
    , fft_eq_ring_buffer(jack_ringbuffer_create(FFT_EQ_RING_BUFFER_SIZE))
    , vu_ring_buffer(jack_ringbuffer_create(VU_RING_BUFFER_SIZE))
    , playbackSettingsToAudioThreadRB(jack_ringbuffer_create(BfrIdx::PSTAT::RB_SIZE))
    , playbackSettingsFromAudioThreadRB(jack_ringbuffer_create(BfrIdx::PSFAT::RB_SIZE))
    , mixerChannelsProcessDataRB(jack_ringbuffer_create(BfrIdx::MixerChannel::ProcessData::RB_SIZE))
    , mixerChannelsSettingsRB(jack_ringbuffer_create(BfrIdx::MixerChannel::Settings::RB_SIZE)) {
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

    jack_ringbuffer_free(fft_eq_ring_buffer);
    jack_ringbuffer_free(vu_ring_buffer);
    jack_ringbuffer_free(mixerChannelsSettingsRB);
    jack_ringbuffer_free(playbackSettingsToAudioThreadRB);
    jack_ringbuffer_free(playbackSettingsFromAudioThreadRB);

    Logging::write(
      Info,
      "Audio::AudioCore::~AudioCore",
      "Destroyed AudioCore"
    );
  }

  Result clearBuffers() {
    std::fill_n(mixerChannelsWriteOutBuffer, AUDIO_CHANNEL_COUNT * MAX_MIXER_CHANNELS * AUDIO_FRAMES_PER_BUFFER_MAX,
                0.0f);
    std::fill_n(playbackBuffersBuffer, AUDIO_CHANNEL_COUNT * AUDIO_FRAMES_PER_BUFFER_MAX, 0.0f);
    std::fill_n(processBuffersBuffer, AUDIO_CHANNEL_COUNT * AUDIO_FRAMES_PER_BUFFER_MAX, 0.0f);

    // fftwf_complex arrays
    std::memset(fft_eq_freq, 0.0f, sizeof(fft_eq_freq));
    std::memset(fft_pv_freq, 0.0f, sizeof(fft_pv_freq));
    std::memset(fft_pv_freq_shift, 0.0f, sizeof(fft_pv_freq_shift));

    // TODO: debug
    // for (auto ch = 0; ch < AUDIO_FRAMES_PER_BUFFER_MAX; ++ch) {
    //   std::fill_n(fft_eq_time[ch], FFT_EQ_TIME_SIZE, 0.0f);
    //   std::fill_n(fft_pv_phase_sum[ch], FFT_PV_FREQ_SIZE, 0.0f);
    //   std::fill_n(fft_pv_phase_prev[ch], FFT_PV_FREQ_SIZE, 0.0f);
    //   std::fill_n(fft_pv_phase_prev_init[ch], FFT_PV_FREQ_SIZE, 0.0f);
    //   std::fill_n(fft_pv_ola_buffer[ch], FFT_PV_OLA_BUFFER_SIZE, 0.0f);
    // }
    //
    // std::fill_n(fft_eq_write_out_buffer, AUDIO_CHANNEL_COUNT * FFT_EQ_FREQ_SIZE, 0.0f);
    // std::fill_n(vu_buffer_in, VU_RING_BUFFER_SIZE, 0.0f);
    // std::fill_n(vu_buffer, VU_RING_BUFFER_SIZE, 0.0f);
    //
    // std::fill_n(fft_pv_time, FFT_PV_TIME_SIZE, 0.0f);
    //
    // std::fill_n(fft_eq_write_out_buffer, AUDIO_CHANNEL_COUNT * FFT_EQ_FREQ_SIZE, 0.0f);
    // std::fill_n(fftFreqBuffersBuffer, AUDIO_CHANNEL_COUNT * AUDIO_FRAMES_PER_BUFFER_MAX, 0.0f);
    // std::fill_n(mixerChannelsSettings, MAX_MIXER_CHANNELS_SETTINGS_COUNT, 0.0f);
    //
    // std::fill_n(playbackSettingsFromAudioThread, PlaybackSettingsFromAudioThread_Count, 0);
    // std::fill_n(playbackSettingsToAudioThread, PlaybackSettingsToAudioThread_Count, 0);

    return OK;
  }

  Result init() {
    Logging::write(
      Info,
      "Audio::AudioCore::init",
      "Initializing AudioCore"
    );

    clearBuffers();
    setBufferRefs();

    fft_eq_0_plan_r2c = fftwf_plan_dft_r2c_1d(FFT_EQ_TIME_SIZE, fft_eq_time[0], fft_eq_freq[0], FFTW_ESTIMATE);
    fft_eq_1_plan_r2c = fftwf_plan_dft_r2c_1d(FFT_EQ_TIME_SIZE, fft_eq_time[1], fft_eq_freq[1], FFTW_ESTIMATE);

    fft_pv_plan_r2c = fftwf_plan_dft_r2c_1d(FFT_PV_TIME_SIZE, fft_pv_time, fft_pv_freq, FFTW_ESTIMATE);
    fft_pv_plan_c2r = fftwf_plan_dft_c2r_1d(FFT_PV_FREQ_SIZE, fft_pv_freq_shift, fft_pv_time, FFTW_ESTIMATE);

    Logging::write(
      Info,
      "Audio::AudioCore::init",
      "Initialized AudioCore"
    );

    return OK;
  }

  Result setBufferRefs() {
    processBuffers[0] = &processBuffersBuffer[0];
    processBuffers[1] = &processBuffersBuffer[AUDIO_FRAMES_PER_BUFFER_MAX];

    playbackBuffers[0] = &playbackBuffersBuffer[0];
    playbackBuffers[1] = &playbackBuffersBuffer[AUDIO_FRAMES_PER_BUFFER_MAX];

    for (ChannelIndex i = 0; i < MAX_MIXER_CHANNELS; i++) {
      mixerChannelsWriteOut[i][0] = &mixerChannelsWriteOutBuffer[2 * i * AUDIO_FRAMES_PER_BUFFER_MAX];
      mixerChannelsWriteOut[i][1] = &mixerChannelsWriteOutBuffer[(2 * i + 1) * AUDIO_FRAMES_PER_BUFFER_MAX];
    }

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

  Result addCassetteFromDecoratedAudioFile(const Db::DecoratedAudioFile& decoratedAudioFile) {
    Logging::write(
      Info,
      "Audio::AudioCore::addCassetteFromDecoratedAudioFile",
      "Adding cassette to deckIndex " + std::to_string(deckIndex) + " for filePath " + decoratedAudioFile.audioFile.
      filePath
    );
    const DeckIndex nextDeckIndex = (deckIndex + 1) % AUDIO_CORE_DECK_COUNT;
    deckIndex = nextDeckIndex;
    deckIndexNext = nextDeckIndex;
    if (decks[deckIndex].setCassetteFromDecoratedAudioFile(decoratedAudioFile) == ERROR) {
      Logging::write(
        Error,
        "Audio::AudioCore::addCassetteFromDecoratedAudioFile",
        "Unable to set Cassette from DecoratedAudioFile. Path: " + decoratedAudioFile.audioFile.filePath
      );
      return ERROR;
    };
    stateCore->setCurrentlyPlaying(decoratedAudioFile);

    Logging::write(
      Info,
      "Audio::AudioCore::addCassetteFromDecoratedAudioFile",
      "Added cassette to deckIndex " + std::to_string(deckIndex) + " for filePath " + decoratedAudioFile.audioFile.
      filePath
    );
    return OK;
  }

  Result addCassetteFromDecoratedAudioFileAtIdx(const Db::DecoratedAudioFile& decoratedAudioFile,
                                                const DeckIndex deckIndexToSet) {
    Logging::write(
      Info,
      "Audio::AudioCore::addCassetteFromDecoratedAudioFileAtIdx",
      "Adding cassette to deckIndex " + std::to_string(deckIndex) + " for filePath " + decoratedAudioFile.audioFile.
      filePath
    );
    if (decks[deckIndexToSet].setCassetteFromDecoratedAudioFile(decoratedAudioFile) == ERROR) {
      Logging::write(
        Error,
        "Audio::AudioCore::addCassetteFromDecoratedAudioFileAtIdx",
        "Unable to set Cassette from DecoratedAudioFile. Path: " + decoratedAudioFile.audioFile.filePath
      );
      return ERROR;
    };
    stateCore->setCurrentlyPlaying(decoratedAudioFile);

    Logging::write(
      Info,
      "Audio::AudioCore::addCassetteFromDecoratedAudioFileAtIdx",
      "Added cassette to deckIndex " + std::to_string(deckIndex) + " for filePath " + decoratedAudioFile.audioFile.
      filePath
    );
    return OK;
  }

  Result setChannelCount(const ChannelIndex val) {
    mixerChannelCount = val;
    return OK;
  }

  Result setDeckIndex(const DeckIndex val) {
    deckIndex = val;
    return OK;
  }

  bool shouldUpdateDeckIndex() {
    return deckIndex != deckIndexNext;
  }

  Result updateDeckIndexToNext() {
    Logging::write(
      Info,
      "Audio::AudioCore::updateDeckIndexToNext",
      "Updating deckIndex " + std::to_string(deckIndex) + " to next deckIndexNext " + std::to_string(deckIndexNext)
    );

    decks[deckIndex].playState = STOP;
    if (decks[deckIndexNext].decoratedAudioFile) {
      stateCore->setCurrentlyPlaying(decks[deckIndexNext].decoratedAudioFile.value());
      decks[deckIndexNext].playState = PLAY;
    } else {
      decks[deckIndexNext].playState = STOP;
    }
    deckIndex = deckIndexNext;
    return OK;
  }

  Result incrDeckIndex() {
    deckIndex = (deckIndex + 1) % AUDIO_CORE_DECK_COUNT;
    return OK;
  }

  AudioDeck& currentDeck() {
    return decks[deckIndex];
  }

  float getDeckGain(const DeckIndex di) const {
    const auto& deck = decks[di];
    const float frameIdF = static_cast<float>(deck.frameId);
    const float framesF = static_cast<float>(deck.frames);
    const float crossfadeF = static_cast<float>(stateCore->getCrossfade());
    // TODO: log, exp transition funcs
    if (deckIndex == di) {
      if (deck.frameId >= deck.frames - stateCore->getCrossfade())
        return (framesF - frameIdF) / crossfadeF;

      if (deck.frameId <= stateCore->getCrossfade())
        return frameIdF / crossfadeF;

      return 1.0f;
    }

    if (di == (deckIndex + 1) % AUDIO_CORE_DECK_COUNT) {
      if (deck.frameId <= stateCore->getCrossfade())
        return frameIdF / crossfadeF;

      return 0.0f;
    }

    if (di == (deckIndex - 1) % AUDIO_CORE_DECK_COUNT) {
      if (deck.frameId >= deck.frames - stateCore->getCrossfade())
        return (framesF - frameIdF) / crossfadeF;

      return 0.0f;
    }

    return 0.0f;
  }

  Result setPlayStateAllDecks(const PlayState playState) {
    for (DeckIndex i = 0; i < AUDIO_CORE_DECK_COUNT; i++)
      decks[i].playState = playState;

    return OK;
  }

  Result setFrameIdAllDecks(const sf_count_t frameId) const {
    for (DeckIndex i = 0; i < AUDIO_CORE_DECK_COUNT; i++)
      decks[i].frameId = frameId;

    return OK;
  }
};
} // Audio
} // Gj

#endif // GJAUDIOAUDIODATA_H
