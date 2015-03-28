#include "StdAfx.h"
#include "hostname.h"

#include <crc32.h>
#include <boost/asio.hpp>

#include "hostresolver.h"

namespace utm {

hostname::hostname(void)
{
}

hostname::~hostname(void)
{
}

bool hostname::operator==(const hostname& rhs) const
{
	return (strcmp(get_host(), rhs.get_host()) == 0);
}

void hostname::clear()
{
	host.clear();
	hash = 0;
}

void hostname::set_host(const char *hostname)
{
	if (hostname == NULL)
	{
		clear();
		return;
	}

	host.assign(hostname);
	hash = utm::crc32::calc(hostname);
}

hostname_ex::hostname_ex(void)
{
}

hostname_ex::hostname_ex(const hostname_ex& hse)
	: hostname((hostname)hse)
{
	addrs_v4 = hse.addrs_v4;
	last_update = hse.last_update;
}

hostname_ex::~hostname_ex(void)
{
}

bool hostname_ex::check_addr(unsigned int addr)
{
	addrip_v4 a(addr);
	boost::mutex::scoped_lock lock(guard);
	bool retval = addrs_v4.find(a) != addrs_v4.end();
	return retval;
}

bool hostname_ex::check_addr(const utm::addrip_v4& a)
{
	boost::mutex::scoped_lock lock(guard);
	bool retval = addrs_v4.find(a) != addrs_v4.end();
	return retval;
}

void hostname_ex::update_addr()
{
	std::set<addrip_v4> res_addrs_v4;
	bool is_timeout = false;

	try
	{
		hostresolver::resolve(get_host(), 1, res_addrs_v4, is_timeout);
		{
			boost::mutex::scoped_lock lock(guard);
			if (addrs_v4 != res_addrs_v4)
				addrs_v4 = res_addrs_v4;
		}
	}
	catch(const boost::system::error_code&)
	{
	}

	return;
}

void hostname_ex::test_all()
{
	{
		hostname_ex h1;
		h1.set_host("localhost");
		h1.update_addr();
	}

	{
		hostname_ex h1;
		h1.set_host(TESTDOMAIN_UTM_NAME);
		h1.update_addr();
	}

	return;
}

}