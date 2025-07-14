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

#include "Mixer.h"

//------------------------------------------------------------------------
namespace Steinberg {
  namespace Vst {
    static constexpr int kJackSuccess = 0;
    static constexpr int kJackError = 1;
  } // Vst
} // Steinberg

namespace Gj {
namespace Audio {

jack_port_t *outPortL;
jack_port_t *outPortR;

using namespace Steinberg;

JackClient::JackClient(Mixer* mixer)
  : mixer(mixer)
  {}

//------------------------------------------------------------------------
JackClient::~JackClient() {
  Logging::write(
    Info,
    "Audio::JackClient::~JackClient()",
    "Destroying JackClient"
  );
  //! We do not need to "unregister" ports. It is done automatically with "jack_client_close"
  // jack_deactivate (jackClient); // Stops calls of process
  // jack_client_close (jackClient); // Remove client from process graph and remove all ports

  // Logging::write(
  // 	Info,
  // 	"Audio::JackClient::~JackClient()",
  // 	"jackClient closed"
  // );

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

Result JackClient::activate(AudioData* audioData) const {
  Logging::write(
    Info,
    "Audio::JackClient::setup",
    "Setting up JackClient"
  );

  if (setCallbacks(audioData) != OK) {
    Logging::write(
      Error,
      "Audio::JackClient::setup",
      "Unable to set callbacks"
    );
    return ERROR;
  }

  if (setPorts() != OK) {
    Logging::write(
      Error,
      "Audio::JackClient::setup",
      "Unable to set ports"
    );
    return ERROR;
  }

  if (activateAndConnectPorts() != OK) {
    Logging::write(
      Error,
      "Audio::JackClient::setup",
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

Result JackClient::setCallbacks(AudioData* audioData) const {
  Logging::write(
    Info,
    "Audio::JackClient::setCallbacks",
    "Setting Jack callbacks"
  );

  const int setProcessStatus = jack_set_process_callback(
    jackClient,
    &JackClient::processCallback,
    audioData
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
  if (const auto success = jack_set_buffer_size_callback(jackClient, &JackClient::setBufferSizeCallback, mixer); success != kJackSuccess) {
    Logging::write(
      Warning,
      "Audio::JackClient::setCallbacks",
      "Unable to set buffer size callback"
    );
    warning = true;
  }

  if (const auto success = jack_set_sample_rate_callback(jackClient, &JackClient::setSampleRateCallback, mixer); success != kJackSuccess) {
    Logging::write(
      Warning,
      "Audio::JackClient::setCallbacks",
      "Unable to set sample rate callback"
    );
    warning = true;
  }

  if (const auto success = jack_set_xrun_callback(jackClient, &JackClient::xRunCallback, mixer); success != kJackSuccess) {
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

// plabackSpeed in [-2.0, 2.0]
int JackClient::fillPlaybackBuffer(AudioData* audioData, const sf_count_t playbackSpeed, const jack_nframes_t nframes) {
  const float nframesF = static_cast<float>(nframes);

  // playbackPitch
  for (int chan = 0; chan < 2; chan++) {
    const float* processHead = audioData->inputBuffers[chan] + audioData->frameId;

    for (int i = 0; i < nframes; i++)
      audioData->fft_time[i] = processHead[i];

    int planIndex = 0;
    switch (nframes) {
      case 128:
        planIndex = 0;
        break;
      case 256:
        planIndex = 1;
        break;
      case 512:
        planIndex = 2;
        break;
      case MAX_AUDIO_FRAMES_PER_BUFFER:
        planIndex = 3;
    }

    fftwf_execute_dft_r2c(
      audioData->fft_plan_r2c[planIndex],
      audioData->fft_time,
      audioData->fft_freq
    );

    const int bins = nframes / 2 + 1;

    for (int k_dst = 0; k_dst < bins; k_dst++) {
      const float frac = static_cast<float>(k_dst) / 0.79f;

      if (const int k_src = static_cast<int>(frac); k_src < bins) {
        audioData->fft_freq_shift[k_dst][0] = audioData->fft_freq[k_src][0];
        audioData->fft_freq_shift[k_dst][1] = audioData->fft_freq[k_src][1];
      } else {
        audioData->fft_freq_shift[k_dst][0] = 0.0f;
        audioData->fft_freq_shift[k_dst][1] = 0.0f;
      }
    }

    fftwf_execute_dft_c2r(
      audioData->fft_plan_c2r[planIndex],
      audioData->fft_freq_shift,
      audioData->fft_time
    );

    for (int i = 0; i < nframes; i++)
      audioData->playbackBuffers[chan][i] = audioData->fft_time[i] / nframesF;
  }
  audioData->frameId += nframes;
  return 0;

  // playbackSpeed
  const float* processHeadL = audioData->inputBuffers[0] + audioData->frameId;
  const float* processHeadR = audioData->inputBuffers[1] + audioData->frameId;

  const float playbackSpeedF = static_cast<float>(playbackSpeed) / 100.0f;
  float playbackPos = 0.0f;
  float playbackPosTrunc = 0.0f;
  int idx = 0;
  for (jack_nframes_t i = 0; i < nframes; i++) {
    playbackPosTrunc = std::trunc(playbackPos);
    idx = static_cast<int>(playbackPosTrunc);
    const float frac = playbackPos - playbackPosTrunc;
    audioData->playbackBuffers[0][i] = (1.0f - frac) * processHeadL[idx] + frac * processHeadL[idx+1];
    audioData->playbackBuffers[1][i] = (1.0f - frac) * processHeadR[idx] + frac * processHeadR[idx+1];
    playbackPos += playbackSpeedF;
  }

  audioData->frameId += idx;
  return 0;
}

// jack process callback
// do not allocate/free memory within this method
// as it may be called at system-interrupt level
int JackClient::processCallback(jack_nframes_t nframes, void *arg) {
  // get port buffers
  auto *outL = static_cast<jack_default_audio_sample_t *>(
    jack_port_get_buffer(outPortL, nframes)
  );
  auto *outR = static_cast<jack_default_audio_sample_t *>(
    jack_port_get_buffer(outPortR, nframes)
  );

  // retrieve AudioData
  const auto audioData = static_cast<AudioData*>(arg);

  // read playbackSettingsToAudioThreadRingBuffer
  if (jack_ringbuffer_read_space(audioData->playbackSettingsToAudioThreadRB) > PlaybackSettingsToAudioThread_RB_SIZE - 2) {
    jack_ringbuffer_read(
      audioData->playbackSettingsToAudioThreadRB,
      reinterpret_cast<char *>(audioData->playbackSettingsToAudioThread),
      PlaybackSettingsToAudioThread_RB_SIZE
    );
  }

  if (audioData->playbackSettingsToAudioThread[0] == 1) // user set frame Id
    audioData->frameId = audioData->playbackSettingsToAudioThread[1];

  audioData->playbackSettingsFromAudioThread[0] = 0; // debug value
  audioData->playbackSettingsFromAudioThread[1] = audioData->frameId;

  // write to playbackSettingsFromAudioThread ring buffer
  if (jack_ringbuffer_write_space(audioData->playbackSettingsFromAudioThreadRB) > PlaybackSettingsFromAudioThread_RB_SIZE - 2) {
    jack_ringbuffer_write(
      audioData->playbackSettingsFromAudioThreadRB,
      reinterpret_cast<char *>(audioData->playbackSettingsFromAudioThread),
      PlaybackSettingsFromAudioThread_RB_SIZE
    );
  }

  audioData->fillPlaybackBuffer(
    audioData,
    audioData->playbackSettingsToAudioThread[2],
    nframes
  );

  // process effects channels
  // main channel is effectsChannelIdx 0
  for (int effectsChannelIdx = 1; effectsChannelIdx < audioData->effectsChannelCount + 1; effectsChannelIdx++) {
    auto [effectCount, processFuncs, buffers] = audioData->effectsChannelsProcessData[effectsChannelIdx];
    for (int pluginIdx = 0; pluginIdx < effectCount; pluginIdx++) {
      if (pluginIdx == 0) {
        buffers[pluginIdx].inputs = static_cast<float**>(audioData->playbackBuffers);
      }
      buffers[pluginIdx].numSamples = static_cast<int32_t>(nframes);

      processFuncs[pluginIdx](
        buffers[pluginIdx],
        nframes
      );
    }
  }

  // read channel settings from ringbuffer
  if (jack_ringbuffer_t* ringBuffer = audioData->effectsChannelsSettingsRB; jack_ringbuffer_read_space(ringBuffer) >= EffectsSettings_RB_SIZE) {
    jack_ringbuffer_read(ringBuffer, reinterpret_cast<char *>(audioData->effectsChannelsSettings),
                         EffectsSettings_RB_SIZE);
  }

  // sum down
  for (int i = 0; i < nframes; i++) {
    for (int effectsChannelIdx = 1; effectsChannelIdx < audioData->effectsChannelCount + 1; effectsChannelIdx++) {
      const float factorLL = audioData->effectsChannelsSettings[4 * effectsChannelIdx];
      const float factorLR = audioData->effectsChannelsSettings[4 * effectsChannelIdx + 1];
      const float factorRL = audioData->effectsChannelsSettings[4 * effectsChannelIdx + 2];
      const float factorRR = audioData->effectsChannelsSettings[4 * effectsChannelIdx + 3];

      if (effectsChannelIdx == 1) {
        if (audioData->effectsChannelsProcessData[effectsChannelIdx].effectCount == 0) {
          audioData->processBuffers[0][i] = factorLL * audioData->playbackBuffers[0][i] + factorRL * audioData->playbackBuffers[1][i];
          audioData->processBuffers[1][i] = factorLR * audioData->playbackBuffers[0][i] + factorRR * audioData->playbackBuffers[1][i];
        } else {
          audioData->processBuffers[0][i] = factorLL * audioData->effectsChannelsWriteOut[effectsChannelIdx][0][i] +
                                           factorRL * audioData->effectsChannelsWriteOut[effectsChannelIdx][1][i];
          audioData->processBuffers[1][i] = factorLR * audioData->effectsChannelsWriteOut[effectsChannelIdx][0][i] +
                                           factorRR * audioData->effectsChannelsWriteOut[effectsChannelIdx][1][i];
        }
      } else {
        if (audioData->effectsChannelsProcessData[effectsChannelIdx].effectCount == 0) {
          audioData->processBuffers[0][i] += factorLL * audioData->playbackBuffers[0][i] + factorRL * audioData->playbackBuffers[1][i];
          audioData->processBuffers[1][i] += factorLR * audioData->playbackBuffers[0][i] + factorRR * audioData->playbackBuffers[1][i];
        } else {
          audioData->processBuffers[0][i] += factorLL * audioData->effectsChannelsWriteOut[effectsChannelIdx][0][i] +
                                            factorRL * audioData->effectsChannelsWriteOut[effectsChannelIdx][1][i];
          audioData->processBuffers[1][i] += factorLR * audioData->effectsChannelsWriteOut[effectsChannelIdx][0][i] +
                                            factorRR * audioData->effectsChannelsWriteOut[effectsChannelIdx][1][i];
        }
      }
    }
  }

  // process summed down mix through main effects
  auto [effectCount, processFuncs, buffers] = audioData->effectsChannelsProcessData[0];
  for (int pluginIdx = 0; pluginIdx < effectCount; pluginIdx++) {
    buffers[pluginIdx].numSamples = static_cast<int32_t>(nframes);

    processFuncs[pluginIdx](
      buffers[pluginIdx],
      nframes
    );
  }

  // write out processed main channel to audio out
  const float factorLL = audioData->effectsChannelsSettings[0];
  const float factorLR = audioData->effectsChannelsSettings[1];
  const float factorRL = audioData->effectsChannelsSettings[2];
  const float factorRR = audioData->effectsChannelsSettings[3];
  for (int i = 0; i < nframes; i++) {
    outL[i] = factorLL * audioData->processBuffers[0][i] + factorRL * audioData->processBuffers[1][i];
    outR[i] = factorLR * audioData->processBuffers[0][i] + factorRR * audioData->processBuffers[1][i];
  }

  if (audioData->frameId >= audioData->frames - nframes)
    audioData->readComplete = true;

  return kJackSuccess;
}

int JackClient::setSampleRateCallback(jack_nframes_t nframes, void *arg) {
  if (const auto* mixer = static_cast<Mixer*>(arg); mixer->setSampleRate(nframes) != OK)
    return kJackError;

  return kJackSuccess;
}

int JackClient::setBufferSizeCallback(jack_nframes_t nframes, void *arg) {
  if (const auto* mixer = static_cast<Mixer*>(arg); mixer->setAudioFramesPerBuffer(nframes) != OK)
    return kJackError;

  return kJackSuccess;
}

int JackClient::xRunCallback(void* arg) {
  logXRun();
  return kJackSuccess;
}

bool JackClient::registerMidiPorts(IMidiClient* processor) {
  const auto ioSetup = processor->getMidiIOSetup();
  for (const auto &input: ioSetup.inputs)
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
    auto *portBuffer = jack_port_get_buffer(midiInputPort, nframes);
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

bool JackClient::registerMidiClient(IMidiClient *client) {
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
bool JackClient::autoConnectMidiPorts(jack_client_t *client) {
  int portIndex = 1;

  //! Connect MIDI Inputs
  auto ports = jack_get_ports(client, nullptr, "midi", JackPortIsPhysical | JackPortIsOutput);
  if (!ports)
    return false;

  for (auto &port: midiInputPorts) {
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
