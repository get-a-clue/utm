#include "stdafx.h"
#include "trafficreport_hourticklist.h"

namespace utm {

trafficreport_hourticklist::trafficreport_hourticklist()
{
}


trafficreport_hourticklist::~trafficreport_hourticklist()
{
}

void trafficreport_hourticklist::xml_create()
{
	xml_append_root("TrafficReportHourTickFakeRootNode");
	xml_append_node(TR_HOURTICK_XMLTAG_ROOT, items);
}

ubase* trafficreport_hourticklist::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, TR_HOURTICK_XMLTAG_ROOT) == 0)
	{
		u = dynamic_cast<trafficreport_hourtick *>(new trafficreport_hourtick());
		add_element(u);
	}

	return u;
}

}