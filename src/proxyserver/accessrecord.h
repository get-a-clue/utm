#pragma once

#include <event_defs.h>
#include <event_msg_base.h>
#include <ubase.h>

#include <string>
#include <iostream>
#include <boost/asio/ip/address.hpp>

namespace utm {

class accessrecord : public event_msg_base, public ubase
{
public:
	static const char this_class_name[];

	accessrecord(void);
	~accessrecord(void);

	bool equals(const ubase* rhs) const;
	bool operator==(const accessrecord& rhs) const;

	const char *get_this_class_name() const { return this_class_name; };

	boost::asio::ip::address client_ip;
	std::string request_phase;
	std::string request_url;
	std::string referer;
	std::string resume;

	std::string to_string(bool make_localization = false);

	void clear() { };
	void xml_create() { };
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *tag_name, const char *classname) { return NULL; };
};

}
