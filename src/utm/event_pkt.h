#ifndef _UTM_EVENT_PKT_H
#define _UTM_EVENT_PKT_H

#pragma once

#define EVENTMSG_XMLTAG_EVENTPKT "P"

#include <event_msg_base.h>
#include <ubase.h>
#include <addrmac.h>
#include <addrip_v4.h>

#include <cstdint>

namespace utm {

// This structure is used only for serizalation purpose
struct event_pkt_bare {
	event_id		id;				// 8 byte
	time_t			pkttime;		// 8 byte
	int				type;
	unsigned char	src_mac[MAX_ETHER_LENGTH];
	unsigned char	dst_mac[MAX_ETHER_LENGTH];
	std::uint32_t	src_ip4;
	std::uint32_t	dst_ip4;
	int				filter_id;
	int				rule_number;
	unsigned short	src_port;
	unsigned short	dst_port;
	unsigned short	length;
	unsigned short	proto;
};

class event_pkt : public event_msg_base, public ubase
{
public:
	static const char this_class_name[];
	static const char* get_xmltag() { return EVENTMSG_XMLTAG_EVENTPKT; };

	event_pkt(void);
	~event_pkt(void);

	bool operator==(const event_pkt& rhs) const;

	virtual const char *get_this_class_name() const { return this_class_name; };

	addrmac		src_mac;
	addrmac		dst_mac;
	addrip_v4	src_ip4;
	addrip_v4	dst_ip4;
	int filter_id;
	int rule_number;
	unsigned short src_port;
	unsigned short dst_port;
	unsigned short length;
	unsigned char proto;

	void to_string_for_xml(std::string& s) const;
	std::string to_string(bool make_localization = false) const;
	bool from_string_for_xml(const char* s);

	void clear();
	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue);
	ubase* xml_catch_subnode(const char *tag_name, const char *class_name) { return NULL; };

#ifdef UTM_DEBUG
	
#endif
};

}

#endif // _UTM_EVENT_PKT_H
