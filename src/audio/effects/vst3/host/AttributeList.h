//
// Created by ns on 12/20/24.
//

#ifndef GJEFFECTSVST3HOSTATTRIBUTELIST_H
#define GJEFFECTSVST3HOSTATTRIBUTELIST_H

#include "public.sdk/source/vst/hosting/pluginterfacesupport.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include <map>
#include <memory>
#include <string>

namespace Gj {
namespace Audio {
namespace Effects {
namespace Vst3 {
namespace Host {

using namespace Steinberg;
using namespace Steinberg::Vst;

class AttributeList  : public IAttributeList
{
public:
	/** make a new attribute list instance */
	static IPtr<IAttributeList> make ();

	tresult PLUGIN_API setInt (AttrID aid, int64 value) override;
	tresult PLUGIN_API getInt (AttrID aid, int64& value) override;
	tresult PLUGIN_API setFloat (AttrID aid, double value) override;
	tresult PLUGIN_API getFloat (AttrID aid, double& value) override;
	tresult PLUGIN_API setString (AttrID aid, const TChar* string) override;
	tresult PLUGIN_API getString (AttrID aid, TChar* string, uint32 sizeInBytes) override;
	tresult PLUGIN_API setBinary (AttrID aid, const void* data, uint32 sizeInBytes) override;
	tresult PLUGIN_API getBinary (AttrID aid, const void*& data, uint32& sizeInBytes) override;

	virtual ~AttributeList () noexcept;
	DECLARE_FUNKNOWN_METHODS
private:
	AttributeList ();

	struct Attribute;
	std::map<std::string, Attribute> list;
};

} // Host
} // Vst3
} // Effects
} // Audio
} // Gj

#endif //ATTRIBUTELIST_H
