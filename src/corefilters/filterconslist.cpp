#include "stdafx.h"
#include "filterconslist.h"

namespace utm {

filterconslist::filterconslist()
{
}


filterconslist::~filterconslist()
{
}

void filterconslist::xml_create()
{
	xml_append_root("TrafficConsFakeRootNode");
	xml_append_node(FILTERCONS_XMLTAG_FILTERCONS, items);
}

ubase* filterconslist::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, FILTERCONS_XMLTAG_FILTERCONS) == 0)
	{
		u = dynamic_cast<ubase *>(new filtercons());
		add_element(u);
	}

	return u;
}

}