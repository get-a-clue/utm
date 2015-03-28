#ifndef UTM_FILTERCNT_H
#define UTM_FILTERCNT_H

#pragma once

#define MAXVALUES 1400
#define REFRESH_INTERVAL 2

#include <base64.h>
#include <gstring.h>

namespace utm {

class filtercnt
{
public:
	filtercnt(void);
	~filtercnt(void);

	void reset(bool also_reset_history);
	void copy(const filtercnt& fc);

	__int64			cnt;
	__int64			cnt_prev;
	__int64			delta;
	__int64			cnt_xml;
	__int64			cnt_xml2;
	__int64			cnt_logdb;
	unsigned short	cnt_set[MAXVALUES];
	unsigned int	cnt_momspeed1;
	unsigned int	cnt_speed;

	static unsigned short pack_speed(unsigned int speed);
	static unsigned int unpack_speed(unsigned short packed_speed);

	void do_refresh(unsigned int total_points, unsigned int next_point);
	void select_speedarray_base64(unsigned int total_points, unsigned int next_point, unsigned int array_elements_count, std::string& result) const;
};

}

#endif // UTM_FILTERCNT_H