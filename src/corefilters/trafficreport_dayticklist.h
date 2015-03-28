#pragma once
#include <ubaselist.h>
#include "trafficreport_daytick.h"

namespace utm {

class trafficreport_dayticklist : public ubaselist<trafficreport_daytick>
{
public:
	trafficreport_dayticklist();
	~trafficreport_dayticklist();

	bool operator==(const trafficreport_dayticklist& rhs) const;

	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *keyname);
};

}