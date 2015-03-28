#include "stdafx.h"
#include "pktcollector_e.h"

#include <ubase_test.h>

namespace utm {

const char pktcollector_e::this_class_name[] = "pktcollector_e";

pktcollector_e::pktcollector_e()
{
}


pktcollector_e::~pktcollector_e()
{
}

#ifdef UTM_DEBUG
void pktcollector_e::test_all()
{
	test_report tr(this_class_name);

	pktcollector_value v;
	pktcollector_key pkey;
	pktcollector_key pkey2;

	TEST_CASE_CHECK(false, pkey < pkey2);
	TEST_CASE_CHECK(false, pkey2 < pkey);

	pktcollector_e pce;
	
	addrip_v4 a1("192.168.1.2");
	addrip_v4 a1_3("192.168.1.3");
	addrip_v4 a2("201.202.203.204");

	ip_header ih0;
	ih0.src_ip_addr.m_addr = a1.m_addr;
	ih0.src_ip_addr = a1;
	ih0.dst_ip_addr.m_addr = a2.m_addr;
	ih0.dst_ip_addr = a2;
	ih0.src_port = 65000;
	ih0.dst_port = 80;
	ih0.proto = 6;
	ih0.ip_hl = 5;
	ih0.version = 4;
	ih0.length = 1200;
	ih0.flags = 0x0002;

	pce.put_packet(ih0, 0, 1, NULL, 0);
	TEST_CASE_CHECK(size_t(1), pce.size());

	ip_header ih1;
	ih1.src_ip_addr.m_addr = a2.m_addr;
	ih1.src_ip_addr = a2;
	ih1.dst_ip_addr.m_addr = a1.m_addr;
	ih1.dst_ip_addr = a1;
	ih1.src_port = 80;
	ih1.dst_port = 65000;
	ih1.proto = 6;
	ih1.ip_hl = 5;
	ih1.version = 4;
	ih1.length = 400;

	pce.put_packet(ih1, 1, 2, NULL, 0);
	TEST_CASE_CHECK(size_t(1), pce.size());

	std::list<std::pair<pktcollector_key, pktcollector_value>> fc1;

	pce.flush(fc1, 400);
	TEST_CASE_CHECK(size_t(1), fc1.size());

	pce.flush(fc1, 505);
	TEST_CASE_CHECK(size_t(1), fc1.size());

	ih0.src_ip_addr.m_addr = a1_3.m_addr;
	ih0.src_ip_addr = a1_3;

	pce.put_packet(ih0, 0, 200, NULL, 0);
	TEST_CASE_CHECK(size_t(2), pce.size());

	return;
}
#endif

}