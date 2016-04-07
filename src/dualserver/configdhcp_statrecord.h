#ifndef CONFIGDHCP_STATRECORD_H
#define CONFIGDHCP_STATRECORD_H

#pragma once

#include <ubase.h>
#include <addrip_v4.h>
#include <addrmac.h>
#include <gstring.h>

#define CONFIGDHCP_XMLTAG_DHCPSTATRECORD "S"
#define CONFIGDHCP_XMLTAG_DHCPSTATRECORD_OPEN "<S>"
#define CONFIGDHCP_XMLTAG_DHCPSTATRECORD_CLOSE "</S>"

namespace utm {

class configdhcp_statrecord : public ubase
{
public:
	configdhcp_statrecord(void);
	~configdhcp_statrecord(void);

	bool equals(const ubase* rhs) const;
	bool operator ==(const configdhcp_statrecord& rhs) const;

	addrip_v4 ip;
	addrmac mac;
	gstring comment;

	void clear();
	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue);
	ubase* xml_catch_subnode(const char *tag_name, const char* classname) { return NULL; };
};

}

#endif // CONFIGDHCP_STATRECORD_H