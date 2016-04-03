#pragma once
#include <utm.h>

#include <ubaselist.h>
#include <filter2.h>

namespace utm {

class filterlist : public ubaselist<filter2>
{
public:
	filterlist(void);
	~filterlist(void);

	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *tag_name, const char *class_name);
};

}
