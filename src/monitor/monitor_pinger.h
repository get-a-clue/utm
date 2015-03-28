#ifndef UTM_MONITOR_PINGER_H
#define UTM_MONITOR_PINGER_H
/*
#pragma once
#include <utm.h>

#include <utime.h>

#include <list>
#include <map>

#include "monitor_total.h"
#include "macvendors.h"

namespace utm {

struct range_state
{
	range_state() { clear(); };

	UINT serial;
	UINT seq_no;
	UINT delay_no;

	void clear() { serial = 0; seq_no = 0; delay_no = 0; };
};

typedef std::map<unsigned int, range_state> range_state_container;

class monitor_pinger
{
public:
	monitor_pinger(void);
	~monitor_pinger(void);

	range_state_container seqs;

	void main(monitor_total& mt, std::list<monitor_result>& host_state_changes, const macvendors& mv, SOCKET s, unsigned short pid, bool lookup_arp);

	static void test_all();
	static void test_print_event(const gstring& msg, bool is_debug)
	{
		utm::utime t(true);
		std::cout << t.to_string(utime::format_log) << " " << msg.getmb() << std::endl;
	};
};

}
*/
#endif // UTM_MONITOR_PINGER2_H