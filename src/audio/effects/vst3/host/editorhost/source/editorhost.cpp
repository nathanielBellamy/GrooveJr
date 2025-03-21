//-----------------------------------------------------------------------------
// Flags       : clang-format auto
// Project     : VST SDK
//
// Category    : EditorHost
// Filename    : public.sdk/samples/vst-hosting/editorhost/source/editorhost.cpp
// Created by  : Steinberg 09.2016
// Description : Example of opening a plug-in editor
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

#include "public.sdk/samples/vst-hosting/editorhost/source/platform/appinit.h"
#include "base/source/fcommandline.h"
// #include "base/source/fdebug.h"
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/gui/iplugview.h"
#include "pluginterfaces/gui/iplugviewcontentscalesupport.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/vsttypes.h"
#include <cstdio>
#include <iostream>
#include "editorhost.h"

//------------------------------------------------------------------------
namespace Steinberg {

//------------------------------------------------------------------------
inline bool operator== (const ViewRect& r1, const ViewRect& r2)
{
	return memcmp (&r1, &r2, sizeof (ViewRect)) == 0;
}

//------------------------------------------------------------------------
inline bool operator!= (const ViewRect& r1, const ViewRect& r2)
{
	return !(r1 == r2);
}

namespace Vst {
namespace EditorHost {

// static AppInit gInit (std::make_unique<App> ());

App::App(WindowPtr window)
	: window(window)
	{}

//------------------------------------------------------------------------
App::~App () noexcept
{
	terminate ();
}

void App::setModule (VST3::Hosting::Module::Ptr module_) {
  module = module_;
}

//------------------------------------------------------------------------
void App::openEditor (const std::string& path, VST3::Optional<VST3::UID> effectID, uint32 flags)
{
	std::string error;

	auto factory = module->getFactory ();
	if (auto factoryHostContext = IPlatform::instance ().getPluginFactoryContext ())
		factory.setHostContext (factoryHostContext);

	Gj::Audio::Logging::write(
		Gj::Audio::LogSeverityLevel::Info,
		"EditorHost::openEditor",
		"Retrived factory from module"
	);

	if (!plugProvider)
	{
		Gj::Audio::Logging::write(
			Gj::Audio::LogSeverityLevel::Error,
			"EditorHost::openEditor",
			"No VST3 plugProvider."
		);
		if (effectID)
			error =
			    "No VST3 Audio Module Class with UID " + effectID->toString () + " found in file ";
		else
			error = "No VST3 Audio Module Class found in file ";
		error += path;
		IPlatform::instance ().kill (-1, error);
	}

	if (!editController)
	{
		Gj::Audio::Logging::write(
			Gj::Audio::LogSeverityLevel::Error,
			"EditorHost::openEditor",
			"No VST3 editController."
		);
		error = "No EditController found (needed for allowing editor) in file " + path;
		IPlatform::instance ().kill (-1, error);
	}
	editController->release (); // plugProvider does an addRef

	// if (flags & kSetComponentHandler)
	if (true)
	{
		// SMTG_DBPRT0 ("setComponentHandler is used\n");
		editController->setComponentHandler (&gComponentHandler);

		Gj::Audio::Logging::write(
			Gj::Audio::LogSeverityLevel::Info,
			"EditorHost::openEditor",
			"Set editController componentHandler"
		);

		// connect the 2 components
		Vst::IConnectionPoint* iConnectionPointComponent = nullptr;
		Vst::IConnectionPoint* iConnectionPointController = nullptr;

		processorComponent->queryInterface (Vst::IConnectionPoint::iid, (void**)&iConnectionPointComponent);
		editController->queryInterface (Vst::IConnectionPoint::iid, (void**)&iConnectionPointController);

		if (iConnectionPointComponent && iConnectionPointController)
		{
			iConnectionPointComponent->connect (iConnectionPointController);
			iConnectionPointController->connect (iConnectionPointComponent);

			Gj::Audio::Logging::write(
				Gj::Audio::LogSeverityLevel::Info,
				"EditorHost::openEditor",
				"Connected controller and component thru ConnectionPoints"
			);
		}

		// TODO: debug undefined MemoryStream ctor + dtor
		// synchronize controller to component by using setComponentState
		// Steinberg::MemoryStream stream; // defined in "public.sdk/source/common/memorystream.h"
		// // stream.setByteOrder (kLittleEndian);
		// if (processorComponent->getState (&stream) == kResultTrue)
		// {
		// 	// stream.rewind ();
		// 	editController->setComponentState (&stream);
		// }
	}

	// SMTG_DBPRT1 ("Open Editor for %s...\n", path.c_str ());
	createViewAndShow (editController);

	Gj::Audio::Logging::write(
		Gj::Audio::LogSeverityLevel::Info,
		"EditorHost::openEditor",
		"Created view and showed"
	);

	if (flags & kSecondWindow)
	{
		// SMTG_DBPRT0 ("Open 2cd Editor...\n");
		createViewAndShow (editController);
	}
}

//------------------------------------------------------------------------
void App::createViewAndShow (IEditController* controller)
{
	auto view = owned (controller->createView (ViewType::kEditor));
	if (!view)
	{
		Gj::Audio::Logging::write(
			Gj::Audio::LogSeverityLevel::Error,
			"EditorHost::createViewAndShow",
			"Could not create view"
		);
		IPlatform::instance ().kill (-1, "EditController does not provide its own editor");
	}

	ViewRect plugViewSize {};
	auto result = view->getSize (&plugViewSize);
	if (result != kResultTrue)
	{
		Gj::Audio::Logging::write(
			Gj::Audio::LogSeverityLevel::Error,
			"EditorHost::createViewAndShow",
			"Could not get view size"
		);
		IPlatform::instance ().kill (-1, "Could not get editor view size");
	}

	auto viewRect = ViewRectToRect (plugViewSize);
	Gj::Audio::Logging::write(
		Gj::Audio::LogSeverityLevel::Info,
		"EditorHost::createViewAndShow",
		"Retrieved view rect"
	);

	windowController = std::make_shared<WindowController> (view);
		// IPlatform::instance ().createWindow (
	 //    "Gj Editor", viewRect.size, view->canResize () == kResultTrue, windowController);
	if (!window)
	{
		Gj::Audio::Logging::write(
			Gj::Audio::LogSeverityLevel::Error,
			"EditorHost::createViewAndShow",
			"Could not create window"
		);
		IPlatform::instance ().kill (-1, "Could not create window");
	}

	Gj::Audio::Logging::write(
		Gj::Audio::LogSeverityLevel::Info,
		"EditorHost::createViewAndShow",
		"Will call window->show()"
	);
	window->show ();
	// TODO
	try {
		windowController->onShow(*window.get());
	} catch (const std::exception& e) {
		std::cout << e.what () << std::endl;
		Gj::Audio::Logging::write(
			Gj::Audio::LogSeverityLevel::Error,
			"EditorHost::createViewAndShow",
			"An Error occurred when calling windowController->onShow"
		);
	}
	Gj::Audio::Logging::write(
		Gj::Audio::LogSeverityLevel::Info,
		"EditorHost::createViewAndShow",
		"Called window->show()"
	);
}

//------------------------------------------------------------------------
void App::init (const std::vector<std::string>& cmdArgs)
{
	VST3::Optional<VST3::UID> uid;
	uint32 flags {};
	for (auto it = cmdArgs.begin (), end = cmdArgs.end (); it != end; ++it)
	{
		if (*it == "--componentHandler")
			flags |= kSetComponentHandler;
		else if (*it == "--secondWindow")
			flags |= kSecondWindow;
		else if (*it == "--uid")
		{
			if (++it != end)
				uid = VST3::UID::fromString (*it);
			if (!uid)
				IPlatform::instance ().kill (-1, "wrong argument to --uid");
		}
	}

	if (cmdArgs.empty () || cmdArgs.back ().find (".vst3") == std::string::npos)
	{
		auto helpText = R"(
usage: EditorHost [options] pluginPath

options:

--componentHandler
	set optional component handler on edit controller

--secondWindow
	create a second window

--uid UID
	use effect class with unique class ID==UID
)";

		IPlatform::instance ().kill (0, helpText);
	}

