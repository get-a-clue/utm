#include "stdafx.h"
#include "hostresolver.h"

#include <utime.h>
#include <ubase_test.h>

#include <boost/bind.hpp>

namespace utm {

const char hostresolver::this_class_name[] = "hostresolver";

hostresolver::hostresolver(boost::asio::io_service* ios) : 
	resolver_(*ios), timer_(*ios), is_timeout(false)
{
	ios_ptr = ios;
}

hostresolver::~hostresolver(void)
{
}

void hostresolver::start_resolve(const char* hostname, int timeout_second)
{
	this->hostname.assign(hostname);

	timer_.expires_from_now(boost::posix_time::seconds(timeout_second));
    timer_.async_wait(boost::bind(&hostresolver::on_timeout, this, _1));

	boost::asio::ip::tcp::resolver::query query(hostname, "http");
	resolver_.async_resolve(query,
							boost::bind(&hostresolver::finish_resolve, this,
							boost::asio::placeholders::error,
							boost::asio::placeholders::iterator));

	ios_ptr->run();
}

void hostresolver::stop()
{
	resolver_.cancel();
	timer_.cancel();
}

void hostresolver::on_timeout(const boost::system::error_code &err) 
{
	if (!err)
	{
		if (err != boost::asio::error::operation_aborted)
		{
			is_timeout = true;
			stop();
		}
	}
}

void hostresolver::finish_resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	if (!err)
	{
		while (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator())
		{
			boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
			if (endpoint.address().is_v4())
			{
				addrip_v4 addr(endpoint.address().to_v4().to_ulong());
				result.insert(addr);
			}
			++endpoint_iterator;
		}
	}

	stop();
}

void hostresolver::resolve(const char* hostname, int timeout_seconds, hostresolver_result_container& result, bool& is_timeout)
{
	boost::asio::io_service io;
	utm::hostresolver hr(&io);
	hr.start_resolve(hostname, timeout_seconds);

	result = hr.result;
	is_timeout = hr.is_timeout;
}

#ifdef UTM_DEBUG
void hostresolver::test_all()
{
	test_case::classname.assign(this_class_name);

	hostresolver_result_container ipaddrs;
	bool is_timeout = false;

	for (int i = 0; i < 2; i++)
	{
		{
			hostresolver::resolve("10.30.111.101", 1, ipaddrs, is_timeout);
			TEST_CASE_CHECK(is_timeout, false);
			TEST_CASE_CHECK(ipaddrs.size(), size_t(1));
			TEST_CASE_CHECK(ipaddrs.begin()->to_string(), std::string("10.30.111.101"));
		}

		{
			hostresolver::resolve("localhost", 1, ipaddrs, is_timeout);
			TEST_CASE_CHECK(is_timeout, false);
			TEST_CASE_CHECK(ipaddrs.size(), size_t(1));
			TEST_CASE_CHECK(ipaddrs.begin()->to_string(), std::string("127.0.0.1"));
		}

		{
			hostresolver::resolve(TESTDOMAIN_UTM_NAME, 1, ipaddrs, is_timeout);
			TEST_CASE_CHECK(is_timeout, false);
			TEST_CASE_CHECK(ipaddrs.size(), size_t(1));
			TEST_CASE_CHECK(ipaddrs.begin()->to_string(), std::string(TESTDOMAIN_UTM_IPADDR_V4));
		}
	}
}
#endif

}