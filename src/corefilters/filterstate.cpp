#include "stdafx.h"
#include "filterstate.h"

namespace utm {

const char filterstate::this_class_name[] = "filterstate";

filterstate::filterstate(void)
{
}


filterstate::~filterstate(void)
{
}

void filterstate::xml_catch_subnode_attribute(const char *attrname, const char* attrvalue)
{
	if ((attrname == NULL) || (attrvalue == NULL))
		return;

	if (_strnicmp(attrname, "Id", 2) == 0)
	{
		filter_id = boost::lexical_cast<unsigned int>(attrvalue);
	}
}

}
