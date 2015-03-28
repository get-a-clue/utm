#include "stdafx.h"
#include "conditem_sitecategory.h"

namespace utm {

const char conditem_sitecategory::this_class_name[] = "conditem_sitecategory";

const char* conditem_sitecategory::categories_str[] = { 
	CONDITEM_NAME_SITECATEGORY_NONE, 
	CONDITEM_NAME_SITECATEGORY_ALL, 
	CONDITEM_NAME_SITECATEGORY_IPADDR,
	CONDITEM_NAME_SITECATEGORY_HTTPS,
	CONDITEM_NAME_SITECATEGORY_GOOGLESEX, 
	CONDITEM_NAME_SITECATEGORY_GOOGLESUSPICIOUS, 
	CONDITEM_NAME_SITECATEGORY_GOOGLEOK };

const int conditem_sitecategory::categories_int[] = {
	(int)conditem_sitecategory::category_none,
	(int)conditem_sitecategory::category_all, 
	(int)conditem_sitecategory::category_ipaddr,
	(int)conditem_sitecategory::category_https,
	(int)conditem_sitecategory::category_googlesex, 
	(int)conditem_sitecategory::category_googlesuspicious, 
	(int)conditem_sitecategory::category_googleok
};


conditem_sitecategory::conditem_sitecategory(void)
{
	clear();
}

conditem_sitecategory::conditem_sitecategory(categories _categorytype)
{
	set_conditem_sitecategory(_categorytype);
}

conditem_sitecategory::~conditem_sitecategory(void)
{
}

void conditem_sitecategory::copy_properties(const conditem& rhs)
{
	conditem_sitecategory *p = (conditem_sitecategory *)&rhs;

	categorytype = p->categorytype;

	conditem::copy_properties(rhs);
}

void conditem_sitecategory::clear()
{
	conditem::clear();
	categorytype = category_none;
}

void conditem_sitecategory::set_conditem_sitecategory(categories _categorytype)
{
	categorytype = _categorytype;
	condvalue.assign(categories_enum_to_str((int)categorytype));
}

bool conditem_sitecategory::parse(const std::string& str)
{
	int retval = categories_enum_from_str_ci(str.c_str(), (int)category_none);
	categorytype = (categories)retval;

	if (categorytype == category_none)
		return false;

	return true;
}

std::string conditem_sitecategory::to_string() const
{
	std::string str(categories_enum_to_str(categorytype));
	return str;
}

bool conditem_sitecategory::do_check(proxyrequest_params *params)
{
	if (categorytype == category_all)
		return true;

	if (categorytype == category_ipaddr)
		return params->request->url.is_host_ipaddr();

	if (categorytype == category_https)
	{
		size_t https_pos = stringtools::ci_find_substr(params->request->url.protocol, std::string("https"));
		return (https_pos != std::string::npos);
	}

	if (categorytype == category_googlesex ||
		categorytype == category_googlesuspicious ||
		categorytype == category_googleok)
	{
		if (params->request->url.is_host_ipaddr())
			return false;

		if (params->whatsite == NULL)
			return true;

		int code = params->whatsite(params->request);

		if (categorytype == category_googlesex)
		{
			return (code == GOOGLE_SEX);
		}

		if (categorytype == category_googlesuspicious)
		{
			return (code == GOOGLE_SEX || code == GOOGLE_SUSPICIOUS);
		}

		if (categorytype == category_googleok)
		{
			return (code == GOOGLE_OK);
		}
	}

	return false;
}


}