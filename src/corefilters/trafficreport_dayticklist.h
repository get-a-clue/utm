#pragma once
#include <ubaselist.h>
#include "trafficreport_daytick.h"

namespace utm {

class trafficreport_dayticklist : public ubaselist<trafficreport_daytick>
{
public:
	trafficreport_dayticklist();
	trafficreport_dayticklist(const trafficreport_dayticklist& rhs) { operator=(rhs); };
	~trafficreport_dayticklist();

	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *tag_name, const char *class_name);
};

}