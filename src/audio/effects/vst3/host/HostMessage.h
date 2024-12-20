//
// Created by ns on 12/20/24.
//

#ifndef GJEFFECTSVST3HOSTHOSTMESSAGE_H
#define GJEFFECTSVST3HOSTHOSTMESSAGE_H

#include "public.sdk/source/vst/hosting/pluginterfacesupport.h"
#include "pluginterfaces/vst/ivsthostapplication.h"

#include "./AttributeList.h"

namespace Gj {
namespace Effects {
namespace Vst3 {
namespace Host {

using namespace Steinberg;
using namespace Steinberg::Vst;

class HostMessage final : public IMessage
{
public:
	HostMessage ();
	virtual ~HostMessage () noexcept;

	const char* PLUGIN_API getMessageID () override;
	void PLUGIN_API setMessageID (const char* messageID) override;
	IAttributeList* PLUGIN_API getAttributes () override;

	DECLARE_FUNKNOWN_METHODS
private:
	char* messageId {nullptr};
	IPtr<IAttributeList> attributeList;
};



#endif //HOSTMESSAGE_H

} // Host
} // Vst3
} // Effects
} // Gj
