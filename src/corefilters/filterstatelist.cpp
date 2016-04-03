#include "stdafx.h"
#include "filterstatelist.h"

namespace utm {

filterstatelist::filterstatelist(void)
{
}

filterstatelist::~filterstatelist(void)
{
}

void filterstatelist::xml_create()
{
    xml_append_root("TrafficCountersFakeRootNode");
    xml_append_node(FILTERSTATE_XMLTAG_FILTER, items);
}

ubase* filterstatelist::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, FILTERSTATE_XMLTAG_FILTER) == 0)
	{
		u = dynamic_cast<ubase *>(new filterstate());
		add_element(u);
	}

	return u;
}

}