#include "stdafx.h"
#include "googlecache.h"

#define GOOGLECACHE_EXPIRED_INTERVAL 300

namespace utm {

googlecache::googlecache(void)
{
}

int googlecache::check_site(const std::string& site)
{
	boost::mutex::scoped_lock lock(guard);

	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

	int retval = GOOGLECACHE_NOT_FOUND;
	googlequeries::iterator iter;

	for (iter = queries.begin(); iter != queries.end(); ++iter)
	{
		if ((*iter).site == site)
		{
			if ((*iter).is_complete)
			{
				retval = (*iter).status;
			}
			else
			{
				retval = GOOGLECACHE_IS_PENDIND;
			}
			break;
		}

		if ((*iter).is_complete)
		{
			boost::posix_time::time_duration elapsed = now - (*iter).finish;
			if (elapsed.total_seconds() > GOOGLECACHE_EXPIRED_INTERVAL)
			{
				iter = queries.erase(iter);
				if (iter == queries.end())
					break;
			}
		}
	}

	return retval;
}

void googlecache::add_pending_query(const std::string& site)
{
	boost::mutex::scoped_lock lock(guard);

	googlequery query;
	
	query.is_complete = false;
	query.site = site;
	query.start = boost::posix_time::microsec_clock::local_time();

	queries.push_back(query);
}

void googlecache::remove_pending_query(const std::string& site)
{
	boost::mutex::scoped_lock lock(guard);

	googlequeries::iterator iter;

	for (iter = queries.begin(); iter != queries.end(); ++iter)
	{
		if ((*iter).site == site)
		{
			queries.erase(iter);
			break;
		}
	}
}

void googlecache::set_completed_query(const std::string& site, int status)
{
	boost::mutex::scoped_lock lock(guard);

	googlequeries::iterator iter;
	for (iter = queries.begin(); iter != queries.end(); ++iter)
	{
		if ((*iter).site == site)
		{
			(*iter).status = status;
			(*iter).is_complete = true;
			(*iter).finish = boost::posix_time::microsec_clock::local_time();
			(*iter).notify();

			break;
		}
	}
}

void googlecache::add_callback(const std::string& site, signalcatcher_ref& callback_object)
{
	boost::mutex::scoped_lock lock(guard);

	googlequeries::iterator iter;
	for (iter = queries.begin(); iter != queries.end(); ++iter)
	{
		if ((*iter).site == site)
		{
			query_callback_ref call = query_callback_ref(new query_callback());
			call->connect(boost::function<void ()>(boost::bind(&signalcatcher::on_completed, callback_object.get())));
			(*iter).callbacks.push_back(call);
			(*iter).catchers.push_back(callback_object);
			break;
		}
	}
}

}