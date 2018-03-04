#include "stdafx.h"

#include "event_msg.h"

#define FIELDS_IN_EVENT 5

namespace utm {

const char event_msg::this_class_name[] = "event_msg";

event_msg::event_msg(void)
{
	clear();
}

event_msg::event_msg(bool now)
{
	clear();
	time.now();
}

event_msg::~event_msg(void)
{
}

bool event_msg::operator==(const event_msg& rhs) const
{
	if (id != rhs.id)
		return false;
	
	if (pane != rhs.pane)
		return false;

	if (type != rhs.type)
		return false;

	if (!(time == rhs.time))
		return false;

	if (message != rhs.message)
		return false;

	return true;
}

std::string event_msg::to_string(bool make_localization)
{
	std::ostringstream oss;
	oss << time.to_string() << " " << event_pane::c_str(pane) << " " << event_type::c_str(type) << " ";
	
	if (!make_localization)
		oss	<< message.getmb();
	else
	{
		gstring localized;
		stringtools::LocalizeMessage(message, localized);
		oss	<< localized.getmb();
	}

	return oss.str();
}

void event_msg::clear()
{
	id = 0;
	pane = event_pane::EP_DEFAULT;
	type = event_type::ET_DEFAULT;
	message.clear();
}

void event_msg::xml_create()
{
	std::ostringstream oss;
	oss << id << "|" << pane << "|" << type << "|" << time.to_string(utime::format_iso) << "|" << message.getutf8();
	std::string s = oss.str();

	xml_append_root(EVENTMSG_XMLTAG_EVENTMSG, s.c_str());
}

void event_msg::xml_catch_value(const char *keyname, const char *keyvalue)
{
	if (keyname == NULL)
		return;

	std::vector<std::string> fields;

	stringtools::split(fields, keyvalue, '|');
	if (fields.size() < FIELDS_IN_EVENT)
		return;

	size_t k = 0;
	id = boost::lexical_cast<std::uint64_t>(fields[k++]);
	pane = boost::lexical_cast<int>(fields[k++]);
	type = boost::lexical_cast<int>(fields[k++]);
	time.from_string(fields[k++].c_str(), utime::format_iso);

	// Message may include the character '|' (one is actually separator).
	// Therefore, parser should correctly handle this case.
	message.clear();
	while (k < fields.size())
	{
		if (k >= FIELDS_IN_EVENT)
		{
			message.append("|");
		}
		utm::gstring str;
		str.assign_fromutf8(fields[k++].c_str());
		message.append(str);
	}

	if (stringtools::localize != NULL)
	{
		gstring localized;
		stringtools::LocalizeMessage(message, localized);
		message = localized;
	}
}

gstring event_msg::do_message_localization()
{
	gstring retval;
	stringtools::LocalizeMessage(message, retval);

	return retval;
}

#ifdef UTM_DEBUG
void event_msg::test_fillparams(int test_num)
{
	clear();

	id = 777;
	type = 1;
	pane = 2;
	time.now();
	message = "Here, there and everywhere.";
}
#endif
}