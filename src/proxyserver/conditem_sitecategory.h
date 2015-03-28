#pragma once

#if defined(_MSC_VER)
#define UTM_WIN
#endif

#ifdef UTM_WIN

#include <Windows.h>
#include <tchar.h>

#endif

#include "conditem.h"

#include <enumhelper.h>

#define GOOGLE_ERROR 0
#define GOOGLE_SEX 1
#define GOOGLE_SUSPICIOUS 2
#define GOOGLE_OK 3

#define CONDITEM_NAME_SITECATEGORY "sitecategory"

#define CONDITEM_NAME_SITECATEGORY_MAXITEMS 7
#define CONDITEM_NAME_SITECATEGORY_NONE ""
#define CONDITEM_NAME_SITECATEGORY_ALL "all"
#define CONDITEM_NAME_SITECATEGORY_IPADDR "ipaddr"
#define CONDITEM_NAME_SITECATEGORY_HTTPS "https"
#define CONDITEM_NAME_SITECATEGORY_GOOGLESEX "googlesex"
#define CONDITEM_NAME_SITECATEGORY_GOOGLESUSPICIOUS "googlesuspicious"
#define CONDITEM_NAME_SITECATEGORY_GOOGLEOK "googleok"

namespace utm {

class conditem_sitecategory : public conditem
{
public:
	static const char this_class_name[];
	const char* get_this_class_name() const { return conditem_sitecategory::this_class_name; };

	enum categories 
	{
		category_none = 0,
		category_all = 1, 
		category_ipaddr = 2,
		category_https = 3,
		category_googlesex = 4, 
		category_googlesuspicious = 5, 
		category_googleok = 6
	};
	static const char* categories_str[];
	static const int categories_int[];

	static const char* categories_enum_to_str(int cat)
	{
		return enumhelper::to_str(cat, &conditem_sitecategory::categories_str[0], &conditem_sitecategory::categories_int[0], CONDITEM_NAME_SITECATEGORY_MAXITEMS);
	}

	static int categories_enum_from_str_ci(const char* str, int default_item = 0)
	{
		return enumhelper::from_str_ci(str, &conditem_sitecategory::categories_str[0], &conditem_sitecategory::categories_int[0], CONDITEM_NAME_SITECATEGORY_MAXITEMS, default_item);
	}

	conditem_sitecategory(void);
	conditem_sitecategory(categories _categorytype);
	~conditem_sitecategory(void);

	void copy_properties(const conditem& rhs);
	void set_conditem_sitecategory(categories _categorytype);
	
	void clear();
	bool parse(const std::string& str);
	std::string to_string() const;

	condtypes get_conditem_type() const	{ return condtype_sitecategory; };
	bool do_check(proxyrequest_params *params);

	categories categorytype;
private:

};

}