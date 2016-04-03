#ifndef UTM_TRAFFICREPORT_HOURTICKLIST_H
#define UTM_TRAFFICREPORT_HOURTICKLIST_H

#pragma once
#include <utm.h>

#include <ubaselist.h>
#include "trafficreport_hourtick.h"

namespace utm {

class trafficreport_hourticklist : public ubaselist<trafficreport_hourtick>
{
public:
	trafficreport_hourticklist();
	trafficreport_hourticklist(const trafficreport_hourticklist& rhs) { operator=(rhs); };
	~trafficreport_hourticklist();

	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *tag_name, const char *class_name);
};

}

#endif // UTM_TRAFFICREPORT_HOURTICKLIST_H