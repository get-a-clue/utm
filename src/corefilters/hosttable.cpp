#include "stdafx.h"
#include "hosttable.h"

#include "hostresolver.h"

#include <ubase_test.h>

namespace utm {

const char hosttable::this_class_name[] = "hosttable";

hosttable::hosttable(void)
{
}

hosttable::~hosttable(void)
{
}

void hosttable::add_host(const char* hostname)
{
	hostname_ex hse;
	hse.set_host(hostname);

	maphost::iterator iter = hosts.find(hse.get_hash());
	bool found = false;
	while (iter != hosts.end())
	{
		if (strcmp(iter->second.get_host(), hostname) == 0)
		{
			found = true;
			break;
		}
		++iter;
	}

	if (!found)
	{
		hosts.insert(std::pair<unsigned int, utm::hostname_ex>(hse.get_hash(), hse));
	}
}

void hosttable::clear()
{
	hosts.clear();
}

bool hosttable::checkaddr(unsigned int hostname_crc32, const char *hostname, const utm::addrip_v4& addr4)
{
	maphost::iterator iter = hosts.find(hostname_crc32);

	bool retval = false;
	while (iter != hosts.end())
	{
		if (strcmp(iter->second.get_host(), hostname) == 0)
		{
			retval = iter->second.check_addr(addr4);
			break;
		}
		++iter;
	}

	return retval;
}

void hosttable::refresh_hosttable()
{
	for (maphost::iterator iter = hosts.begin(); iter != hosts.end(); ++iter)
	{
		iter->second.update_addr();
	}
}

void hosttable::test_all()
{
	test_case::classname.assign(this_class_name);

	hosttable ht;
	ht.add_host(TESTDOMAIN_UTM_NAME);
	ht.add_host(TESTDOMAIN_UTM_NAME);
	ht.add_host("localhost");

	TEST_CASE_CHECK(ht.size(), size_t(2));

	ht.refresh_hosttable();

	{
		hostname_ex h1;
		h1.set_host(TESTDOMAIN_UTM_NAME);
		h1.update_addr();
		TEST_CASE_CHECK(h1.addrs_v4.size(), size_t(1));

		bool r1 = ht.checkaddr(h1.get_hash(), h1.get_host(), *h1.addrs_v4.begin());
		TEST_CASE_CHECK(r1, bool(true));
	}

	{
		hostname_ex h2;
		h2.set_host("localhost");
		h2.update_addr();
		TEST_CASE_CHECK(h2.addrs_v4.size(), size_t(1));

		bool r2 = ht.checkaddr(h2.get_hash(), h2.get_host(), *h2.addrs_v4.begin());
		TEST_CASE_CHECK(r2, bool(true));
	}

	return;
}

}
