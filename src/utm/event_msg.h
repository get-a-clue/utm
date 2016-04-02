#ifndef _UTM_EVENT_MSG_H
#define _UTM_EVENT_MSG_H

#pragma once
#include <utm.h>

#include <utime.h>
#include <gstring.h>
#include <event_defs.h>
#include <event_msg_base.h>
#include <ubase.h>
#include <stringtools.h>

#include <iostream>
#include <string>

#define EVENTMSG_XMLTAG_EVENTMSG "E"
#define EVENTMSG_XMLTAG_ID "I"
#define EVENTMSG_XMLTAG_PANE "P"
#define EVENTMSG_XMLTAG_TYPE "Y"
#define EVENTMSG_XMLTAG_TIME "T"
#define EVENTMSG_XMLTAG_MESSAGE "M"

namespace utm {

class event_msg : public event_msg_base, public ubase
{
public:
	static const char this_class_name[];
	static const char* get_xmltag() { return EVENTMSG_XMLTAG_EVENTMSG; };

	event_msg(void);
	event_msg(bool now);
	~event_msg(void);

	bool equals(const ubase* rhs) const;
	bool operator==(const event_msg& rhs) const;

	virtual const char *get_this_class_name() const { return this_class_name; };

	int				pane;
	gstring			message;

	std::string to_string(bool make_localization = false);
	gstring do_message_localization();

	void assign(const char *p) { message.assign(p); };

#ifdef UTM_WIN
#ifdef UNICODE
	void assign(const WCHAR *p) { message.assign(p); };
#endif
#endif

	void clear();
	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue);
	ubase* xml_catch_subnode(const char *tag_name, const char *class_name) { return NULL; };

#ifdef UTM_DEBUG
	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num);
#endif
};

}

#endif // _UTM_EVENT_MSG_H
