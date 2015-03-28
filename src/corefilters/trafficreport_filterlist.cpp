#include "stdafx.h"
#include "trafficreport_filterlist.h"

namespace utm {

trafficreport_filterlist::trafficreport_filterlist()
{
}


trafficreport_filterlist::~trafficreport_filterlist()
{
}

bool trafficreport_filterlist::operator==(const trafficreport_filterlist& rhs) const
{
	return items == rhs.items;
}

void trafficreport_filterlist::xml_create()
{
	xml_append_root("TrafficReportFilterFakeRootNode");
	xml_append_node(TR_FILTER_XMLTAG_ROOT, items);
};

ubase* trafficreport_filterlist::xml_catch_subnode(const char *keyname)
{
	ubase *u = NULL;

	if (strcmp(keyname, TR_FILTER_XMLTAG_ROOT) == 0)
	{
		trafficreport_filter* r = insert_and_get_inserted();
		u = (ubase *)r;
	}

	return u;
}

}