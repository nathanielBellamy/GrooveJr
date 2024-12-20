//
// Created by ns on 12/7/24.
//

#include "App.h"

namespace Gj {
namespace Effects {
namespace Vst3 {
namespace Host {

using namespace Steinberg;
using namespace Steinberg::Vst;

//-----------------------------------------------------------------------------
App::App ()
{
	FUNKNOWN_CTOR

	mPlugInterfaceSupport = owned (new PlugInterfaceSupport);
}

tresult App::terminate() {
  	std::cout << "Terminating Gj::Effects::Vst3::Host::App..." << std::endl;

    this->release();

    std::cout << "Finished Terminating Gj::Effects::Vst3::Host::App..." << std::endl;
    return kResultOk;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API App::getName (String128 name)
{
	return StringConvert::convert ("GrooveJr", name) ? kResultTrue : kInternalError;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API App::createInstance (TUID cid, TUID _iid, void** obj)
{
	if (FUnknownPrivate::iidEqual (cid, IMessage::iid) &&
	    FUnknownPrivate::iidEqual (_iid, IMessage::iid))
	{
		*obj = new HostMessage;
		return kResultTrue;
	}
	if (FUnknownPrivate::iidEqual (cid, IAttributeList::iid) &&
	    FUnknownPrivate::iidEqual (_iid, IAttributeList::iid))
	{
		if (auto al = AttributeList::make ())
		{
			*obj = al.take ();
			return kResultTrue;
		}
		return kOutOfMemory;
	}
	*obj = nullptr;
	return kResultFalse;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API App::queryInterface (const char* _iid, void** obj)
{
	QUERY_INTERFACE (_iid, obj, FUnknown::iid, IHostApplication)
	QUERY_INTERFACE (_iid, obj, IHostApplication::iid, IHostApplication)

	if (mPlugInterfaceSupport && mPlugInterfaceSupport->queryInterface (iid, obj) == kResultTrue)
		return kResultOk;

	*obj = nullptr;
	return kResultFalse;
}

//-----------------------------------------------------------------------------
uint32 PLUGIN_API App::addRef ()
{
	return 1;
}

//-----------------------------------------------------------------------------
uint32 PLUGIN_API App::release ()
{
	return 1;
}

} // Host
} // Vst3
} // Effects
} // Gj
