#include "StdAfx.h"
#include "monitor_detail_list.h"

namespace utm {

monitor_detail_list::monitor_detail_list(void)
{
}


monitor_detail_list::~monitor_detail_list(void)
{
}

bool monitor_detail_list::operator==(const monitor_detail_list& rhs) const
{
	return items == rhs.items;
}

void monitor_detail_list::xml_create()
{
    xml_append_root("EntryList");
    xml_append_node(MONDETREC_XMLTAG_ROOT, items);
}

ubase* monitor_detail_list::xml_catch_subnode(const char *keyname)
{
	ubase *u = NULL;

	if (strcmp(keyname, MONDETREC_XMLTAG_ROOT) == 0)
	{
		monitor_detail_record* r = insert_and_get_inserted();
		u = (ubase *)r;
	}

	return u;
}

}