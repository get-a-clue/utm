#include "stdafx.h"
#include "filterstatelist.h"

namespace utm {

filterstatelist::filterstatelist(void)
{
}

filterstatelist::~filterstatelist(void)
{
}

bool filterstatelist::operator==(const filterstatelist& rhs) const
{
	return items == rhs.items;
}

void filterstatelist::xml_create()
{
    xml_append_root("TrafficCountersFakeRootNode");
    xml_append_node(FILTERSTATE_XMLTAG_FILTER, items);
}

ubase* filterstatelist::xml_catch_subnode(const char *keyname)
{
	ubase *u = NULL;

	if (strcmp(keyname, FILTERSTATE_XMLTAG_FILTER) == 0)
	{
		filterstate* r = insert_and_get_inserted();
		u = (ubase *)r;
	}

	return u;
}

}