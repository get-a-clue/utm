#include "stdafx.h"
#include "trafficreport_dayticklist.h"

namespace utm {

trafficreport_dayticklist::trafficreport_dayticklist()
{
}


trafficreport_dayticklist::~trafficreport_dayticklist()
{
}

void trafficreport_dayticklist::xml_create()
{
	xml_append_root("TrafficReportDayTickFakeRootNode");
	xml_append_node(TR_DAYTICK_XMLTAG_ROOT, items);
}

ubase* trafficreport_dayticklist::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, TR_DAYTICK_XMLTAG_ROOT) == 0)
	{
		u = dynamic_cast<trafficreport_daytick *>(new trafficreport_daytick());
		add_element(u);
	}

	return u;
}

}