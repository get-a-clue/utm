#include "stdafx.h"
#include "procnicknamelist.h"

namespace utm {

procnicknamelist::procnicknamelist(void)
{
}

procnicknamelist::~procnicknamelist(void)
{
}

bool procnicknamelist::operator==(const procnicknamelist& rhs) const
{
	return items == rhs.items;
}

bool procnicknamelist::find_by_pathexe(const gstring& _pathexe, procnickname& result) const
{
	bool retval = false;
	ubaselist_container::const_iterator iter;

	for (iter = items.begin(); iter != items.end(); ++iter)
	{
		if ((*iter).pathexe.is_ci_equal(_pathexe))
		{
			result = *iter;
			retval = true;
			break;
		};
	};

	return retval;
}


void procnicknamelist::clear()
{
	items.clear();

	procnickname p1;
	p1.set_id(2);
	p1.nickname.assign("SYSTEM");
	items.push_back(p1);

	procnickname p2;
	p2.set_id(3);
	p2.nickname.assign("SVCHOST");
	items.push_back(p2);
}

void procnicknamelist::xml_create()
{
    xml_append_root("ProcNickNameList");
    xml_append_node(PROCNICKNAME_XMLTAG_ROOT, items);
}

ubase* procnicknamelist::xml_catch_subnode(const char *keyname)
{
	ubase *u = NULL;

	if (strcmp(keyname, PROCNICKNAME_XMLTAG_ROOT) == 0)
	{
		u = (ubase *)get_temp_item();
	}

	return u;
}

void procnicknamelist::xml_catch_subnode_finished(const char *keyname)
{
	if (strcmp(keyname, PROCNICKNAME_XMLTAG_ROOT) == 0)
	{
		commit_temp_item();
	}
}

}