#pragma once

#if defined(_MSC_VER)
#define UTM_WIN
#endif

#ifdef UTM_WIN

#include <Windows.h>
#include <tchar.h>

#endif

#include <string>

#include <addrip_v4.h>
#include <ubase.h>
#include <enumhelper.h>

#include "proxyrequest_params.h"

#define CONDITEM_XMLTAG "Conditem"
#define CONDITEM_XMLTAG_NAME "Name"
#define CONDITEM_XMLTAG_TYPE "Type"
#define CONDITEM_XMLTAG_VALUE "Value"

#define MAX_CONDTYPES 6

namespace utm {

class conditem : public ubase
{
public:
	static size_t instances_created;
	static const char this_class_name[];
	virtual const char* get_this_class_name() const { return conditem::this_class_name; };

	enum condtypes 
	{ 
		condtype_none = 0, 
		condtype_srcip = 1, 
		condtype_sitecategory = 2, 
		condtype_urlpath = 3, 
		condtype_urlhost = 4, 
		condtype_urlhostpath = 5 
	};
	static const char* condtypes_str[MAX_CONDTYPES];
	static const int condtypes_int[MAX_CONDTYPES];

	static const char* condtypes_enum_to_str(int ctype)
	{
		return enumhelper::to_str(ctype, &conditem::condtypes_str[0], &conditem::condtypes_int[0], MAX_CONDTYPES);
	}

	static int condtypes_enum_from_str_ci(const char* str, int default_item = 0)
	{
		return enumhelper::from_str_ci(str, &conditem::condtypes_str[0], &conditem::condtypes_int[0], MAX_CONDTYPES, default_item);
	}

	enum condclasses
	{
		condclass_none = 0,
		condclass_addrip = 1,
		condclass_addrurl = 2,
		condclass_sitecategory = 3
	};
	static int get_condclass_by_condtype(condtypes ctype);

	conditem(void);
	virtual ~conditem(void);

	void copy_my_properties(const conditem& rhs);
	virtual void copy_properties(const conditem& rhs);

	bool operator ==(const conditem& rhs) const;

	static conditem* create(const conditem& preconfigured_conditem);
	static conditem* create(condtypes ctype);
	virtual bool parse(const std::string& s) { return false; };
	virtual std::string to_string() const { return std::string(); };

	virtual condtypes get_conditem_type() const { return condtype; };
	void create_description(std::string& descr) const;
	void generate_condname();
	virtual bool do_check(proxyrequest_params *params);

	condtypes condtype;
	std::string condname;
	std::string condvalue;

	// XML specific methods

	void clear();
	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue);
	ubase* xml_catch_subnode(const char *name) { return NULL; };
};

}
