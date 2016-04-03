#pragma once

#include <ubaselist.h>
#include "monitor_detail_record.h"

namespace utm {

class monitor_detail_list : public ubaselist<monitor_detail_record>
{
public:
	monitor_detail_list(void);
	~monitor_detail_list(void);

    void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *tag_name, const char *class_name);
};

}