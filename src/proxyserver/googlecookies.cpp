#include "stdafx.h"
#include "googlecookies.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace utm {

googlecookies* googlecookies::self = NULL;

googlecookies::googlecookies(void)
{
}


googlecookies::~googlecookies(void)
{
}

void googlecookies::set_cookie(boost::asio::ip::address clientip, const char *useragent, const char *host, const char *cookie)
{
	if ((useragent == NULL) || (host == NULL))
		return;

	boost::mutex::scoped_lock lock(guard);

	gcookie gc;
	gc.clientip = clientip;
	gc.useragent.assign(useragent);
	gc.host.assign(host);
	gc.ctime.now();
	gc.cookie.assign(cookie);

	bool found = false;
	gcookie_container::iterator iter;

	for (iter = cookies.begin(); iter != cookies.end(); ++iter)
	{
		if (iter->is_key_equal(gc))
		{
			iter->ctime = gc.ctime;
			iter->cookie.assign(cookie);
			found = true;
			break;
		}
	}

	if (!found)
	{
		cookies.push_back(gc);
	}
}

void googlecookies::get_cookie(boost::asio::ip::address clientip, const char *useragent, const char *host, std::string& cookie)
{
	if ((useragent == NULL) || (host == NULL))
		return;

	boost::mutex::scoped_lock lock(guard);

	gcookie gc;
	gc.clientip = clientip;
	gc.useragent.assign(useragent);
	gc.host.assign(host);

	utime now(true);
	for (gcookie_container::iterator iter = cookies.begin(); iter != cookies.end(); )
	{
		if (iter->is_key_equal(gc))
		{
			boost::posix_time::time_duration diff = now.ptime - iter->ctime.ptime;

			if (diff.total_seconds() < 86400*30)    // cookie will be valid for 30 days
			{
				cookie = iter->cookie;
				break;
			}
			else
			{
				iter = cookies.erase(iter);
			}
		}
		else
		{
			++iter;
		}
	}
}


}