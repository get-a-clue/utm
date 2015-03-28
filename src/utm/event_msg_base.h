#ifndef _UTM_EVENT_MSG_BASE_H
#define _UTM_EVENT_MSG_BASE_H

#pragma once
#include <utm.h>

#include <utime.h>
#include <event_defs.h>

namespace utm {

class event_msg_base
{
public:
	static const char* get_xmltag() { return "Request"; };

	event_msg_base(void);
	virtual ~event_msg_base(void);

	event_id		id;
	int				type;
	utime			time;

	void clear();
};

}

#endif // _UTM_EVENT_MSG_BASE_H
