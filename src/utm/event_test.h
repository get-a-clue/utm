#ifndef _UTM_EVENT_TEST_H
#define _UTM_EVENT_TEST_H

#ifdef UTM_DEBUG

#pragma once
#include <utm.h>

#include <event_msg.h>
#include <event_holder.h>

namespace utm {

class event_test
{
public:

	static std::string make_fail_message(const char* classname, const char* msg)
	{
		std::ostringstream oss;
		oss << "error: " << classname << ": " << msg;
		return oss.str();
	}

	static void test_all()
	{
		event_holder<event_msg> queue;
		queue.set_capacity(5);

		queue.set_directory(utm::gstring("c:\\tmp"));
		queue.set_filename(utm::gstring("test_event_holder_%Y_%m_%d.txt"));
		queue.enable_file = true;
		queue.enable_console = true;

		queue.add_message(event_pane::EP_AGENT, event_type::ET_ERROR, "message 0");
		queue.add_message(event_pane::EP_AGENT, event_type::ET_DEBUG, "message 1");
		queue.add_message(event_pane::EP_AGENT, event_type::ET_INFORMATION, "message 2");
		queue.add_message(event_pane::EP_AGENT, event_type::ET_INFORMATION, "<LSTR>message 3</LSTR>");
		queue.add_message(event_pane::EP_AGENT, event_type::ET_INFORMATION, "<LSTR>message</LSTR> 4");
		queue.add_message(event_pane::EP_AGENT, event_type::ET_INFORMATION, "<LSTR>message</LSTR> 5 <LSTR>message</LSTR> 5");

		event_holder<event_msg> queue2;
		queue2 = queue;

		event_msg msg0, msg1, msg2, msg3, msg4, msg5;

		if (!queue.find_item(2, msg0))
			throw make_fail_message("event_holder<event_msg>", "find_item, test 0");

		if (!queue.pop_front_item(msg1))
			throw make_fail_message("event_holder<event_msg>", "empty queue, test 1");

		if (msg1.do_message_localization().getmb() != "message 1")
			throw make_fail_message("event_holder<event_msg>", "bad message, test 1");

		if (!queue.pop_front_item(msg2))
			throw make_fail_message("event_holder<event_msg>", "empty queue, test 2");

		if (msg2.do_message_localization().getmb() != "message 2")
			throw make_fail_message("event_holder<event_msg>", "bad message, test 2");

		if (!queue.pop_front_item(msg3))
			throw make_fail_message("event_holder<event_msg>", "empty queue, test 3");

		if (msg3.do_message_localization().getmb() != "message 3")
			throw make_fail_message("event_holder<event_msg>", "bad message, test 3");

		if (!queue.pop_front_item(msg4))
			throw make_fail_message("event_holder<event_msg>", "empty queue, test 4");

		if (msg4.do_message_localization().getmb() != "message 4")
			throw make_fail_message("event_holder<event_msg>", "bad message, test 4");

		if (!queue.pop_front_item(msg5))
			throw make_fail_message("event_holder<event_msg>", "empty queue, test 5");

		if (msg5.do_message_localization().getmb() != "message 5 message 5")
			throw make_fail_message("event_holder<event_msg>", "bad message, test 5");

		if (queue.size() != 0)
			throw make_fail_message("event_holder<event_msg>", "bad queue size, test empty");

		std::string xml;
		queue2.xml_create(3, xml);

		event_holder<event_msg> queue3;
		queue3.xml_parse(xml.c_str());

		event_msg msg31, msg41, msg51;

		if (!queue3.pop_front_item(msg31))
			throw make_fail_message("event_holder<event_msg>", "empty queue, test 31");

		if (msg31.do_message_localization().getmb() != "message 3")
			throw make_fail_message("event_holder<event_msg>", "bad message, test 31");

		if (!queue3.pop_front_item(msg41))
			throw make_fail_message("event_holder<event_msg>", "empty queue, test 41");

		if (msg41.do_message_localization().getmb() != "message 4")
			throw make_fail_message("event_holder<event_msg>", "bad message, test 41");

		if (!queue3.pop_front_item(msg51))
			throw make_fail_message("event_holder<event_msg>", "empty queue, test 51");

		if (msg51.do_message_localization().getmb() != "message 5 message 5")
			throw make_fail_message("event_holder<event_msg>", "bad message, test 51");

		if (queue3.size() != 0)
			throw make_fail_message("event_holder<event_msg>", "bad queue size, test empty");

		event_holder<event_msg> queue4;
		queue4.xml_parse(xml.c_str(), true);
		queue4.xml_parse(xml.c_str(), true);

		return;
	}
};

}

#endif // UTM_DEBUG

#endif // _UTM_EVENT_TEST_H