#include "stdafx.h"
#include "conditem.h"

#include "conditem_addrip.h"
#include "conditem_addrurl.h"
#include "conditem_sitecategory.h"

#include <utime.h>

namespace utm {

const char conditem::this_class_name[] = "conditem";
size_t conditem::instances_created = 0;

const char* conditem::condtypes_str[MAX_CONDTYPES] = { 
	"", 
	"srcip", 
	"sitecategory", 
	"urlpath", 
	"urlhost", 
	"urlhostpath" 
};

const int conditem::condtypes_int[MAX_CONDTYPES] = { 
	(int)condtype_none, 
	(int)condtype_srcip, 
	(int)condtype_sitecategory, 
	(int)condtype_urlpath, 
	(int)condtype_urlhost, 
	(int)condtype_urlhostpath 
};

conditem::conditem(void)
{
	instances_created++;
	clear();
}

conditem::~conditem(void)
{
}

void conditem::copy_my_properties(const conditem& rhs)
{
	condtype = rhs.condtype;
	condname = rhs.condname;
	condvalue = rhs.condvalue;
}

void conditem::copy_properties(const conditem& rhs)
{
	copy_my_properties(rhs);
}

bool conditem::operator ==(const conditem& rhs) const
{
	if (get_conditem_type() != rhs.get_conditem_type())
		return false;

	if (condname != rhs.condname)
		return false;

	if (condvalue != rhs.condvalue)
		return false;

	return true;
}

int conditem::get_condclass_by_condtype(condtypes ctype)
{
	if (ctype == conditem::condtype_srcip) 
	{
		return conditem::condclass_addrip;
	}

	if (ctype == conditem::condtype_urlpath ||
		ctype == conditem::condtype_urlhost ||
		ctype == conditem::condtype_urlhostpath) 
	{
		return conditem::condclass_addrurl;
	}

	if (ctype == conditem::condtype_sitecategory)
	{
		return conditem::condclass_sitecategory;
	}

	return conditem::condclass_none;
}

conditem* conditem::create(const conditem& preconfigured_conditem)
{
	condtypes ctype = preconfigured_conditem.get_conditem_type();
	conditem* ci = create(ctype);

	if (ci != NULL)
	{
		const char *classname_new = ci->get_this_class_name();
		const char *classname_pre = preconfigured_conditem.get_this_class_name();

		if (classname_new != classname_pre)
			ci->copy_my_properties(preconfigured_conditem);
		else
			ci->copy_properties(preconfigured_conditem);
	}

	return ci;
}

conditem* conditem::create(condtypes ctype)
{
	conditem *ci = NULL;

	int cclass = get_condclass_by_condtype(ctype);

	switch(cclass)
	{
		case conditem::condclass_addrip:
			ci = new conditem_addrip();
			break;

		case conditem::condclass_addrurl:
			ci = new conditem_addrurl(conditem_addrurl::get_urlpart_from_conditem_type(ctype), NULL);
			break;

		case conditem::condclass_sitecategory:
			ci = new conditem_sitecategory();
			break;
	}

	return ci;
}

void conditem::clear()
{
	condtype = condtype_none;
	condname.clear();
	condvalue.clear();
}
	
void conditem::xml_create()
{
	condtypes ctype = get_conditem_type();
	if (ctype == condtype_none)
		return;

	xml_append_root(CONDITEM_XMLTAG);
	xml_append_node(CONDITEM_XMLTAG_TYPE, condtypes_enum_to_str((int)ctype));
	xml_append_node(CONDITEM_XMLTAG_NAME, condname);
	xml_append_node(CONDITEM_XMLTAG_VALUE, condvalue);
}
	
void conditem::xml_catch_value(const char *keyname, const char *keyvalue)
{
	std::string tmp;

	while (1)
	{
		if (xml_check_value(keyname, CONDITEM_XMLTAG_TYPE, keyvalue, tmp))
		{
			condtype = (condtypes)condtypes_enum_from_str_ci(tmp.c_str(), (int)condtype_none);
			break;
		};
		if (xml_check_value(keyname, CONDITEM_XMLTAG_NAME, keyvalue, condname)) break;
		if (xml_check_value(keyname, CONDITEM_XMLTAG_VALUE, keyvalue, condvalue)) break;
		break;
	}
}

bool conditem::do_check(proxyrequest_params *params)
{
	return false;
};

void conditem::create_description(std::string& descr) const
{
	int condtype = (int)get_conditem_type();
	descr.append(conditem::condtypes_enum_to_str(condtype));

	descr.append("(");
	descr.append(condvalue);
	descr.append(")");
}

void conditem::generate_condname()
{
	int ctype = get_conditem_type();
	std::string cname(condtypes_enum_to_str(ctype));

	utime u(true);

	std::ostringstream oss;
	oss << cname << "_" << u.to_string(utime::format_iso) << "_" << instances_created;
	condname = oss.str();
}


}
