#pragma once
#include <ubaselist.h>
#include "trafficreport_hourtick.h"

namespace utm {

class trafficreport_hourticklist : public ubaselist<trafficreport_hourtick>
{
public:
	trafficreport_hourticklist();
	~trafficreport_hourticklist();

	bool operator==(const trafficreport_hourticklist& rhs) const;

	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *keyname);
};

}