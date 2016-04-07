#include "stdafx.h"

#include "configdhcp_statrecord.h"
#include "configdhcp_statrecord.h"

#include <ostream>

namespace utm {

configdhcp_statrecord::configdhcp_statrecord(void)
{
}


configdhcp_statrecord::~configdhcp_statrecord(void)
{
}

void configdhcp_statrecord::clear()
{
	ip.clear();
	mac.clear();
	comment.clear();
}

bool configdhcp_statrecord::equals(const ubase* rhs) const
{
	const configdhcp_statrecord* p = dynamic_cast<const configdhcp_statrecord *>(rhs);
	if (p == NULL)
	{
		return false;
	}

	return operator==(*p);
}

bool configdhcp_statrecord::operator==(const configdhcp_statrecord& rhs) const
{
	if (ip != rhs.ip)
		return false;

	if (mac != rhs.mac)
		return false;

	if (comment != rhs.comment)
		return false;

	return true;
}

void configdhcp_statrecord::xml_create()
{
	std::ostringstream oss;
	oss << ip.to_string() << ";" << mac.to_string() << ";" << comment.getutf8();
	std::string s = oss.str();

	xml_append_root(CONFIGDHCP_XMLTAG_DHCPSTATRECORD, s.c_str());
}

void configdhcp_statrecord::xml_catch_value(const char *keyname, const char *keyvalue)
{
	const char* first = strchr(keyvalue, ';');
	if (first == NULL)
		return;

	const char* second = strchr(first + 1, ';');
	if (second == NULL)
		return;

	size_t firstlen = first - keyvalue;
	size_t secondlen = second - first - 1;

	std::string ipstr = std::string(keyvalue, firstlen);
	ip.from_string(ipstr.c_str());

	std::string macstr = std::string(first + 1, secondlen);
	mac.from_string(macstr.c_str());

	comment.assign_fromutf8(second + 1);
}

}