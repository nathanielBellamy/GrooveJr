//-----------------------------------------------------------------------------
// Flags       : clang-format auto
// Project     : VST SDK
//
// Category    : AudioHost
// Filename    : public.sdk/samples/vst-hosting/audiohost/source/media/jack/jackclient.cpp
// Created by  : Steinberg 09.2016
// Description : Audio Host Example for VST 3 using Jack
//
//-----------------------------------------------------------------------------
// LICENSE
// (c) 2024, Steinberg Media Technologies GmbH, All Rights Reserved
//-----------------------------------------------------------------------------
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of the Steinberg Media Technologies nor the names of its
//     contributors may be used to endorse or promote products derived from this
//     software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#include "JackClient.h"

#include <caf/log/core.hpp>
#include <caf/log/level.hpp>

#include "../mixer/Core.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {
static constexpr int kJackSuccess = 0;

static constexpr int kJackError = 1;
} // Vst
} // Steinberg

namespace Gj {
namespace Audio {
jack_port_t* outPortL;

jack_port_t* outPortR;

using namespace Steinberg;

JackClient::JackClient(Mixer::Core* mixer)
: mixer(mixer) {
}

//------------------------------------------------------------------------
JackClient::~JackClient() {
  Logging::write(
    Info,
    "Audio::JackClient::~JackClient()",
    "Destroyed JackClient"
  );
}

Result JackClient::initialize(const JackName name) {
  Logging::write(
    Info,
    "Audio::JackClient::initialize",
    "Initializing JackClient"
  );
  try {
    jackClient = registerClient(name);
  } catch (...) {
    Logging::write(
      Error,
      "Audio::JackClient::initialize",
      "An error occurred while attempting to Register JackClient"
    );
    return ERROR;
  }

  if (!jackClient) {
    Logging::write(
      Error,
      "Audio::JackClient::initialize",
      "Unable to Register JackClient"
    );
    return ERROR;
  }

  return OK;
}

Result JackClient::activate(AudioCore* audioCore) const {
  Logging::write(
    Info,
    "Audio::JackClient::activate",
    "Setting up JackClient"
  );

  if (setCallbacks(audioCore) != OK) {
    Logging::write(
      Error,
      "Audio::JackClient::activate",
      "Unable to set callbacks"
    );
    return ERROR;
  }

  if (setPorts() != OK) {
    Logging::write(
      Error,
      "Audio::JackClient::activate",
      "Unable to set ports"
    );
    return ERROR;
  }

  if (activateAndConnectPorts() != OK) {
    Logging::write(
      Error,
      "Audio::JackClient::activate",
      "Unable to activate jackClient and/or connect ports"
    );
    return ERROR;
  }

  return OK;
}

Result JackClient::deactivate() const {
  if (const auto jackDeactivateStatus = jack_deactivate(jackClient); jackDeactivateStatus != 0) {
    Logging::write(
      Error,
      "Audio::JackClient::deactivate",
      "Unable to deactivate JackClient. Status: " + std::to_string(jackDeactivateStatus)
    );
    return ERROR;
  } else {
    return OK;
  }
}

Result JackClient::setCallbacks(AudioCore* audioCore) const {
  Logging::write(
    Info,
    "Audio::JackClient::setCallbacks",
    "Setting Jack callbacks"
  );

  const int setProcessStatus = jack_set_process_callback(
    jackClient,
    &JackClient::processCallback,
    audioCore
  );
  if (setProcessStatus == kJackSuccess) {
    Logging::write(
      Info,
      "Audio::JackClient::setup",
      "Set Jack process callback"
    );
  } else {
    Logging::write(
      Error,
      "Audio::JackClient::setup",
      "Unable to set process callback - status: " + std::to_string(setProcessStatus)
    );
    return ERROR;
  }

  bool warning = false;
  if (const auto success = jack_set_buffer_size_callback(jackClient, &JackClient::setBufferSizeCallback, mixer);
    success != kJackSuccess) {
    Logging::write(
      Warning,
      "Audio::JackClient::setCallbacks",
      "Unable to set buffer size callback"
    );
    warning = true;
  }

  if (const auto success = jack_set_sample_rate_callback(jackClient, &JackClient::setSampleRateCallback, mixer);
    success != kJackSuccess) {
    Logging::write(
      Warning,
      "Audio::JackClient::setCallbacks",
      "Unable to set sample rate callback"
    );
    warning = true;
  }

  if (const auto success = jack_set_xrun_callback(jackClient, &JackClient::xRunCallback, mixer);
    success != kJackSuccess) {
    Logging::write(
      Warning,
      "Audio::JackClient::setCallbacks",
      "Unable to set xrun callback"
    );
  }

  return warning ? WARNING : OK;
}

Result JackClient::logXRun() {
  Logging::write(
    Error,
    "Audio::JackClient::logXRun",
    "Jack xRun occurred"
  );
  return OK;
}

Result JackClient::setPorts() const {
  Logging::write(
    Info,
    "Audio::JackClient::setPorts",
    "Setting Jack ports"
  );

  if (outPortL == nullptr) {
    outPortL = jack_port_register(
      jackClient,
      "out_port_L",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput,
      0
    );
  }
  if (outPortL == nullptr) {
    Logging::write(
      Error,
      "Audio::JackClient::setup",
      "Unable to create Jack outPortL"
    );
    return ERROR;
  }

  if (outPortR == nullptr) {
    outPortR = jack_port_register(
      jackClient,
      "out_port_R",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput,
      0
    );
  }
  if (outPortR == nullptr) {
    Logging::write(
      Error,
      "Audio::JackClient::setup",
      "Unable to create Jack outPortR"
    );
    return ERROR;
  }

  return OK;
}

Result JackClient::activateAndConnectPorts() const {
  Logging::write(
    Info,
    "Audio::JackClient::activateAndConnectPorts",
    "Activating Jack client and connecting ports"
  );

  if (const int jackActivateStatus = jack_activate(jackClient); jackActivateStatus == 0) {
    Logging::write(
      Info,
      "Audio::JackClient::setup",
      "Jack activated successfully"
    );
  } else {
    Logging::write(
      Error,
      "Audio::JackClient::setup",
      "Unable to activate jack - status: " + std::to_string(jackActivateStatus)
    );
    return ERROR;
  }

  // connect ports after activating client!
  if (
    const int connectStatusL = jack_connect(jackClient, jack_port_name(outPortL), "system:playback_1");
    connectStatusL != 0
  ) {
    Logging::write(
      Error,
      "Audio::JackClient::setup",
      "Unable to connect out_port_L - status: " + std::to_string(connectStatusL)
    );
    return ERROR;
  }

  if (
    const int connectStatusR = jack_connect(jackClient, jack_port_name(outPortR), "system:playback_2");
    connectStatusR != 0
  ) {
    Logging::write(
      Error,
      "Audio::JackClient::setup",
      "Unable to connect out_port_R - status: " + std::to_string(connectStatusR)
    );
    return ERROR;
  }

  // TESTING SHOW PORTS
  // const char** ports = jack_get_ports(jackClient, nullptr, nullptr, 0);
  // if (ports == nullptr) {
  //   printf("No ports found.\n");
  // } else {
  //   for (int i = 0; ports[i] != nullptr; i++) {
  //     printf("Port: %s\n", ports[i]);
  //   }
  // }
  // TESTING SHOW PORTS

  return OK;
}

float JackClient::princArg(const float phaseIn) {
  return std::atan2(std::sinf(phaseIn), std::cosf(phaseIn));
}

// plabackSpeed in [-2.0, 2.0]
int JackClient::fillPlaybackBuffer(AudioCore* audioCore, const sf_count_t playbackSpeed, const jack_nframes_t nframes) {
  if constexpr (false) {
    // TODO: fix phase-tracking in phase vocoder
    const int hopAnalysis = FFT_PV_HOP_ANALYSIS;
    const float hopAnalysisF = FFT_PV_HOP_ANALYSISF;
    const int fftSize = FFT_PV_TIME_SIZE;
    const float fftSizeF = FFT_PV_TIME_SIZEF;

    const float pitchShiftSemitones = 0.0f;
    const float pitchRatio = powf(2.0f, pitchShiftSemitones / 12.0f);
    const float stretch = powf(pitchRatio, -1.0f);
    const float hopSynthesisF = hopAnalysisF * stretch;
    const int hopSynthesis = static_cast<int>(hopSynthesisF);
    const int numBins = fftSize / 2 + 1;

    // playbackPitch
    for (int chan = 0; chan < 2; chan++) {
      std::fill_n(audioCore->fft_pv_ola_buffer[chan], 2 * FFT_PV_TIME_SIZE, 0.0f);

      for (int hop = 0; hop < FFT_PV_HOP_COUNT; hop++) {
        // TODO: update to decks
        const float* processHead = nullptr; // audioCore->inputBuffers[chan] + audioCore->frameId + hop * hopAnalysis;

        for (int i = 0; i < fftSize; i++) {
          const float hannFactor = 0.5f * (1.0f - std::cosf(TWO_PI * static_cast<float>(i) / fftSizeF));
          audioCore->fft_pv_time[i] = processHead[i] * hannFactor;
        }

        fftwf_execute_dft_r2c(
          audioCore->fft_pv_plan_r2c,
          audioCore->fft_pv_time,
          audioCore->fft_pv_freq
        );

        for (int k = 0; k < numBins; k++) {
          const float kF = static_cast<float>(k);
          const float re = audioCore->fft_pv_freq[k][0];
          const float im = audioCore->fft_pv_freq[k][1];

          const float mag = std::hypot(re, im);
          const float phase = std::atan2(im, re);

          const float freqBin = TWO_PI * kF / fftSizeF;

          const float expectedPhaseAdvance = freqBin * FFT_PV_HOP_ANALYSISF;

          const float phaseDiff = princArg(phase - audioCore->fft_pv_phase_prev[chan][k]);
          const float phaseEp = princArg(phaseDiff - expectedPhaseAdvance);
          const float phaseTrue = princArg(expectedPhaseAdvance + phaseEp);

          audioCore->fft_pv_phase_prev[chan][k] = phase;

          audioCore->fft_pv_phase_sum[chan][k] = audioCore->fft_pv_phase_sum[chan][k] + phaseTrue * stretch;

          audioCore->fft_pv_freq_shift[k][0] = mag * cosf(audioCore->fft_pv_phase_sum[chan][k]);
          audioCore->fft_pv_freq_shift[k][1] = mag * sinf(audioCore->fft_pv_phase_sum[chan][k]);
        }

        fftwf_execute_dft_c2r(
          audioCore->fft_pv_plan_c2r,
          audioCore->fft_pv_freq_shift,
          audioCore->fft_pv_time
        );

        const int indexC = hop * hopSynthesis;
        for (int i = 0; i < fftSize; i++) {
          if (const int index = i + indexC; index < FFT_PV_OLA_BUFFER_SIZE) {
            const float hannFactor = 0.5f * (1.0f - std::cosf(TWO_PI * static_cast<float>(i) / fftSizeF));
            const float val = hannFactor * audioCore->fft_pv_time[i] / fftSizeF;
            audioCore->fft_pv_ola_buffer[chan][index] += val;
          }
        }
      }

      for (int i = 0; i < nframes; i++)
        audioCore->playbackBuffersPre[chan][i] = audioCore->fft_pv_ola_buffer[chan][i];
    }
  }

  for (auto& deck: audioCore->decks) {
    if (!deck.isPlaying())
      continue;

    deck.gain = audioCore->getDeckGain(deck.deckIndex);

    // playbackSpeed
    // todo: playbackBuffersPre
    const float* processHeadL = deck.inputBuffers[BfrIdx::AudCh::LEFT] + deck.frameId;
    const float* processHeadR = deck.inputBuffers[BfrIdx::AudCh::RIGHT] + deck.frameId;

    const float playbackSpeedF = static_cast<float>(playbackSpeed) / 100.0f;
    float playbackPos = 0.0f;
    float playbackPosTrunc = 0.0f;
    int idx = 0;
    for (jack_nframes_t i = 0; i < nframes; i++) {
      playbackPosTrunc = std::trunc(playbackPos);
      idx = static_cast<int>(playbackPosTrunc);
      const float frac = playbackPos - playbackPosTrunc;
      const auto valL = ((1.0f - frac) * processHeadL[idx] + frac * processHeadL[idx + 1]) * deck.gain;
      const auto valR = ((1.0f - frac) * processHeadR[idx] + frac * processHeadR[idx + 1]) * deck.gain;
      audioCore->playbackBuffers[BfrIdx::AudCh::LEFT][i] += std::isnan(valL) ? 0.0f : valL;
      audioCore->playbackBuffers[BfrIdx::AudCh::RIGHT][i] += std::isnan(valR) ? 0.0f : valR;
      playbackPos += playbackSpeedF;
    }

    audioCore->frameAdvance = idx;
    deck.frameId += idx;
  }

  const auto& currentDeck = audioCore->currentDeck();
  const DeckIndex nextDeckIndex = (audioCore->deckIndex + 1) % AUDIO_CORE_DECK_COUNT;
  const DeckIndex prevDeckIndex = (audioCore->deckIndex + AUDIO_CORE_DECK_COUNT - 1) % AUDIO_CORE_DECK_COUNT;
  if (playbackSpeed > 0) {
    if (currentDeck.frameId >= currentDeck.frames - audioCore->crossfade)
      audioCore->decks[nextDeckIndex].playState = PLAY;
    if (currentDeck.frameId >= currentDeck.frames - 4 * nframes)
      audioCore->deckIndexNext = nextDeckIndex;
  } else {
    if (currentDeck.frameId < audioCore->crossfade) {
      audioCore->decks[prevDeckIndex].frameId = audioCore->decks[prevDeckIndex].frames - 2;
      audioCore->decks[prevDeckIndex].playState = PLAY;
    }
    if (currentDeck.frameId < 4 * nframes)
      audioCore->deckIndexNext = prevDeckIndex;
  }

  return 0;
}

// jack process callback
// do not allocate/free memory within this method
// as it may be called at system-interrupt level
int JackClient::processCallback(jack_nframes_t nframes, void* arg) {
  // get port buffers
  auto* outL = static_cast<jack_default_audio_sample_t*>(
    jack_port_get_buffer(outPortL, nframes)
  );
  auto* outR = static_cast<jack_default_audio_sample_t*>(
    jack_port_get_buffer(outPortR, nframes)
  );

  const auto audioCore = static_cast<AudioCore*>(arg);
  audioCore->clearBuffers();

  if (jack_ringbuffer_read_space(audioCore->playbackSettingsToAudioThreadRB) > BfrIdx::PSTAT::RB_SIZE -
      2) {
    jack_ringbuffer_read(
      audioCore->playbackSettingsToAudioThreadRB,
      reinterpret_cast<char*>(audioCore->playbackSettingsToAudioThread),
      BfrIdx::PSTAT::RB_SIZE
    );
  }

  if (jack_ringbuffer_read_space(audioCore->mixerChannelsProcessDataRB) > BfrIdx::MixerChannel::ProcessData::RB_SIZE -
      2) {
    jack_ringbuffer_read(
      audioCore->mixerChannelsProcessDataRB,
      reinterpret_cast<char*>(audioCore->mixerChannelsProcessData),
      BfrIdx::MixerChannel::ProcessData::RB_SIZE
    );

    if (audioCore->playbackSettingsToAudioThread[BfrIdx::PSTAT::PROCESS_DATA_CHANGE_FLAG] ==
        ProcessDataChangeFlag::ACKNOWLEDGE)
      audioCore->playbackSettingsFromAudioThread[BfrIdx::PSFAT::PROCESS_DATA_CHANGE_FLAG] =
          ProcessDataChangeFlag::ROGER;
  } else {
    audioCore->playbackSettingsFromAudioThread[BfrIdx::PSFAT::PROCESS_DATA_CHANGE_FLAG] = ProcessDataChangeFlag::BASE;
  }

  if (audioCore->playbackSettingsToAudioThread[BfrIdx::PSTAT::USER_SETTING_FRAME_ID_FLAG] == 1) // user set frame Id
    audioCore->currentDeck().frameId = audioCore->playbackSettingsToAudioThread[BfrIdx::PSTAT::NEW_FRAME_ID];

  audioCore->playbackSettingsFromAudioThread[BfrIdx::PSFAT::DEBUG_VALUE] = 0;
  audioCore->playbackSettingsFromAudioThread[BfrIdx::PSFAT::CURRENT_FRAME_ID] = audioCore->currentDeck().frameId;

  // write to playbackSettingsFromAudioThread ring buffer
  if (jack_ringbuffer_write_space(audioCore->playbackSettingsFromAudioThreadRB) >
      BfrIdx::PSFAT::RB_SIZE - 2) {
    jack_ringbuffer_write(
      audioCore->playbackSettingsFromAudioThreadRB,
      reinterpret_cast<char*>(audioCore->playbackSettingsFromAudioThread),
      BfrIdx::PSFAT::RB_SIZE
    );
  }

  audioCore->fillPlaybackBuffer(
    audioCore,
    audioCore->playbackSettingsToAudioThread[BfrIdx::PSTAT::PLAYBACK_SPEED],
    nframes
  );

  // process channels
  // main channel is chhannelIdx 0
  const int32_t nframes32t = static_cast<int32_t>(nframes);
  for (ChannelIndex channelIdx = 1; channelIdx < MAX_MIXER_CHANNELS; ++channelIdx) {
    if (auto [processFuncs, buffers, pluginCount] = audioCore->mixerChannelsProcessData[channelIdx];
      pluginCount != 0) {
      for (PluginIndex pluginIdx = 0; pluginIdx < MAX_PLUGINS_PER_CHANNEL; ++pluginIdx) {
        buffers[pluginIdx].numSamples = nframes32t;
        processFuncs[pluginIdx](
          buffers[pluginIdx],
          nframes
        );
      }
    }
  }

  // read channel settings from ringbuffer
  if (jack_ringbuffer_t* ringBuffer = audioCore->mixerChannelsSettingsRB;
    jack_ringbuffer_read_space(ringBuffer) >= BfrIdx::MixerChannel::Settings::RB_SIZE - 2) {
    jack_ringbuffer_read(
      ringBuffer,
      reinterpret_cast<char*>(audioCore->mixerChannelsSettings),
      BfrIdx::MixerChannel::Settings::RB_SIZE
    );
  }

  float rmsL[MAX_MIXER_CHANNELS] = {0.0f};
  float rmsR[MAX_MIXER_CHANNELS] = {0.0f};
  // sum down
  for (jack_nframes_t i = 0; i < nframes; i++) {
    float accumL = 0.0f;
    float accumR = 0.0f;
    for (ChannelIndex channelIndex = 1; channelIndex < audioCore->mixerChannelCount; channelIndex++) {
      const float factorLL = audioCore->mixerChannelsSettings[BfrIdx::MixerChannel::Settings::factorLL(channelIndex)];
      const float factorLR = audioCore->mixerChannelsSettings[BfrIdx::MixerChannel::Settings::factorLR(channelIndex)];
      const float factorRL = audioCore->mixerChannelsSettings[BfrIdx::MixerChannel::Settings::factorRL(channelIndex)];
      const float factorRR = audioCore->mixerChannelsSettings[BfrIdx::MixerChannel::Settings::factorRR(channelIndex)];

      float valL = 0.0f;
      float valR = 0.0f;
      if (audioCore->mixerChannelsProcessData[channelIndex].pluginCount == 0) {
        valL = factorLL * audioCore->playbackBuffers[BfrIdx::AudCh::LEFT][i]
               + factorRL * audioCore->playbackBuffers[BfrIdx::AudCh::RIGHT][i];
        valR = factorLR * audioCore->playbackBuffers[BfrIdx::AudCh::LEFT][i]
               + factorRR * audioCore->playbackBuffers[BfrIdx::AudCh::RIGHT][i];
      } else {
        const auto writeOutValL =
            std::isnan(audioCore->mixerChannelsWriteOut[channelIndex][BfrIdx::AudCh::LEFT][i])
              ? 0.0f
              : audioCore->mixerChannelsWriteOut[channelIndex][BfrIdx::AudCh::LEFT][i];
        const auto writeOutValR =
            std::isnan(audioCore->mixerChannelsWriteOut[channelIndex][BfrIdx::AudCh::RIGHT][i])
              ? 0.0f
              : audioCore->mixerChannelsWriteOut[channelIndex][BfrIdx::AudCh::RIGHT][i];
        valL = factorLL * writeOutValL + factorRL * writeOutValR;
        valR = factorLR * writeOutValL + factorRR * writeOutValR;
      }

      accumL += valL;
      accumR += valR;

      rmsL[channelIndex] += valL * valL;
      rmsR[channelIndex] += valR * valR;
    }
    audioCore->processBuffers[BfrIdx::AudCh::LEFT][i] = accumL;
    audioCore->processBuffers[BfrIdx::AudCh::RIGHT][i] = accumR;
  }

  const float nframesF = static_cast<float>(nframes);
  for (ChannelIndex chIdx = 1; chIdx < audioCore->mixerChannelCount; chIdx++) {
    audioCore->vu_buffer_in[BfrIdx::VU::left(chIdx)] = std::sqrt(rmsL[chIdx] / nframesF);
    audioCore->vu_buffer_in[BfrIdx::VU::right(chIdx)] = std::sqrt(rmsR[chIdx] / nframesF);
  }

  // process summed down mix through main plugins
  auto [processFuncs, buffers, pluginCount] = audioCore->mixerChannelsProcessData[0];
  for (PluginIndex pluginIdx = 0; pluginIdx < MAX_PLUGINS_PER_CHANNEL; pluginIdx++) {
    buffers[pluginIdx].numSamples = static_cast<int32_t>(nframes);

    processFuncs[pluginIdx](
      buffers[pluginIdx],
      nframes
    );
  }

  // write out processed main channel to audio out
  const float factorLL = audioCore->mixerChannelsSettings[BfrIdx::MixerChannel::Settings::factorLL(0)];
  const float factorLR = audioCore->mixerChannelsSettings[BfrIdx::MixerChannel::Settings::factorLR(0)];
  const float factorRL = audioCore->mixerChannelsSettings[BfrIdx::MixerChannel::Settings::factorRL(0)];
  const float factorRR = audioCore->mixerChannelsSettings[BfrIdx::MixerChannel::Settings::factorRR(0)];

  for (size_t chan = 0; chan < AUDIO_CHANNEL_COUNT; chan++) {
    std::copy(
      std::begin(audioCore->fft_eq_time[chan]) + nframes,
      std::end(audioCore->fft_eq_time[chan]),
      std::begin(audioCore->fft_eq_time[chan])
    );
  }

  for (jack_nframes_t i = 0; i < nframes; i++) {
    const float pbValL =
        std::isnan(audioCore->processBuffers[BfrIdx::AudCh::LEFT][i])
          ? 0.0f
          : audioCore->processBuffers[BfrIdx::AudCh::LEFT][i];

    const float pbValR =
        std::isnan(audioCore->processBuffers[BfrIdx::AudCh::RIGHT][i])
          ? 0.0f
          : audioCore->processBuffers[BfrIdx::AudCh::RIGHT][i];


    const float valL = factorLL * pbValL + factorRL * pbValR;
    const float valR = factorLR * pbValL + factorRR * pbValR;

    rmsL[0] += valL * valL;
    rmsR[0] += valR * valR;

    audioCore->fft_eq_time[BfrIdx::AudCh::LEFT][FFT_EQ_TIME_SIZE - nframes + i] = valL;
    audioCore->fft_eq_time[BfrIdx::AudCh::RIGHT][FFT_EQ_TIME_SIZE - nframes + i] = valR;

    outL[i] = valL;
    outR[i] = valR;
  }

  audioCore->vu_buffer_in[BfrIdx::VU::left(0)] = std::sqrt(rmsL[0] / nframesF);
  audioCore->vu_buffer_in[BfrIdx::VU::right(0)] = std::sqrt(rmsR[0] / nframesF);

  if (jack_ringbuffer_write_space(audioCore->vu_ring_buffer) > VU_RING_BUFFER_SIZE - 2) {
    jack_ringbuffer_write(
      audioCore->vu_ring_buffer,
      reinterpret_cast<char*>(audioCore->vu_buffer_in),
      VU_RING_BUFFER_SIZE
    );
  }

  fftwf_execute_dft_r2c(
    audioCore->fft_eq_0_plan_r2c,
    audioCore->fft_eq_time[BfrIdx::AudCh::LEFT],
    audioCore->fft_eq_freq[BfrIdx::AudCh::LEFT]
  );

  fftwf_execute_dft_r2c(
    audioCore->fft_eq_1_plan_r2c,
    audioCore->fft_eq_time[BfrIdx::AudCh::RIGHT],
    audioCore->fft_eq_freq[BfrIdx::AudCh::RIGHT]
  );

  // compute + write magnitudes to ring buffer
  for (int i = 0; i < FFT_EQ_FREQ_SIZE; i++) {
    audioCore->fft_eq_write_out_buffer[BfrIdx::FFT::left(i)] = std::hypot(
      audioCore->fft_eq_freq[BfrIdx::AudCh::LEFT][i][0],
      audioCore->fft_eq_freq[BfrIdx::AudCh::LEFT][i][1]);
    audioCore->fft_eq_write_out_buffer[BfrIdx::FFT::right(i)] = std::hypot(
      audioCore->fft_eq_freq[BfrIdx::AudCh::RIGHT][i][0],
      audioCore->fft_eq_freq[BfrIdx::AudCh::RIGHT][i][1]);
  }

  if (jack_ringbuffer_write_space(audioCore->fft_eq_ring_buffer) > FFT_EQ_RING_BUFFER_SIZE - 2) {
    jack_ringbuffer_write(
      audioCore->fft_eq_ring_buffer,
      reinterpret_cast<char*>(audioCore->fft_eq_write_out_buffer),
      FFT_EQ_RING_BUFFER_SIZE
    );
  }


  return kJackSuccess;
}

int JackClient::setSampleRateCallback(jack_nframes_t nframes, void* arg) {
  if (auto* mixer = static_cast<Mixer::Core*>(arg); mixer->setSampleRate(nframes) != OK)
    return kJackError;

  return kJackSuccess;
}

int JackClient::setBufferSizeCallback(jack_nframes_t nframes, void* arg) {
  if (auto* mixer = static_cast<Mixer::Core*>(arg); mixer->setAudioFramesPerBuffer(nframes) != OK)
    return kJackError;

  return kJackSuccess;
}

int JackClient::xRunCallback(void* arg) {
  logXRun();
  return kJackSuccess;
}

bool JackClient::registerMidiPorts(IMidiClient* processor) {
  const auto ioSetup = processor->getMidiIOSetup();
  for (const auto& input: ioSetup.inputs)
    addMidiInputPort(input);

  return true;
}

bool JackClient::addMidiInputPort(JackClient::JackName name) {
  auto port =
      jack_port_register(jackClient, name.data(), JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
  if (!port)
    return false;

  midiInputPorts.push_back(port);
  return true;
}

int JackClient::processMidi(jack_nframes_t nframes) {
  // TODO: pass to plugins thru mixer
  static const uint8_t kChannelMask = 0x0F;
  static const uint8_t kStatusMask = 0xF0;
  static const uint32_t kDataMask = 0x7F;

  for (int32_t portIndex = 0, count = static_cast<int32_t>(midiInputPorts.size());
       portIndex < count; ++portIndex) {
    auto midiInputPort = midiInputPorts.at(portIndex);
    auto* portBuffer = jack_port_get_buffer(midiInputPort, nframes);
    if (!portBuffer)
      continue;

    jack_midi_event_t in_event;
    auto event_count = jack_midi_get_event_count(portBuffer);
    for (uint32_t i = 0; i < event_count; i++) {
      jack_midi_event_get(&in_event, portBuffer, i);
      if (in_event.size == 0)
        continue;

      auto midiData = in_event.buffer;
      IMidiClient::MidiData channel = midiData[0] & kChannelMask;
      IMidiClient::MidiData status = midiData[0] & kStatusMask;
      IMidiClient::MidiData data0 = midiData[1];
      IMidiClient::MidiData data1 = midiData[2];
      midiClient->onEvent({status, channel, data0, data1, in_event.time}, portIndex);
    }
  }

  return kJackSuccess;
}

jack_client_t* JackClient::registerClient(const JackName name) {
  Logging::write(
    Info,
    "Audio::JackClient::registerClient",
    "Registering JackClient: " + name
  );
  constexpr jack_options_t options = JackServerName;
  jack_status_t status;

  try {
    // TODO:
    // - if the jack server is in a bad state this call may hang
    // - detect hang and auto-restart jack server
    jackClient = jack_client_open(name.data(), options, &status, nullptr);
  } catch (...) {
    Logging::write(
      Error,
      "Audio::JackClient::registerClient",
      "An error occurred while attempting to open jack client - jack_client_open: " + name
    );
  }
  if (!jackClient) {
    Logging::write(
      Error,
      "Audio::JackClient::registerClient",
      "Unable to open JackClient: " + name + " - status: " + std::to_string(status)
    );
  } else {
    Logging::write(
      Info,
      "Audio::JackClient::registerClient",
      "Opened JackClient: " + name
    );
  }

  return jackClient;

  /* Use the status to check for errors:
      if (status & JackServerFailed)
      {
          fprintf (stderr, "Unable to connect to JACK server\n");
      }*/
}

bool JackClient::registerAudioClient(IAudioClient* client) {
  return registerClient("GrooveJr");
}

bool JackClient::registerMidiClient(IMidiClient* client) {
  if (midiClient)
    return false;

  midiClient = client;

  //! Register the midi ports.
  if (!registerMidiPorts(midiClient))
    return false;

  //! Afterwards auto-connect them.
  if (!autoConnectMidiPorts(jackClient))
    return false;

  return true;
}

//------------------------------------------------------------------------
bool JackClient::autoConnectMidiPorts(jack_client_t* client) {
  int portIndex = 1;

  //! Connect MIDI Inputs
  auto ports = jack_get_ports(client, nullptr, "midi", JackPortIsPhysical | JackPortIsOutput);
  if (!ports)
    return false;

  for (auto& port: midiInputPorts) {
    if (!ports[portIndex])
      break;

    auto inputPortName = ports[portIndex++];
    auto res = jack_connect(client, inputPortName, jack_port_name(port));
    if (res != 0)
      continue;
  }

  jack_free(ports);
  return true;
}
} // Audio
} // Gj
