#include "stdafx.h"
#include "filterlist.h"

#include "filter_base.h"

namespace utm {

filterlist::filterlist(void)
{
}


filterlist::~filterlist(void)
{
}

void filterlist::xml_create()
{
    xml_append_root("FilterList");
    xml_append_node(FILTER_XMLTAG_FILTER, items);
}

ubase* filterlist::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, FILTER_XMLTAG_FILTER) == 0)
	{
		u = dynamic_cast<ubase *>(new filter2());
		add_element(u);
	}

	return u;
}

}
