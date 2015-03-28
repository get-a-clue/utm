#ifndef _UTM_EVENT_MSG_QUEUE_H
#define _UTM_EVENT_MSG_QUEUE_H

#pragma once
#include <utm.h>

#include <event_msg.h>
#include <event_holder.h>

#include <map>

namespace utm {

typedef std::list<event_msg> event_msg_queue_container;
typedef std::list<event_msg>::iterator event_msg_queue_iterator;
typedef std::map<std::string, event_id> event_msg_flags_container;
typedef std::map<std::string, event_id>::iterator event_msg_flags_iterator;

class event_msg_queue : public event_holder<event_msg>
{
public:
	static const char this_class_name[];

	event_msg_queue(void);
	~event_msg_queue(void);

	void parse_diff_dump(const char *xml_dump);
	void parse_diff_dump(const char *xml_dump, event_msg_flags_container& flags);

#ifdef UTM_DEBUG
public:
	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num)
	{
		clear();

		add_message(event_pane::EP_AGENT, event_type::ET_DEBUG, "message 1");
		add_message(event_pane::EP_AGENT, event_type::ET_ERROR, "message 2");
		add_message(event_pane::EP_AGENT, event_type::ET_INFORMATION, "message 3");
	}

	static void test_parse_diff_dump_flags();
#endif
};

}

#endif // _UTM_EVENT_MSG_QUEUE_H
