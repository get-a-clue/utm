#ifndef _UTM_EVENT_PKT_QUEUE_H
#define _UTM_EVENT_PKT_QUEUE_H

#pragma once
#include <utm.h>

#include <event_holder.h>
#include <event_pkt.h>

#include <list>

namespace utm {

typedef std::list<event_pkt> event_pkt_queue_container;

class event_pkt_queue : public event_holder<event_pkt>
{
public:
	static const char this_class_name[];

	event_pkt_queue(void);
	~event_pkt_queue(void);

	void parse_diff_dump(const char *xml_dump);

#ifdef UTM_DEBUG
public:
	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num)
	{
		clear();
	}

	static void test_all();
#endif
};

}

#endif // _UTM_EVENT_PKT_QUEUE_H