	Gj::Audio::Logging::write(
		Gj::Audio::LogSeverityLevel::Info,
		"EditorHost::init",
		"Handled cmdArgs - will call openEditor"
	);
	openEditor (cmdArgs.back (), std::move (uid), flags);
}

//------------------------------------------------------------------------
void App::terminate ()
{
    PluginContextFactory::instance ().setPluginContext (nullptr);
	if (windowController)
		windowController->closePlugView ();
	windowController.reset ();
	plugProvider.reset ();
	module.reset ();
}

//------------------------------------------------------------------------
WindowController::WindowController (const IPtr<IPlugView>& plugView) : plugView (plugView)
{
}

//------------------------------------------------------------------------
WindowController::~WindowController () noexcept
{
}

//------------------------------------------------------------------------
void WindowController::onShow (IWindow& w)
{
	// SMTG_DBPRT1 ("onShow called (%p)\n", (void*)&w);

	window = &w;
	if (!plugView)
		return;

	auto platformWindow = window->getNativePlatformWindow ();
	if (plugView->isPlatformTypeSupported (platformWindow.type) != kResultTrue)
	{
		IPlatform::instance ().kill (-1, std::string ("PlugView does not support platform type: ") +
		                                     platformWindow.type);
	}

	plugView->setFrame (this);

	if (plugView->attached (platformWindow.ptr, platformWindow.type) != kResultTrue) {
		IPlatform::instance ().kill (-1, "Attaching PlugView failed");
		Gj::Audio::Logging::write(
			Gj::Audio::LogSeverityLevel::Error,
			"WindowController::onShow",
			"Could not attach to PlugView"
		);
	}
	Gj::Audio::Logging::write(
		Gj::Audio::LogSeverityLevel::Info,
		"WindowController::onShow",
		"PlugView attached"
	);
}

//------------------------------------------------------------------------
void WindowController::closePlugView ()
{
	if (plugView)
	{
		plugView->setFrame (nullptr);
		if (plugView->removed () != kResultTrue)
		{
			IPlatform::instance ().kill (-1, "Removing PlugView failed");
		}
		plugView = nullptr;
	}
	// window = nullptr;
}

//------------------------------------------------------------------------
void WindowController::onClose (IWindow& w)
{
	// SMTG_DBPRT1 ("onClose called (%p)\n", (void*)&w);

	closePlugView ();

	// TODO maybe quit only when the last window is closed
	IPlatform::instance ().quit ();
}

//------------------------------------------------------------------------
void WindowController::onResize (IWindow& w, Size newSize)
{
	// SMTG_DBPRT1 ("onResize called (%p)\n", (void*)&w);

	if (plugView)
	{
		ViewRect r {};
		r.right = newSize.width;
		r.bottom = newSize.height;
		ViewRect r2 {};
		if (plugView->getSize (&r2) == kResultTrue && r != r2)
			plugView->onSize (&r);
	}
}

//------------------------------------------------------------------------
Size WindowController::constrainSize (IWindow& w, Size requestedSize)
{
	// SMTG_DBPRT1 ("constrainSize called (%p)\n", (void*)&w);

	ViewRect r {};
	r.right = requestedSize.width;
	r.bottom = requestedSize.height;
	if (plugView && plugView->checkSizeConstraint (&r) != kResultTrue)
	{
		plugView->getSize (&r);
	}
	requestedSize.width = r.right - r.left;
	requestedSize.height = r.bottom - r.top;
	return requestedSize;
}

//------------------------------------------------------------------------
void WindowController::onContentScaleFactorChanged (IWindow& w, float newScaleFactor)
{
	// SMTG_DBPRT1 ("onContentScaleFactorChanged called (%p)\n", (void*)&w);

	if (auto css = U::cast<IPlugViewContentScaleSupport> (plugView))
	{
		css->setContentScaleFactor (newScaleFactor);
	}
}

//------------------------------------------------------------------------
tresult PLUGIN_API WindowController::resizeView (IPlugView* view, ViewRect* newSize)
{
	// SMTG_DBPRT1 ("resizeView called (%p)\n", (void*)view);

	if (newSize == nullptr || view == nullptr || view != plugView)
		return kInvalidArgument;
	if (!window)
		return kInternalError;
	if (resizeViewRecursionGard)
		return kResultFalse;
	ViewRect r;
	if (plugView->getSize (&r) != kResultTrue)
		return kInternalError;
	if (r == *newSize)
		return kResultTrue;

	resizeViewRecursionGard = true;
	Size size {newSize->right - newSize->left, newSize->bottom - newSize->top};
	window->resize (size);
	resizeViewRecursionGard = false;
	if (plugView->getSize (&r) != kResultTrue)
		return kInternalError;
	if (r != *newSize)
		plugView->onSize (newSize);
	return kResultTrue;
}

//------------------------------------------------------------------------
} // EditorHost
} // Vst
} // Steinberg
