//
// Created by ns on 12/20/24.
//

#ifndef COMPONENTHANDLER_H
#define COMPONENTHANDLER_H

#include <iostream>

#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/hosting/plugprovider.h"

namespace Steinberg {
namespace Vst {
namespace EditorHost {

//------------------------------------------------------------------------
class ComponentHandler : public IComponentHandler
{
public:
	tresult PLUGIN_API beginEdit (ParamID id) override
	{
		return kNotImplemented;
	}
	tresult PLUGIN_API performEdit (ParamID id, ParamValue valueNormalized) override
	{
		return kNotImplemented;
	}
	tresult PLUGIN_API endEdit (ParamID id) override
	{
		return kNotImplemented;
	}
	tresult PLUGIN_API restartComponent (int32 flags) override
	{
		return kNotImplemented;
	}

private:
	tresult PLUGIN_API queryInterface (const TUID /*_iid*/, void** /*obj*/) override
	{
		return kNoInterface;
	}
	// we do not care here of the ref-counting. A plug-in call of release should not destroy this
	// class!
	uint32 PLUGIN_API addRef () override { return 1000; }
	uint32 PLUGIN_API release () override { return 1000; }
};

static ComponentHandler gComponentHandler;

} // EditorHost
} // Vst
} // Steinberg

#endif //COMPONENTHANDLER_H
