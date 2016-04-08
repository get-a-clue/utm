#pragma once

#if defined(_MSC_VER)
#define UTM_WIN
#endif

#ifdef UTM_WIN

#include <Windows.h>
#include <tchar.h>

#endif

#include <string>

#include <ubase.h>
#include "conditem.h"

#define PROXYRULEITEM_XMLTAG "Item"
#define PROXYRULEITEM_XMLTAG_INVERT "Invert"
#define PROXYRULEITEM_XMLTAG_CONDITION "Condition"

namespace utm {

class proxyrule_item : public ubase
{
public:
	static const char this_class_name[];

	proxyrule_item(void);
	proxyrule_item(const proxyrule_item& rhs);
	proxyrule_item(const char* _condname, bool _invert, const std::shared_ptr<conditem>& _ci);
	~proxyrule_item(void);

	const char *get_this_class_name() const { return this_class_name; };

	void copy_properties(const proxyrule_item& rhs);
	proxyrule_item& operator=(const proxyrule_item& rhs);
	bool equals(const ubase* rhs) const;
	bool operator ==(const proxyrule_item& rhs) const;

	bool invert;
	std::string condname;
	std::shared_ptr<conditem> ci;

	void create_description(std::string& descr) const;

	void clear();
	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue);
	ubase* xml_catch_subnode(const char *tag_name, const char *classname) { return NULL; };

	bool check(proxyrequest_params *params) const;
};

}