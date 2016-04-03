#include "stdafx.h"
#include "trafficreport_filterlist.h"

namespace utm {

trafficreport_filterlist::trafficreport_filterlist()
{
}

trafficreport_filterlist::~trafficreport_filterlist()
{
}

void trafficreport_filterlist::xml_create()
{
	xml_append_root("TrafficReportFilterFakeRootNode");
	xml_append_node(TR_FILTER_XMLTAG_ROOT, items);
};

ubase* trafficreport_filterlist::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, TR_FILTER_XMLTAG_ROOT) == 0)
	{
		u = dynamic_cast<ubase *>(new trafficreport_filter());
		add_element(u);
	}

	return u;
}

}