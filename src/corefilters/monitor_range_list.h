#pragma once

#include <ubaselist.h>
#include "monitor_range.h"

namespace utm {

class monitor_range_list : public ubaselist<monitor_range>
{
public:
	monitor_range_list(void);
	~monitor_range_list(void);

	bool operator==(const monitor_range_list& rhs) const;

	void clear();
    void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *keyname);
};

}