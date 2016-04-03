#include "StdAfx.h"
#include "monitor_range_list.h"

namespace utm {

monitor_range_list::monitor_range_list(void)
{
}


monitor_range_list::~monitor_range_list(void)
{
}

void monitor_range_list::clear()
{
	items.clear();
}

void monitor_range_list::xml_create()
{
    xml_append_root("RangeList");
    xml_append_node(MONRANGE_XMLTAG_ROOT, items);
}

ubase* monitor_range_list::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, MONRANGE_XMLTAG_ROOT) == 0)
	{
		u = dynamic_cast<monitor_range *>(new monitor_range());
		add_element(u);
	}

	return u;
}

}