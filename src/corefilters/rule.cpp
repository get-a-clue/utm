#include "StdAfx.h"
#include "rule.h"

#include <ubase_test.h>

namespace utm {

const char rule::this_class_name[] = "rule";

rule::rule(void)
{
}

rule::rule(int src_type, const char* src_ip, const char* src_mask, int dst_type, const char* dst_ip, const char* dst_mask)
{
	this->src_type = src_type;
	this->src_ip.from_string(src_ip);
	this->src_mask.from_string(src_mask);
	this->dst_type = dst_type;
	this->dst_ip.from_string(dst_ip);
	this->dst_mask.from_string(dst_mask);
}

rule::~rule(void)
{
}

void rule::test_fillparams(int test_num)
{
	clear();

	if (test_num == 0)
	{
		mirrored = DIRECTION_ONEWAY;
		descr = "Hey Jude! Don't make it bad.";
		action = ACTION_DENY;
		rwr_fwd = 1;
		proto = 6;
		nicalias = NICALIAS_PRIVATE;
		pkt_options = PKTOPT_ICMPECHOREQUEST;
		condition_type = COND_RECV_LESS;
		condition_limit = 999999;
		cond_mac_type = COND_MAC_SRC_EQUAL;
		cond_mac_data.from_string("aa:bb:cc:dd:ee:ff");
		prevfilter_type = PREVFILTER_MATCHED;
		natuse = NATUSE_YES;
		wday = WDAY_FRI|WDAY_SAT;
		time_from = 1;
		time_to = 22;
		src_type = RULE_HOST;
		src_ip.from_string("192.168.1.1");
		src_mask.from_string("255.255.255.255");
		src_addr_type = 432;
		src_port_type = 321;
		src_port = 888;
		src_port_to = 999;
		src_atkey = 123;
		src_uid = 234;
		src_procnick = 345;
		src_mac.from_string("21:22:23:24:25:26");
		src_host.set_host("cosanostra.com");
		dst_type = RULE_MAC;
		dst_ip.from_string("10.0.0.0");
		dst_mask.from_string("255.0.0.0");
		dst_addr_type = 987;
		dst_port_type = 876;
		dst_port = 65000;
		dst_port_to = 65535;
		dst_atkey = 678;
		dst_uid = 567;
		dst_procnick = 789;
		dst_mac.from_string("b1:b2:b3:b4:b5:ff");
		dst_host.set_host("www.w3.org");
	}

	if (test_num == 1)
	{
		xml_load("_legacyxml//rule1.xml");

		test_case::classname.assign(this_class_name);
		test_case::testcase_num = test_num;

		TEST_CASE_CHECK(descr, utm::gstring("test descr"));
		TEST_CASE_CHECK(mirrored, unsigned short(0));
		TEST_CASE_CHECK(action, int(2));
		TEST_CASE_CHECK(rwr_fwd, unsigned short(1));
		TEST_CASE_CHECK(proto, int(17));
		TEST_CASE_CHECK(nicalias, unsigned int(4));
		TEST_CASE_CHECK(cond_mac_type, unsigned int(1));
		TEST_CASE_CHECK(condition_limit, unsigned int(999));
		TEST_CASE_CHECK(cond_mac_data, utm::addrmac("99:88:77:66:55:44"));
		TEST_CASE_CHECK(prevfilter_type, (2));
		TEST_CASE_CHECK(natuse, (1));
		TEST_CASE_CHECK(wday, unsigned int(121));
		TEST_CASE_CHECK(time_from, unsigned short(3));
		TEST_CASE_CHECK(time_to, unsigned short(5));
		TEST_CASE_CHECK(src_type, (1));
		TEST_CASE_CHECK(src_ip, utm::addrip_v4("192.168.100.111"));
		TEST_CASE_CHECK(src_mask, utm::addrip_v4("255.255.0.0"));
		TEST_CASE_CHECK(src_port_type, unsigned short(1));
		TEST_CASE_CHECK(src_port, unsigned short(12345));
		TEST_CASE_CHECK(src_port_to, unsigned short(12500));
		TEST_CASE_CHECK(dst_type, int(4));
		TEST_CASE_CHECK(dst_mac, utm::addrmac("00:11:22:33:44:55"));
		TEST_CASE_CHECK(dst_port_type, unsigned short(2));
		TEST_CASE_CHECK(dst_port, unsigned short(23456));
		TEST_CASE_CHECK(dst_port_to, unsigned short(23900));
	}

}

}