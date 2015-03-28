#include "stdafx.h"
#include "trafficreport_dayticklist.h"

namespace utm {

trafficreport_dayticklist::trafficreport_dayticklist()
{
}


trafficreport_dayticklist::~trafficreport_dayticklist()
{
}

bool trafficreport_dayticklist::operator==(const trafficreport_dayticklist& rhs) const
{
	return items == rhs.items;
}

void trafficreport_dayticklist::xml_create()
{
	xml_append_root("TrafficReportDayTickFakeRootNode");
	xml_append_node(TR_DAYTICK_XMLTAG_ROOT, items);
}

ubase* trafficreport_dayticklist::xml_catch_subnode(const char *keyname)
{
	ubase *u = NULL;

	if (strcmp(keyname, TR_DAYTICK_XMLTAG_ROOT) == 0)
	{
		trafficreport_daytick* r = insert_and_get_inserted();
		u = (ubase *)r;
	}

	return u;
}

}