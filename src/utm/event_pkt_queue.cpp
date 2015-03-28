#include "stdafx.h"
#include "event_pkt_queue.h"

#include "ubase_test.h"

namespace utm {

const char event_pkt_queue::this_class_name[] = "event_pkt_queue";

event_pkt_queue::event_pkt_queue(void)
{
}


event_pkt_queue::~event_pkt_queue(void)
{
}

void event_pkt_queue::parse_diff_dump(const char *xml_dump)
{
	xml_parse(xml_dump, true);
}

#ifdef UTM_DEBUG
void event_pkt_queue::test_all()
{
	test_report tr(this_class_name);

	event_pkt_queue q;

	event_pkt p1;
	{
		p1.type = 1;
		p1.src_mac.from_string("01:02:03:04:05:06");
		p1.dst_mac.from_string("a1:b2:c3:d4:e5:f6");
		p1.src_ip4.from_string("10.30.111.1");
		p1.dst_ip4.from_string("172.31.99.244");
		p1.proto = 6;
		p1.src_port = 25;
		p1.dst_port = 1099;
		p1.length = 1500;
		p1.id = q.add_item(p1, true);
	}

	event_pkt p2;
	{
		p2.type = 1;
		p2.src_mac.from_string("21:22:23:24:25:26");
		p2.dst_mac.from_string("31:32:33:34:35:36");
		p2.src_ip4.from_string("101.130.211.191");
		p2.dst_ip4.from_string("172.31.0.0");
		p2.proto = 17;
		p2.src_port = 53;
		p2.dst_port = 2099;
		p2.length = 89;
		p2.id = q.add_item(p2, true);
	}

	std::string x1;
	q.make_diff_dump(0, x1);

	event_pkt_queue u;
	u.parse_diff_dump(x1.c_str());

	TEST_CASE_CHECK(u.size(), size_t(2));

	{
		event_pkt k1;
		u.pop_front_item(k1);
		TEST_CASE_CHECK_NOSERIALIZE(k1, p1);

		event_pkt k2;
		u.pop_front_item(k2);
		TEST_CASE_CHECK_NOSERIALIZE(k2, p2);
	}

	TEST_CASE_CHECK(u.size(), size_t(0));
}
#endif

}
