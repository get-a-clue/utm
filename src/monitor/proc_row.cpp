#include "stdafx.h"
#include "proc_row.h"

namespace utm {

const char proc_row::this_class_name[] = "proc_row";

proc_row::proc_row()
{
}


proc_row::~proc_row()
{
}

bool proc_row::equals(const ubase* rhs) const
{
	return proc_row_base::equals(rhs);
}

ubase* proc_row::xml_catch_subnode(const char *tag_name, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(tag_name, PROCROW_XMLTAG_PROCROW) == 0)
	{
		u = new proc_row();
	}

	return u;
}

}