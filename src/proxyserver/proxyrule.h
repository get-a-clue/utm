#pragma once

#if defined(_MSC_VER)
#define UTM_WIN
#endif

#ifdef UTM_WIN

#include <Windows.h>
#include <tchar.h>

#endif

#include <ubase.h>
#include <gstring.h>

#include "conditem.h"
#include "proxyrule_item.h"

#include <vector>
#include <list>

#define PROXYRULE_XMLTAG "Proxyrule"
#define PROXYRULE_XMLTAG_ENABLED "Enabled"
#define PROXYRULE_XMLTAG_NOLOG_ACCESS "NologAccess"
#define PROXYRULE_XMLTAG_NAME "Name"
#define PROXYRULE_XMLTAG_ACTION "Action"

#define PROXYRULE_NAME_ACTION_MAXITEMS 3
#define PROXYRULE_NAME_ACTION_NONE ""
#define PROXYRULE_NAME_ACTION_DENY "deny"
#define PROXYRULE_NAME_ACTION_ALLOW "allow"

namespace utm {

typedef std::list<proxyrule_item> proxyrule_items_container;
typedef std::list<proxyrule_item>::iterator proxyrule_items_iterator;

class proxyrule : public ubase
{
public:
	static const char this_class_name[];

	enum actions { action_none = -1, action_deny = 0, action_allow = 1 };
	static const char* actions_str[];
	static const int actions_int[];

	static const char* actions_enum_to_str(int cat)
	{
		return enumhelper::to_str(cat, &proxyrule::actions_str[0], &proxyrule::actions_int[0], PROXYRULE_NAME_ACTION_MAXITEMS);
	}

	static int actions_enum_from_str_ci(const char* str, int default_item = 0)
	{
		return enumhelper::from_str_ci(str, &proxyrule::actions_str[0], &proxyrule::actions_int[0], PROXYRULE_NAME_ACTION_MAXITEMS, default_item);
	}

	proxyrule(void);
	~proxyrule(void);

	void copy_properties(const proxyrule& rhs);
	bool operator ==(const proxyrule& rhs) const;

	bool enabled;
	bool nolog_access;
	gstring rulename;
	actions action;
	proxyrule_items_container items;

	bool item_get(size_t index, proxyrule_item& item);
	void item_add(const proxyrule_item& item);
	void item_replace(const proxyrule_item& item, size_t index);
	void item_delete(size_t index);

	void clear();
	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue);
	ubase* xml_catch_subnode(const char *name);

	int process_request(proxyrequest_params *params) const;
	void create_description(std::string& descr) const;

	const char* get_rulename4log() const;
	const char* get_rulename4web() const;
	void prepare_rulename();

private:
	std::string rulename4log;
	std::string rulename4web;
};

}
