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
#include <cstdio>
#include <iostream>

#include "../../../../../JackClient.h"

//#if WIN32
//#include "windows.h"
//#include <wtypes.h>
//#endif

//------------------------------------------------------------------------
namespace Steinberg {

namespace Vst {
namespace AudioHost {
// static AudioHost::AppInit gInit (std::make_unique<App> (128));

void App::setModule (VST3::Hosting::Module::Ptr module_) {
  module = module_;
}

App::App(Gj::AppState* gAppState, std::shared_ptr<Gj::Audio::JackClient> jackClient)
	: gAppState(gAppState)
	, jackClient(jackClient)
	{
}

//------------------------------------------------------------------------
App::~App () noexcept
{
}

//------------------------------------------------------------------------
void App::startAudioClient (const std::string& path, VST3::Optional<VST3::UID> effectID,
                            uint32 flags)
{

	std::string error;
	const auto factory = module->getFactory ();
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
	if (!plugProvider)
	{
		std::string error;
		if (effectID)
			error =
			    "No VST3 Audio Module Class with UID " + effectID->toString () + " found in file ";
		else
			error = "No VST3 Audio Module Class found in file ";
		error += path;

		// TODO: debug linking Logging
		// Gj::Audio::Logging::write(
		// 	Gj::Audio::LogSeverityLevel::Error,
		// 	"Audio::Steinberg::Vst::AudioHost::App::startAudioClient",
		// 	"Unable to start audio client. Message: " + error
		// );

		// EditorHost::IPlatform::instance ().kill (-1, error);
        return;
	}

	component = plugProvider->getComponent ();
	editController = plugProvider->getController ();
	const auto midiMapping = U::cast<IMidiMapping> (editController);

	std::string name = plugProvider->getClassInfo().name();
	audioClient = AudioClient::create (name, component, midiMapping, jackClient);
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

	startAudioClient (cmdArgs.back (), std::move (uid), flags);
}

//------------------------------------------------------------------------
void App::terminate ()
{
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
