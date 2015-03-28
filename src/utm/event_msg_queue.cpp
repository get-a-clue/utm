#include "stdafx.h"

#include "event_msg_queue.h"

namespace utm {

const char event_msg_queue::this_class_name[] = "event_msg_queue";

event_msg_queue::event_msg_queue(void)
{
}


event_msg_queue::~event_msg_queue(void)
{
}

void event_msg_queue::parse_diff_dump(const char *xml_dump)
{
	xml_parse(xml_dump, true);
}

void event_msg_queue::parse_diff_dump(const char *xml_dump, event_msg_flags_container& flags)
{
	event_id last_id = this->next_event_id;
	xml_parse(xml_dump, true);

	if (flags.empty())
		return;

	event_msg_queue_container arrived_msg;
	safe_copy_container(arrived_msg, last_id);

	event_msg_flags_iterator fit;
	std::string msgstr;

	event_msg_queue_iterator iter;
	for(iter = arrived_msg.begin(); iter != arrived_msg.end(); ++iter)
	{
		for (fit = flags.begin(); fit != flags.end(); ++fit)
		{
			msgstr = iter->message.getmb();
			size_t fitsize = fit->first.size();

			if (strncmp(msgstr.c_str(), fit->first.c_str(), fitsize) == 0)
			{
				fit->second = iter->id;
				break;
			}
		}
	}
}

#ifdef UTM_DEBUG
void event_msg_queue::test_parse_diff_dump_flags()
{
	test_report tr(this_class_name);

	event_msg_queue q1;
	q1.add_message(event_pane::EP_AGENT, event_type::ET_DEBUG, "message 1");
	q1.add_message(event_pane::EP_AGENT, event_type::ET_ERROR, "message 2");
	q1.add_message(event_pane::EP_AGENT, event_type::ET_INFORMATION, "message 3");

	std::string xml1;
	q1.make_diff_dump(0, xml1);

	q1.add_message(event_pane::EP_AGENT, event_type::ET_DEBUG, "message 4");
	q1.add_message(event_pane::EP_AGENT, event_type::ET_ERROR, "message 5");
	q1.add_message(event_pane::EP_AGENT, event_type::ET_INFORMATION, "message 6");

	std::string xml2;
	q1.make_diff_dump(3, xml2);

	event_msg_flags_container flags;
	flags.insert(std::pair<std::string,event_id>("message 5", 0));
	flags.insert(std::pair<std::string,event_id>("message 4", 0));
	flags.insert(std::pair<std::string,event_id>("message 3", 0));

	event_msg_queue q2;
	q2.parse_diff_dump(xml1.c_str());
	q2.parse_diff_dump(xml2.c_str(), flags);

	TEST_CASE_CHECK(q2.size(), size_t(6));

	event_msg_queue_container c1;
	q1.safe_copy_container(c1, 0);

	event_msg_queue_container c2;
	q2.safe_copy_container(c2, 0);

	event_msg_queue_iterator iter1;
	event_msg_queue_iterator iter2 = c2.begin();
	for (iter1 = c1.begin(); iter1 != c1.end(); ++iter1, ++iter2)
	{
		TEST_CASE_CHECK_NOSERIALIZE(*iter1, *iter2);
	}

	TEST_CASE_CHECK(flags[std::string("message 5")], event_id(4));
	TEST_CASE_CHECK(flags[std::string("message 4")], event_id(3));
	TEST_CASE_CHECK(flags[std::string("message 3")], event_id(0));
}

#endif

}