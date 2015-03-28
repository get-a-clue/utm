#include "stdafx.h"
#include "dhcpleaselist.h"

namespace utm {

dhcpleaselist::dhcpleaselist(void)
{
}

dhcpleaselist::~dhcpleaselist(void)
{
}

bool dhcpleaselist::operator==(const dhcpleaselist& rhs) const
{
	return items == rhs.items;
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

ubase* dhcpleaselist::xml_catch_subnode(const char *keyname)
{
	ubase *u = NULL;

	if (strcmp(keyname, SINGLEDHCPLEASE_XMLTAG_ROOT) == 0)
	{
		u = (ubase *)get_temp_item();
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