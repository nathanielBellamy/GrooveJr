//
// Created by ns on 12/20/24.
//

#include "AttributeList.h"

namespace Gj {
namespace Effects {
namespace Vst3 {
namespace Host {

using namespace Steinberg::Vst;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct AttributeList::Attribute
{
	enum class Type
	{
		kUninitialized,
		kInteger,
		kFloat,
		kString,
		kBinary
	};
	Attribute () = default;

	Attribute (int64 value) : type (Type::kInteger) { v.intValue = value; }
	Attribute (double value) : type (Type::kFloat) { v.floatValue = value; }
	/* size is in code unit (count of TChar) */
	Attribute (const TChar* value, uint32 sizeInCodeUnit)
	: size (sizeInCodeUnit), type (Type::kString)
	{
		v.stringValue = new TChar[sizeInCodeUnit];
		memcpy (v.stringValue, value, sizeInCodeUnit * sizeof (TChar));
	}
	Attribute (const void* value, uint32 sizeInBytes) : size (sizeInBytes), type (Type::kBinary)
	{
		v.binaryValue = new char[sizeInBytes];
		memcpy (v.binaryValue, value, sizeInBytes);
	}
	Attribute (Attribute&& o) { *this = std::move (o); }
	Attribute& operator= (Attribute&& o)
	{
		v = o.v;
		size = o.size;
		type = o.type;
		o.size = 0;
		o.type = Type::kUninitialized;
		o.v = {};
		return *this;
	}
	~Attribute () noexcept
	{
		if (size)
			delete[] v.binaryValue;
	}

	int64 intValue () const { return v.intValue; }
	double floatValue () const { return v.floatValue; }
	/* sizeInCodeUnit is in code unit (count of TChar) */
	const TChar* stringValue (uint32& sizeInCodeUnit)
	{
		sizeInCodeUnit = size;
		return v.stringValue;
	}
	const void* binaryValue (uint32& sizeInBytes)
	{
		sizeInBytes = size;
		return v.binaryValue;
	}

	Type getType () const { return type; }

private:
	union v
	{
		int64 intValue;
		double floatValue;
		TChar* stringValue;
		char* binaryValue;
	} v;
	uint32 size {0};
	Type type {Type::kUninitialized};
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IMPLEMENT_FUNKNOWN_METHODS (AttributeList, IAttributeList, IAttributeList::iid)

//-----------------------------------------------------------------------------
IPtr<IAttributeList> AttributeList::make ()
{
	return owned (new AttributeList);
}

//-----------------------------------------------------------------------------
AttributeList::AttributeList () {FUNKNOWN_CTOR}

//-----------------------------------------------------------------------------
AttributeList::~AttributeList () noexcept
{
	FUNKNOWN_DTOR
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API AttributeList::setInt (AttrID aid, int64 value)
{
	if (!aid)
		return kInvalidArgument;
	list[aid] = Attribute (value);
	return kResultTrue;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API AttributeList::getInt (AttrID aid, int64& value)
{
	if (!aid)
		return kInvalidArgument;
	auto it = list.find (aid);
	if (it != list.end () && it->second.getType () == Attribute::Type::kInteger)
	{
		value = it->second.intValue ();
		return kResultTrue;
	}
	return kResultFalse;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API AttributeList::setFloat (AttrID aid, double value)
{
	if (!aid)
		return kInvalidArgument;
	list[aid] = Attribute (value);
	return kResultTrue;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API AttributeList::getFloat (AttrID aid, double& value)
{
	if (!aid)
		return kInvalidArgument;
	auto it = list.find (aid);
	if (it != list.end () && it->second.getType () == Attribute::Type::kFloat)
	{
		value = it->second.floatValue ();
		return kResultTrue;
	}
	return kResultFalse;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API AttributeList::setString (AttrID aid, const TChar* string)
{
	if (!aid)
		return kInvalidArgument;
	// + 1 for the null-terminate
	auto length = tstrlen (string) + 1;
	list[aid] = Attribute (string, length);
	return kResultTrue;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API AttributeList::getString (AttrID aid, TChar* string, uint32 sizeInBytes)
{
	if (!aid)
		return kInvalidArgument;
	auto it = list.find (aid);
	if (it != list.end () && it->second.getType () == Attribute::Type::kString)
	{
		uint32 sizeInCodeUnit = 0;
		const TChar* _string = it->second.stringValue (sizeInCodeUnit);
		memcpy (string, _string, std::min<uint32> (sizeInCodeUnit * sizeof (TChar), sizeInBytes));
		return kResultTrue;
	}
	return kResultFalse;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API AttributeList::setBinary (AttrID aid, const void* data, uint32 sizeInBytes)
{
	if (!aid)
		return kInvalidArgument;
	list[aid] = Attribute (data, sizeInBytes);
	return kResultTrue;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API AttributeList::getBinary (AttrID aid, const void*& data, uint32& sizeInBytes)
{
	if (!aid)
		return kInvalidArgument;
	auto it = list.find (aid);
	if (it != list.end () && it->second.getType () == Attribute::Type::kBinary)
	{
		data = it->second.binaryValue (sizeInBytes);
		return kResultTrue;
	}
	sizeInBytes = 0;
	return kResultFalse;
}

} // Host
} // Vst3
} // Effects
} // Gj