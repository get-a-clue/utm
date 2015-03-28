#include "stdafx.h"
#include "rulelist.h"

namespace utm {

rulelist::rulelist()
{
}


rulelist::~rulelist()
{
}

bool rulelist::operator==(const rulelist& rhs) const
{
	return items == rhs.items;
}

void rulelist::clear()
{
	items.clear();
}

void rulelist::xml_create()
{
	xml_append_root("RuleList");
	xml_append_node(RULE_XMLTAG_RULE, items);
}

ubase* rulelist::xml_catch_subnode(const char *keyname)
{
	ubase *u = NULL;

	if (strcmp(keyname, RULE_XMLTAG_RULE) == 0)
	{
		rule* r = insert_and_get_inserted();
		u = (ubase *)r;
	}

	return u;
}

}