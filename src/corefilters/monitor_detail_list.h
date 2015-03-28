#pragma once

#include <ubaselist.h>
#include "monitor_detail_record.h"

namespace utm {

class monitor_detail_list : public ubaselist<monitor_detail_record>
{
public:
	monitor_detail_list(void);
	~monitor_detail_list(void);

	bool operator==(const monitor_detail_list& rhs) const;

    void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *keyname);
};

}