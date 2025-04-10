//
// Created by ns on 4/6/25.
//

#ifndef GJAUDIOJACKCLIENT_H
#define GJAUDIOJACKCLIENT_H

#include "public.sdk/samples/vst-hosting/audiohost/source/media/imediaserver.h"

#include <cassert>

#include <jack/jack.h>
#include <jack/midiport.h>

#include "Logging.h"

namespace Gj {
namespace Audio {

using namespace Steinberg::Vst;

//------------------------------------------------------------------------
//  jack Client
//------------------------------------------------------------------------
class JackClient : public IMediaServer {

public:
//--------------------------------------------------------------------
	using JackPorts = std::vector<jack_port_t*>;
	using JackName = std::string;

	JackClient () = default;
	~JackClient () override;

	[[nodiscard]]
	jack_client_t* getJackClient() const {
		return jackClient;
	};

	// IMediaServer interface
	bool registerAudioClient (IAudioClient* client) override;
	bool registerMidiClient (IMidiClient* client) override;

	bool initialize (JackName name);

	// jack process callback
	int process (jack_nframes_t nframes);

//--------------------------------------------------------------------
private:
	jack_client_t* registerClient (JackName name);
	bool registerAudioPorts (IAudioClient* processor);
	bool registerMidiPorts (IMidiClient* processor);
	bool addAudioOutputPort (JackName name);
	bool addAudioInputPort (JackName name);
	bool addMidiInputPort (JackName name);
	int processMidi (jack_nframes_t nframes);
	bool setupJackProcessCallbacks (jack_client_t* client);
	bool autoConnectAudioPorts (jack_client_t* client);
	bool autoConnectMidiPorts (jack_client_t* client);
	void updateAudioBuffers (jack_nframes_t nframes);

	// Jack objects
	jack_client_t* jackClient = nullptr;
	JackPorts audioOutputPorts;
	JackPorts audioInputPorts;
	JackPorts midiInputPorts;

	Steinberg::Vst::IAudioClient* audioClient = nullptr;
	Steinberg::Vst::IMidiClient* midiClient = nullptr;
	using BufferPointers = std::vector<jack_default_audio_sample_t*>;
	BufferPointers audioOutputPointers;
	BufferPointers audioInputPointers;
	Steinberg::Vst::IAudioClient::Buffers buffers {nullptr};
};

} // Audio
} // Gj

#endif //JACKCLIENT_H
