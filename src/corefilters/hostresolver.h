#pragma once

#include <addrip_v4.h>

#include <set>

#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/ip/basic_resolver_iterator.hpp>

#define TESTDOMAIN_UTM_NAME "test-purpose-domain.utm"
#define TESTDOMAIN_UTM_IPADDR_V4 "10.0.99.100"

namespace utm {

typedef std::set<addrip_v4> hostresolver_result_container;

class hostresolver
{
public:
    static const char this_class_name[];

	hostresolver(boost::asio::io_service* ios);
	~hostresolver(void);

	boost::asio::io_service* ios_ptr;
	boost::asio::ip::tcp::resolver resolver_;
	boost::asio::deadline_timer timer_;

	std::string hostname;
	bool is_timeout;
	hostresolver_result_container result;

	void on_timeout(const boost::system::error_code &err);
	void start_resolve(const char* hostname, int timeout_seconds);
	void finish_resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

private:
	void stop();

public:
	static void resolve(const char* hostname, int timeout_seconds, hostresolver_result_container& result, bool& is_timeout);

#ifdef UTM_DEBUG
	static void test_all();
#endif
};

}