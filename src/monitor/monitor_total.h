#ifndef UTM_MONITOR_TOTAL_H
#define UTM_MONITOR_TOTAL_H

#pragma once
#include <boost/asio.hpp>
#include <utm.h>
#include <utime.h>

#include "macvendors.h"
#include "monitor_result.h"
#include "monitor_minidump.h"
#include <monitor_range_list.h>
#include <addrip_v4.h>
#include <gstring.h>
#include <enumhelper.h>

#include <list>
#include <vector>
#include <map>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

namespace utm {

struct dnsserver4addr 
{
	addrip_v4 addr;
	addrip_v4 dns;
};

typedef std::vector<dnsserver4addr> host2resolve_container;
typedef std::vector<monitor_result> monitor_result_container;
typedef std::map<addrip_v4, unsigned int> monitor_index_container;
typedef std::list<monitor_range> monitor_range_container;
typedef std::map<addrmac, gstring> addrmac_comment_container;

class monitor_total
{
public:
	static const char this_class_name[];
	static boost::function<std::int64_t ()> get_current_hires_tick;

	/* fake function and vartiable - only for testing purposes */
	static std::int64_t get_fake_tick() { return ++fake_tick; };
	static std::int64_t fake_tick;

	monitor_total(void);
	~monitor_total(void);

	void clear();
	void safe_copy_to(monitor_total& duplicate) const;

	size_t result_size() const;
	size_t maccomments_size() const;
	int get_init_count() const;

private:
	volatile int init_count;
	std::int64_t last_reverse_lookup_tick;

public:
	void init_from_ranges(const monitor_range_list& r);
	bool get_result_record(size_t index, monitor_result& mr) const;
	void set_result_record(size_t index, const monitor_result& mr);
	void set_reverse_hostname(const addrip_v4& addr, const char *hostname);

	void extract_minidump(monitor_minidump& mini, __int64 nRemoteReverseLookupTick);
	void apply_minidump(const monitor_minidump& mini);

	void format_monitoring_message(const monitor_result& r, const macvendors& mv, gstring& msg) const;
	static void parse_monitoring_message(const gstring& msg, utm::addrip_v4& ip, utm::gstring& state, utm::gstring& flag, utm::gstring& descr, utm::gstring& hostname, utm::addrmac& mac);
	void analyze(const macvendors& mv);

	boost::function<void (const gstring& msg, bool is_debug)> add_event;

//private:
public:
	mutable boost::mutex guard;
	
	monitor_range_list ranges;						// copy of monitoring configuration
	addrmac_comment_container maccomments;			// table with comments based on mac addresses (extracted from ranges - just for faster access)
	monitor_result_container results;				// results and internal state of monitoring hosts set
	monitor_index_container results_index;
	host2resolve_container host2resolve;

private:
	inline void append_monitoring_message(gstring& msg, const gchar* keyname, const gstring& keyvalue) const
	{
		if (keyvalue.empty())
			return;

		msg.append(keyname);
		msg.append(keyvalue);
		msg.append(_U(";"));
	}

public:
	static void test_all();
	static void test_print_event(const gstring& msg, bool is_debug)
	{
		utm::utime t(true);
		std::cout << t.to_string(utime::format_log) << " " << msg.getmb() << std::endl;
	};
};

}

#endif // UTM_MONITOR_TOTAL_H
