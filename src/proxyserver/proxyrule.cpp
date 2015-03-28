#include "stdafx.h"
#include "proxyrule.h"

#include <boost/algorithm/string/replace.hpp>

namespace utm {

const char proxyrule::this_class_name[] = "proxyrule";

const char* proxyrule::actions_str[] = { 
	PROXYRULE_NAME_ACTION_NONE, 
	PROXYRULE_NAME_ACTION_DENY, 
	PROXYRULE_NAME_ACTION_ALLOW
};

const int proxyrule::actions_int[] = {
	(int)proxyrule::action_none,
	(int)proxyrule::action_deny, 
	(int)proxyrule::action_allow
};

proxyrule::proxyrule(void)
{
	clear();
}

proxyrule::~proxyrule(void)
{
}

void proxyrule::copy_properties(const proxyrule& rhs)
{
	enabled = rhs.enabled;
	nolog_access = rhs.nolog_access;
	rulename = rhs.rulename;
	action = rhs.action;
	prepare_rulename();
}

bool proxyrule::operator ==(const proxyrule& rhs) const
{
	if (enabled != rhs.enabled)
		return false;

	if (nolog_access != rhs.nolog_access)
		return false;

	if (rulename != rhs.rulename)
		return false;

	if (action != rhs.action)
		return false;

	if (items.size() != rhs.items.size())
		return false;

	proxyrule_items_container::const_iterator iter;
	proxyrule_items_container::const_iterator iter2 = rhs.items.begin();
	for (iter = items.begin(); iter != items.end(); ++iter, ++iter2)
	{
		if (!((*iter->ci.get()) == (*iter2->ci.get())))
			return false;
	}

	return true;
}

void proxyrule::clear()
{
	enabled = true;
	nolog_access = false;
	rulename.clear();
	action = action_none;
	items.clear();
}

void proxyrule::xml_create()
{
	xml_append_root(PROXYRULE_XMLTAG);
	xml_append_node(PROXYRULE_XMLTAG_ENABLED, enabled);
	xml_append_node(PROXYRULE_XMLTAG_NOLOG_ACCESS, nolog_access);
	xml_append_node(PROXYRULE_XMLTAG_NAME, rulename);
	xml_append_node(PROXYRULE_XMLTAG_ACTION, (int)action);

	proxyrule_items_iterator iter;
	for (iter = items.begin(); iter != items.end(); ++iter)
	{
		iter->xml_create();
		xml_append_child(iter->get_first_child());
	}
}

void proxyrule::xml_catch_value(const char *keyname, const char *keyvalue)
{
	int tmp;

	while (1)
	{
		if (xml_check_value(keyname, PROXYRULE_XMLTAG_ENABLED, keyvalue, enabled)) break;
		if (xml_check_value(keyname, PROXYRULE_XMLTAG_NOLOG_ACCESS, keyvalue, nolog_access)) break;
		if (xml_check_value(keyname, PROXYRULE_XMLTAG_NAME, keyvalue, rulename)) break;
		if (xml_check_value(keyname, PROXYRULE_XMLTAG_ACTION, keyvalue, tmp))
		{
			action = (actions)tmp;
			break;		
		};

		break;
	}
}

ubase* proxyrule::xml_catch_subnode(const char *keyname)
{
	if (strcmp(keyname, PROXYRULEITEM_XMLTAG) == 0)
	{
		return insert_element_and_get_lastptr(items);
	}

	return NULL;
}

int proxyrule::process_request(proxyrequest_params *params) const
{
	if (enabled)
	{
		proxyrule_items_container::const_iterator iter = items.begin();

		if (iter != items.end())
		{
			bool match = true;

			for (; iter != items.end(); ++iter)
			{
				if (!iter->check(params))
				{
					match = false;
					break;
				}
			}

			if (match)
				return (int)action;
		}
	}

	return (int)proxyrule::action_none;
}

void proxyrule::create_description(std::string& descr) const
{
	proxyrule_items_container::const_iterator iter;
	for (iter = items.begin(); iter != items.end(); ++iter)
	{
		if (!descr.empty())
			descr.append(" & ");

		std::string itemdescr;
		iter->create_description(itemdescr);

		descr.append(itemdescr);
	}
}

bool proxyrule::item_get(size_t index, proxyrule_item& item)
{
	size_t k = 0;

	proxyrule_items_iterator iter;
	for (iter = items.begin(); iter != items.end(); ++iter)
	{
		if (index == k)
		{
			item = *iter;
			return true;
		}
		k++;
	}

	return false;
}

void proxyrule::item_add(const proxyrule_item& item)
{
	items.push_back(item);
}

void proxyrule::item_replace(const proxyrule_item& item, size_t index)
{
	size_t k = 0;

	proxyrule_items_iterator iter;
	for (iter = items.begin(); iter != items.end(); ++iter)
	{
		if (index == k)
		{
			*iter = item;
			break;
		}
		k++;
	}
}

void proxyrule::item_delete(size_t index)
{
	size_t k = 0;

	proxyrule_items_iterator iter;
	for (iter = items.begin(); iter != items.end(); ++iter)
	{
		if (index == k)
		{
			items.erase(iter);
			break;
		}
		k++;
	}
}

const char* proxyrule::get_rulename4log() const
{
	return rulename4log.c_str();
}

const char* proxyrule::get_rulename4web() const
{
	return rulename4web.c_str();
}

void proxyrule::prepare_rulename()
{
	rulename4log = rulename.getmb();
	boost::algorithm::replace_all(rulename4log, " ", "_");

	rulename4web = rulename.getutf8();
}

}