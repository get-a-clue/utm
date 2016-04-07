#include "stdafx.h"
#include "dhcpleaselist.h"

namespace utm {

dhcpleaselist::dhcpleaselist(void)
{
}

dhcpleaselist::~dhcpleaselist(void)
{
}

void dhcpleaselist::clear()
{
	items.clear();
}

void dhcpleaselist::xml_create()
{
    xml_append_root("DhcpLeaseList");
    xml_append_node(SINGLEDHCPLEASE_XMLTAG_ROOT, items);
}

ubase* dhcpleaselist::xml_catch_subnode(const char *keyname, const char* classname)
{
	ubase *u = NULL;

	if (strcmp(keyname, SINGLEDHCPLEASE_XMLTAG_ROOT) == 0)
	{
		u = dynamic_cast<ubase *>(init_and_get_temp_item(new singledhcplease()));
	}

	return u;
}

void dhcpleaselist::xml_catch_subnode_finished(const char *keyname)
{
	if (strcmp(keyname, SINGLEDHCPLEASE_XMLTAG_ROOT) == 0)
	{
		commit_temp_item();
	}
}

}