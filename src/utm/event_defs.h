#ifndef _UTM_EVENT_DEFS_H
#define _UTM_EVENT_DEFS_H

#pragma once
#include <utm.h>

#include <gstring.h>

#include <cstdint>

namespace utm {

typedef std::uint64_t event_id;

struct event_type {
enum types { ET_ERROR = 0, ET_INFORMATION = 1, ET_DEBUG = 2, ET_WARNING = 3, ET_DEFAULT = 255 };

	static const char* c_str(int type)
	{
		if (type == ET_ERROR) return event_error;
		if (type == ET_INFORMATION) return event_information;
		if (type == ET_DEBUG) return event_debug;
		if (type == ET_WARNING) return event_warning;

		return event_default;
	}

protected:
	static const char event_error[];
	static const char event_information[];
	static const char event_debug[];
	static const char event_warning[];
	static const char event_default[];
};

struct event_pane {
enum panes { EP_DEFAULT = 0, EP_STAT = 1, EP_AGENT = 2, EP_FILTERSET = 3, EP_NETWORK = 4, EP_COUNTERS = 5,
	EP_AGENT_CMD = 6, EP_DENIED = 7, EP_XMLTRF = 8, EP_LOGADO = 9, EP_PKTCOL = 10, EP_HH = 11, EP_TRAFADMIN = 12, 
	EP_MONITORING = 13 };

	static const char* c_str(int pane)
	{
		if (pane == EP_NETWORK) return pane_network;
		if (pane == EP_HH) return pane_hh;
		if (pane == EP_MONITORING) return pane_monitoring;

		return pane_default;
	}

protected:
	static const char pane_default[];
	static const char pane_network[];
	static const char pane_hh[];
	static const char pane_monitoring[];
};

}

#endif  // _UTM_EVENT_DEFS_H
