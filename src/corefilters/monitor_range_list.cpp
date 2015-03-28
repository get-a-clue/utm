#include "StdAfx.h"
#include "monitor_range_list.h"

namespace utm {

monitor_range_list::monitor_range_list(void)
{
}


monitor_range_list::~monitor_range_list(void)
{
}

bool monitor_range_list::operator==(const monitor_range_list& rhs) const
{
	return items == rhs.items;
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

ubase* monitor_range_list::xml_catch_subnode(const char *keyname)
{
	ubase *u = NULL;

	if (strcmp(keyname, MONRANGE_XMLTAG_ROOT) == 0)
	{
		monitor_range* r = insert_and_get_inserted();
		u = (ubase *)r;
	}

	return u;
}

}