#include "stdafx.h"
#include "event_pkt.h"

#include "base64.h"
#include "ubase_test.h"
#include "uproto.h"

#include <vector>

namespace utm {

const char event_pkt::this_class_name[] = "event_pkt";

event_pkt::event_pkt(void)
{
	clear();
}

event_pkt::~event_pkt(void)
{
}

bool event_pkt::operator==(const event_pkt& rhs) const
{
	if (id != rhs.id) return false;
	if (type != rhs.type) return false;
	if (time != rhs.time) return false;
	if (src_mac != rhs.src_mac) return false;
	if (dst_mac != rhs.dst_mac) return false;
	if (src_ip4 != rhs.src_ip4) return false;
	if (dst_ip4 != rhs.dst_ip4) return false;
	if (filter_id != rhs.filter_id) return false;
	if (rule_number != rhs.rule_number) return false;
	if (src_port != rhs.src_port) return false;
	if (dst_port != rhs.dst_port) return false;
	if (length != rhs.length) return false;
	if (proto != rhs.proto) return false;

	return true;
}

std::string event_pkt::to_string(bool make_localization) const
{
	std::string retval;
	retval.reserve(128);

	retval.append(time.to_string(utime::format_log, 0));

	char acl[40];
	sprintf_s(acl, 40, "ACL:%d/%d", filter_id, rule_number);

	char tmp[80];
	sprintf_s(tmp, 80, " %10s %5s  ", acl, uproto::get_name(proto));

	retval.append(tmp);

	retval.append(src_ip4.to_string());
	if ((proto == 6) || (proto == 17))
	{
		sprintf_s(tmp, 80, ":%u", src_port);
		retval.append(tmp);
	}

	retval.append(" -> ");

	retval.append(dst_ip4.to_string());
	if ((proto == 6) || (proto == 17))
	{
		sprintf_s(tmp, 80, ":%u", dst_port);
		retval.append(tmp);
	}

	sprintf_s(tmp, 80, "  len=%u", length);
	retval.append(tmp);

	return retval;
}

void event_pkt::to_string_for_xml(std::string& s) const
{
	event_pkt_bare b;
	{
		b.id = id;
		b.type = type;
		b.pkttime = time.to_time_t();
		memcpy(b.src_mac, src_mac.get(), MAX_ETHER_LENGTH);
		memcpy(b.dst_mac, dst_mac.get(), MAX_ETHER_LENGTH);
		b.src_ip4 = src_ip4.m_addr;
		b.dst_ip4 = dst_ip4.m_addr;
		b.filter_id = filter_id;
		b.rule_number = rule_number;
		b.src_port = src_port;
		b.dst_port = dst_port;
		b.length = length;
		b.proto = static_cast<unsigned short>(proto);
	}

	base64::encode(reinterpret_cast<const unsigned char*>(&b), sizeof(event_pkt_bare), s);
}

bool event_pkt::from_string_for_xml(const char* s)
{
	std::vector<unsigned char> data;
	base64::decode(s, data);

	event_pkt_bare b;
	if (data.size() != sizeof(event_pkt_bare))
		return false;

	{
		memcpy(&b, data.data(), sizeof(event_pkt_bare));
		id = b.id;
		type = b.type;
		time.from_time_t(b.pkttime);
		src_mac.set(b.src_mac);
		dst_mac.set(b.dst_mac);
		src_ip4.m_addr = b.src_ip4;
		dst_ip4.m_addr = b.dst_ip4;
		filter_id = b.filter_id;
		rule_number = b.rule_number;
		src_port = b.src_port;
		dst_port = b.dst_port;
		length = b.length;
		proto = static_cast<unsigned char>(b.proto);
	}

	return true;
}

void event_pkt::clear()
{
	event_msg_base::clear();

	src_mac.clear();
	dst_mac.clear();
	src_ip4.clear();
	dst_ip4.clear();
	filter_id = 0;
	rule_number = 0;
	src_port = 0;
	dst_port = 0;
	length = 0;
	proto = 0;
}

void event_pkt::xml_create()
{
	std::string s;
	to_string_for_xml(s);
	xml_append_root(EVENTMSG_XMLTAG_EVENTPKT, s.c_str());
}

void event_pkt::xml_catch_value(const char *keyname, const char *keyvalue)
{
	if ((keyname == NULL) || (keyvalue == NULL))
		return;

	if (strcmp(keyname, EVENTMSG_XMLTAG_EVENTPKT) == 0)
	{
		from_string_for_xml(keyvalue);
	}
}

#ifdef UTM_DEBUG
void event_pkt::test_all()
{
	test_report tr(this_class_name);

	size_t size_event_pkt_bare = sizeof(event_pkt_bare);
	TEST_CASE_CHECK(size_event_pkt_bare, size_t(56));

	event_pkt ep1;
	{
		ep1.id = 10;
		ep1.type = 1;
		ep1.time = utime::create_from_string("2010-12-31 12:59:05");
		ep1.filter_id = 2;
		ep1.rule_number = 1;
		ep1.proto = 17;
		ep1.src_mac.from_string("01:02:03:04:05:06");
		ep1.dst_mac.from_string("ff:ee:dd:cc:bb:aa");
		ep1.src_ip4.from_string("192.168.100.200");
		ep1.dst_ip4.from_string("10.30.255.255");
		ep1.src_port = 9999;
		ep1.dst_port = 65535;
		ep1.length = 1500;
	}

	std::string eps1;
	ep1.to_string_for_xml(eps1);

	event_pkt ep2;
	ep2.from_string_for_xml(eps1.c_str());

	TEST_CASE_CHECK_NOSERIALIZE(ep1, ep2);
}
#endif

}
