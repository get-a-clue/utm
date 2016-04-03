#include "StdAfx.h"
#include "monitor_detail_list.h"

namespace utm {

monitor_detail_list::monitor_detail_list(void)
{
}


monitor_detail_list::~monitor_detail_list(void)
{
}

void monitor_detail_list::xml_create()
{
    xml_append_root("EntryList");
    xml_append_node(MONDETREC_XMLTAG_ROOT, items);
}

ubase* monitor_detail_list::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, MONDETREC_XMLTAG_ROOT) == 0)
	{
		u = dynamic_cast<monitor_detail_record *>(new monitor_detail_record());
		add_element(u);
	}

	return u;
}

}