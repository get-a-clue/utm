#pragma once

#include <event_defs.h>
#include <event_msg_base.h>
#include <ubase.h>

namespace utm {

class googlerecord : public event_msg_base, public ubase
{
public:
	googlerecord(void);
	~googlerecord(void);

	std::string msg;
	std::string site;
	int result_code;

	std::string to_string(bool make_localization = false);

	void clear() { };
	void xml_create() { };
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *name) { return NULL; };
};

}