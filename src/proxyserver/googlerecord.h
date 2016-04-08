#pragma once

#include <event_defs.h>
#include <event_msg_base.h>
#include <ubase.h>

namespace utm {

class googlerecord : public event_msg_base, public ubase
{
public:
	static const char this_class_name[];

	googlerecord(void);
	~googlerecord(void);

	bool equals(const ubase* rhs) const;
	bool operator==(const googlerecord& rhs) const;

	const char *get_this_class_name() const { return this_class_name; };

	std::string msg;
	std::string site;
	int result_code;

	std::string to_string(bool make_localization = false);

	void clear() { };
	void xml_create() { };
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *tag_name, const char *classname) { return NULL; };
};

}