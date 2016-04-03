#include "stdafx.h"
#include "rulelist.h"

namespace utm {

rulelist::rulelist()
{
}


rulelist::~rulelist()
{
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

ubase* rulelist::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, RULE_XMLTAG_RULE) == 0)
	{
		u = dynamic_cast<rule *>(new rule());
		add_element(u);
	}

	return u;
}

}