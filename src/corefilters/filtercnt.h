#ifndef UTM_FILTERCNT_H
#define UTM_FILTERCNT_H

#pragma once

#define MAXVALUES 1400
#define REFRESH_INTERVAL 2

#include <atomic>

#include <base64.h>
#include <gstring.h>

namespace utm {

class filtercnt
{
public:
	filtercnt(void);
	~filtercnt(void);

	filtercnt& operator=(const filtercnt& rhs);
	filtercnt(const filtercnt& rhs);

	void reset(bool also_reset_history);
	void copy(const filtercnt& fc);

private:
	__int64			cnt_prev;
	std::atomic<int_fast64_t>	cnt;
	std::atomic<int_fast64_t>	cnt_xml;
	std::atomic<int_fast64_t>	cnt_xml2;
	std::atomic<int_fast64_t>	cnt_logdb;

public:
	__int64 get_cnt() const;
	void set_cnt(__int64 value);
	void set_cnt_with_prev(__int64 value);
	void add_cnt(__int64 value);
	void add_cnt_with_prev(__int64 value);

	__int64 get_xml() const;
	void set_xml(__int64 value);
	void add_xml(__int64 value);

	__int64 get_xml2() const;
	void set_xml2(__int64 value);
	void add_xml2(__int64 value);

	__int64 get_logdb() const;
	void set_logdb(__int64 value);
	void add_logdb(__int64 value);

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