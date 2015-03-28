#ifndef GOOGLECACHE_H
#define GOOGLECACHE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <list>

#include <boost/bind.hpp>
#include <boost/signal.hpp> 
#include <boost/thread/condition.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
namespace bp = boost::posix_time;

namespace utm {

class signalcatcher
{
public:
	signalcatcher() { };
	~signalcatcher() { };

	boost::condition signal;

	void on_completed()
	{
		signal.notify_one();
	}
};
typedef std::shared_ptr<signalcatcher> signalcatcher_ref;

typedef boost::signal<void ()> query_callback;
typedef std::shared_ptr<query_callback> query_callback_ref;

class googlequery
{
public:
	googlequery()
	{ 
		is_complete = false;
		status = 0;
	};
	
	~googlequery() { };

	bool is_complete;
	std::string site;
	int status;

	bp::ptime start;
	bp::ptime finish;

	std::list<query_callback_ref> callbacks;
	std::list<signalcatcher_ref> catchers;

	void notify()
	{
		std::list<query_callback_ref>::iterator i;
		for (i = callbacks.begin(); i != callbacks.end(); ++i)
		{
//			std::string debugstr = "notify: " + site;
			(*i)->operator()();
		}
	}
};

typedef std::list<googlequery> googlequeries;

#define GOOGLECACHE_NOT_FOUND -1
#define GOOGLECACHE_IS_PENDIND 0

class googlecache
{
public:
	googlecache(void);

	boost::mutex guard;
	googlequeries queries;

	int check_site(const std::string& site);
	void add_pending_query(const std::string& site);
	void remove_pending_query(const std::string& site);
	void set_completed_query(const std::string& site, int status);
	void add_callback(const std::string& site, signalcatcher_ref& callback_object);
};

}

#endif // GOOGLECACHE_H
