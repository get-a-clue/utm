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

bool filterlist::operator==(const filterlist& rhs) const
{
	return items == rhs.items;
}

void filterlist::xml_create()
{
    xml_append_root("FilterList");
    xml_append_node(FILTER_XMLTAG_FILTER, items);
}

ubase* filterlist::xml_catch_subnode(const char *keyname)
{
	ubase *u = NULL;

	if (strcmp(keyname, FILTER_XMLTAG_FILTER) == 0)
	{
		filter2* r = insert_and_get_inserted();
		u = (ubase *)r;
	}

	return u;
}

}
