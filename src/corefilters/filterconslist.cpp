#include "stdafx.h"
#include "filterconslist.h"

namespace utm {

filterconslist::filterconslist()
{
}


filterconslist::~filterconslist()
{
}

bool filterconslist::operator==(const filterconslist& rhs) const
{
	return items == rhs.items;
}

void filterconslist::xml_create()
{
	xml_append_root("TrafficConsFakeRootNode");
	xml_append_node(FILTERCONS_XMLTAG_FILTERCONS, items);
}

ubase* filterconslist::xml_catch_subnode(const char *keyname)
{
	ubase *u = NULL;

	if (strcmp(keyname, FILTERCONS_XMLTAG_FILTERCONS) == 0)
	{
		filtercons* r = insert_and_get_inserted();
		u = (ubase *)r;
	}

	return u;
}

}