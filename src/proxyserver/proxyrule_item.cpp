#include "stdafx.h"
#include "proxyrule_item.h"

namespace utm {

const char proxyrule_item::this_class_name[] = "proxyrule_item";

proxyrule_item::proxyrule_item(void)
{
	clear();
}

proxyrule_item::proxyrule_item(const proxyrule_item& rhs)
{
	operator=(rhs);
}

proxyrule_item::~proxyrule_item(void)
{
	ci.reset();
}

proxyrule_item::proxyrule_item(const char* _condname, bool _invert, const std::shared_ptr<conditem>& _ci)
{
	invert = _invert;
	condname = _condname;
	ci = _ci;
}

void proxyrule_item::copy_properties(const proxyrule_item& rhs)
{
	invert = rhs.invert;
	condname = rhs.condname;
}

proxyrule_item& proxyrule_item::operator=(const proxyrule_item& rhs)
{
	copy_properties(rhs);
	ci = rhs.ci;
/*
	ci.reset();
	if (rhs.ci.get() != NULL)
	{
		conditem* cdi = new conditem(*rhs.ci.get());
		ci = std::shared_ptr<conditem>(cdi);
	}
*/
	return *this;
}

bool proxyrule_item::operator ==(const proxyrule_item& rhs) const
{
	if (invert != rhs.invert)
		return false;

	if ((ci.get() == NULL) && (rhs.ci.get() == NULL))
		return true;

	if ((ci.get() == NULL) || (rhs.ci.get() == NULL))
		return false;

	if (!(*ci == *rhs.ci))
		return false;

	return true;
}

void proxyrule_item::clear()
{
	invert = false;
	condname.clear();
	ci.reset();
}

void proxyrule_item::xml_create()
{
	xml_append_root(PROXYRULEITEM_XMLTAG);
	xml_append_node(PROXYRULEITEM_XMLTAG_INVERT, invert);
	xml_append_node(PROXYRULEITEM_XMLTAG_CONDITION, ci->condname);

//	ci->xml_create();
//	xml_append_child(ci->get_first_child());
}

void proxyrule_item::xml_catch_value(const char *keyname, const char *keyvalue)
{
	while (1)
	{
		if (xml_check_value(keyname, PROXYRULEITEM_XMLTAG_INVERT, keyvalue, invert)) break;
		if (xml_check_value(keyname, PROXYRULEITEM_XMLTAG_CONDITION, keyvalue, condname)) break;
		break;
	}
}

bool proxyrule_item::check(proxyrequest_params *params) const
{
	bool retval = false;
	if (ci.get() != NULL)
	{
		retval = ci->do_check(params);
		if (invert)
			retval = !retval;
	}

	return retval;
}

void proxyrule_item::create_description(std::string& descr) const
{
	if (invert)
		descr.append("!");

	ci->create_description(descr);
}

}