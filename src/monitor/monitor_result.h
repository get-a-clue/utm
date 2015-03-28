#ifndef UTM_MONITOR_RESULT_H
#define UTM_MONITOR_RESULT_H

#pragma once
#include <utm.h>

#include "monitor_result_base.h"

#include <monitor_range.h>
#include <gstring.h>

#define MONITOR_FLAG_EXCLUDE 1

namespace utm {

class monitor_result : public monitor_result_base
{
public:
//	enum ping_status { PING_STATUS_UNKNOWN = 0, PING_STATUS_OK = 1, PING_STATUS_OFFLINE = 2, PING_STATUS_QUALITYDROP = 3 };
	static const gchar* ping_status_str[];
	static const int ping_status_int[];

	static const gchar* get_ping_status_str(int pingstatus)
	{
		if ((pingstatus < 0) && (pingstatus >= PING_STATUS_MAXITEMS))
			return ping_status_str[PING_STATUS_UNKNOWN];

		return ping_status_str[pingstatus];
	}

	monitor_result(void);
	~monitor_result(void);

	static void analyze_and_set_new_status(monitor_result& mr, bool supress_quality_drop_status);
};

}

#endif // UTM_MONITOR_RESULT_H
