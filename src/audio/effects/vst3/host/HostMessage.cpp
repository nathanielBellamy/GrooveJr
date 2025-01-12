//
// Created by ns on 12/20/24.
//

#include "HostMessage.h"


#include <algorithm>

namespace Gj {
namespace Audio {
namespace Effects {
namespace Vst3 {
namespace Host {

using namespace Steinberg;
using namespace Steinberg::Vst;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IMPLEMENT_FUNKNOWN_METHODS (HostMessage, IMessage, IMessage::iid)
//-----------------------------------------------------------------------------
HostMessage::HostMessage () {FUNKNOWN_CTOR}

//-----------------------------------------------------------------------------
HostMessage::~HostMessage () noexcept
{
	setMessageID (nullptr);
	FUNKNOWN_DTOR
}

//-----------------------------------------------------------------------------
const char* PLUGIN_API HostMessage::getMessageID ()
{
	return messageId;
}

//-----------------------------------------------------------------------------
void PLUGIN_API HostMessage::setMessageID (const char* mid)
{
	if (messageId)
		delete[] messageId;
	messageId = nullptr;
	if (mid)
	{
		size_t len = strlen (mid) + 1;
		messageId = new char[len];
		strcpy (messageId, mid);
	}
}

//-----------------------------------------------------------------------------
IAttributeList* PLUGIN_API HostMessage::getAttributes ()
{
	if (!attributeList)
		attributeList = AttributeList::make ();
	return attributeList;
}

} // Host
} // Vst3
} // Effects
} // Audio
} // Gj
