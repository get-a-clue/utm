#pragma once

#include <string>
#include <list>

#include <boost/asio/ip/address.hpp>
#include <boost/thread/mutex.hpp>

#include <utime.h>

namespace utm {

struct gcookie
{
	boost::asio::ip::address clientip;
	std::string useragent;
	std::string host;
	utime ctime;
	std::string cookie;

	bool is_key_equal(const gcookie& rhs)
	{
		if (clientip != rhs.clientip)
			return false;

		if (useragent != rhs.useragent)
			return false;

		if (host != rhs.host)
			return false;

		return true;
	}
};

typedef std::list<gcookie> gcookie_container;

class googlecookies
{
public:
	static googlecookies* instance()
	{
		if (!self) self = new googlecookies();
		return self;
	}

	void set_cookie(boost::asio::ip::address clientip, const char *useragent, const char *host, const char *cookie);
	void get_cookie(boost::asio::ip::address clientip, const char *useragent, const char *host, std::string& cookie);

	gcookie_container cookies;
	boost::mutex guard;

private:
	static googlecookies* self;
	googlecookies(void);
	~googlecookies(void);
};

}