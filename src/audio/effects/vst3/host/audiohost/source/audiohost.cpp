//-----------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : AudioHost
// Filename    : public.sdk/samples/vst-hosting/audiohost/source/audiohost.cpp
// Created by  : Steinberg 09.2016
// Description : Audio Host Example for VST 3
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

#include "./audiohost.h"
#include "./platform/appinit.h"
#include "public.sdk/source/vst/hosting/hostclasses.h"
#include "public.sdk/source/vst/utility/stringconvert.h"
#include "base/source/fcommandline.h"
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/fstrdefs.h"
#include "pluginterfaces/gui/iplugview.h"
#include "pluginterfaces/gui/iplugviewcontentscalesupport.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/vsttypes.h"
#include "../../../../../constants.h"
#include <cstdio>
#include <iostream>

//#if WIN32
//#include "windows.h"
//#include <wtypes.h>
//#endif

//------------------------------------------------------------------------
namespace Steinberg {
FUnknown* gStandardPluginContext = new Vst::HostApplication ();

namespace Vst {
namespace AudioHost {
static AudioHost::AppInit gInit (std::make_unique<App> ());

//------------------------------------------------------------------------
App::~App () noexcept
{
}

//------------------------------------------------------------------------
void App::startAudioClient (const std::string& path, VST3::Optional<VST3::UID> effectID,
                            uint32 flags)
{
	std::string error;
	module = VST3::Hosting::Module::create (path, error);
	if (!module)
	{
        std::cout << "\n !module starAudioClient!";
		std::string reason = "Could not create Module for file:";
		reason += path;
		reason += "\nError: ";
		reason += error;
		// EditorHost::IPlatform::instance ().kill (-1, reason);
        return;
	}
	auto factory = module->getFactory ();
	for (auto& classInfo : factory.classInfos ())
	{
		if (classInfo.category () == kVstAudioEffectClass)
		{
			if (effectID)
			{
				if (*effectID != classInfo.ID ())
					continue;
			}
			plugProvider = owned (new PlugProvider (factory, classInfo, true));
			break;
		}
	}
    std::cout << "\n plugProviders init starAudioClient!";
	if (!plugProvider)
	{
        std::cout << "\n !plugProvider starAudioClient!";
		std::string error;
		if (effectID)
			error =
			    "No VST3 Audio Module Class with UID " + effectID->toString () + " found in file ";
		else
			error = "No VST3 Audio Module Class found in file ";
		error += path;
		// EditorHost::IPlatform::instance ().kill (-1, error);
        return;
	}

	OPtr<IComponent> component = plugProvider->getComponent ();
	OPtr<IEditController> controller = plugProvider->getController ();
	auto midiMapping = U::cast<IMidiMapping> (controller);

	std::string name;
	name = plugProvider->getClassInfo().name();
	vst3Processor = AudioClient::create (name, component, midiMapping);
//	vst3Processor->setParameter (ParamID id, ParamValue value, int32 sampleOffset) override;
    vst3Processor->setParameter(0, 0.0, 0); // "chew_depth"
    // vst3Processor->setParameter(1, 0.0, 0); // "chew_freq"
    // vst3Processor->setParameter(2, 0.0, 0); // "chew_var"
    // vst3Processor->setParameter(3, 0.0, 0); // "deg_amt"
    // vst3Processor->setParameter(4, 0.0, 0); // "deg_depth"
    // vst3Processor->setParameter(5, 0.0, 0); // "deg_var"
    // vst3Processor->setParameter(6, 0.0, 0); // "depth"
    // vst3Processor->setParameter(7, 1.0, 0); // "drive"
    // vst3Processor->setParameter(8, 100.0, 0); // "drywet"
    // vst3Processor->setParameter(9, 9.999999974752427e-7, 0); // "gap"
    // vst3Processor->setParameter(10, 0.0, 0); // "h_bass"
    // vst3Processor->setParameter(11, 0.0, 0); // "h_treble"
    // vst3Processor->setParameter(12, 0.0, 0); // "ingain"
    // vst3Processor->setParameter(13, 2, 0);   // "mode"
    // vst3Processor->setParameter(14, 2, 0);   // "os"
    // vst3Processor->setParameter(15, -3.5, 0);  // "outgain"
    // vst3Processor->setParameter(16, 0.2999999821186066, 0); // "rate"
    // vst3Processor->setParameter(17, 1.0, 0); // "sat"
    // vst3Processor->setParameter(18, 2.999998396262527, 0); // "spacing"
    // vst3Processor->setParameter(19, 7.500000476837158, 0); // "speed"
    // vst3Processor->setParameter(20, 4.999995231628418, 0); // "thick"
    // vst3Processor->setParameter(21, 0.5999999642372131, 0); // "width"
    // vst3Processor->setParameter(22, 0.0, 0); // "wow_depth"
    vst3Processor->setParameter(23, 0.25, 0); // "wow_rate"
    // vst3Processor->setParameter(24, 3, 0); // "preset"
    std::cout << "\n Finished setting parameters - which may or may not be id-ed correctly :shrug:" << std::endl;
}

//------------------------------------------------------------------------
void App::init (const std::vector<std::string>& cmdArgs)
{
	if (cmdArgs.empty ())
	{
		/*auto helpText = R"(
		usage: AudioHost pluginPath
		)";
		*/
		return;
	}

	VST3::Optional<VST3::UID> uid;
	uint32 flags {};

	allocateBuffers ();
	startAudioClient (cmdArgs.back (), std::move (uid), flags);
}

void App::allocateBuffers()
{
	int channelCount = 2; // TODO: access info from libsndfile
	// std::cout << "\n HERE \n";
	auto chowTapeModelInputs = static_cast<float**>(
		malloc(2 * AUDIO_BUFFER_FRAMES * sizeof(float*))
	);
    auto chowTapeModelOutputs = static_cast<float**>(
		malloc(2 * AUDIO_BUFFER_FRAMES * sizeof(float*))
	);

	if (chowTapeModelInputs == NULL || chowTapeModelOutputs == NULL) {
		std::cout << "Unable to allocate memory for chowTapeModelInputs or chowTapeModelOutputs." << std::endl;
		throw std::runtime_error ("Unable to allocate memory for chowTapeModelInputs.");
	}

	for (int c = 0; c < channelCount; c++) {
		chowTapeModelInputs[c] = new float[AUDIO_BUFFER_FRAMES];
		chowTapeModelOutputs[c] = new float[AUDIO_BUFFER_FRAMES];
	}

    chowTapeModelBuffers = { // Steinberg::Vst::IAudioClient::Buffers
         chowTapeModelInputs,
        2,
        chowTapeModelOutputs,
        2,
        AUDIO_BUFFER_FRAMES
    };
}

//------------------------------------------------------------------------
void App::terminate ()
{
    delete chowTapeModelBuffers.inputs;
    delete chowTapeModelBuffers.outputs;
    delete this;
}

//------------------------------------------------------------------------
} // EditorHost
} // Vst
} // Steinberg

//------------------------------------------------------------------------
//#if WIN32
//int wmain (int argc, wchar_t* argv[])
//{
//	std::vector<std::string> cmdArgs;
//	for (int i = 1; i < argc; ++i)
//		cmdArgs.push_back (Steinberg::Vst::StringConvert::convert (Steinberg::wscast (argv[i])));
//
//	Steinberg::Vst::AudioHost::gInit.app->init (cmdArgs);
//
//	std::cout << "Press <enter> to continue . . .";
//	std::getchar ();
//
//	return 0;
//}
//#else
//int main (int argc, char* argv[])
//
//{
//	std::vector<std::string> cmdArgs;
//	for (int i = 1; i < argc; ++i)
//		cmdArgs.push_back (argv[i]);
//
//	Steinberg::Vst::AudioHost::gInit.app->init (cmdArgs);
//
//	std::cout << "Press <enter> to continue . . .";
//	std::getchar ();
//
//	return 0;
//}
//#endif
