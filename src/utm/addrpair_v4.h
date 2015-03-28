#ifndef _UTM_ADDRPAIR_V4_H
#define _UTM_ADDRPAIR_V4_H

#pragma once
#include <utm.h>

#include <addrip_v4.h>
#include <addrpair.h>
#include <ubase_test.h>

typedef utm::addrpair<utm::addrip_v4> addrpair_v4;

namespace utm {

#ifdef UTM_DEBUG

void addrpair_v4::test_all()
{
	test_report tr("addrpair_v4");

	addrpair_v4 p1;
	p1.from_string("192.168.1.1 192.168.1.2", ' ');
	TEST_CASE_CHECK(p1.addr1.to_string(), std::string("192.168.1.1"));
	TEST_CASE_CHECK(p1.addr2.to_string(), std::string("192.168.1.2"));

	addrpair_v4 p2;
	p2.from_string("192.168.1.1  192.168.1.2", ' ');
	TEST_CASE_CHECK(p2.addr1.to_string(), std::string("192.168.1.1"));
	TEST_CASE_CHECK(p2.addr2.to_string(), std::string("192.168.1.2"));

	TEST_CASE_CHECK(p1 == p2, true);

	addrpair_v4 p3('/');
	p3.from_string("192.168.1.0/255.255.255.0");
	TEST_CASE_CHECK(p3.addr1.to_string(), std::string("192.168.1.0"));
	TEST_CASE_CHECK(p3.addr2.to_string(), std::string("255.255.255.0"));

	return;
}

#endif

}

#endif // _UTM_ADDRPAIR_V4_H