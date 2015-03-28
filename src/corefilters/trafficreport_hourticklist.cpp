#include "stdafx.h"
#include "trafficreport_hourticklist.h"

namespace utm {

trafficreport_hourticklist::trafficreport_hourticklist()
{
}


trafficreport_hourticklist::~trafficreport_hourticklist()
{
}

bool trafficreport_hourticklist::operator==(const trafficreport_hourticklist& rhs) const
{
	return items == rhs.items;
}

void trafficreport_hourticklist::xml_create()
{
	xml_append_root("TrafficReportHourTickFakeRootNode");
	xml_append_node(TR_HOURTICK_XMLTAG_ROOT, items);
}

ubase* trafficreport_hourticklist::xml_catch_subnode(const char *keyname)
{
	ubase *u = NULL;

	if (strcmp(keyname, TR_HOURTICK_XMLTAG_ROOT) == 0)
	{
		trafficreport_hourtick* r = insert_and_get_inserted();
		u = (ubase *)r;
	}

	return u;
}

}