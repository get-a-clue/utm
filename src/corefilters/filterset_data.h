#ifndef _UTM_FILTERSET_DATA_H
#define _UTM_FILTERSET_DATA_H

#pragma once
#include <utm.h>

#include <filtercnt.h>
#include <ubase_test.h>

typedef unsigned int fdata_time;

namespace utm {

class filterset_data
{
public:
	static const char this_class_name[];

	filterset_data();
	~filterset_data();

protected:
	unsigned int	total_points;
	unsigned int	next_point;
	unsigned int	next_seqnum;
public:
	unsigned int	seqnum_array[MAXVALUES];
	fdata_time		time_array[MAXVALUES];

	void copy(const filterset_data& fd);
	void clear();

	unsigned int get_total_points() const { return total_points; };
	void set_total_points(unsigned int value) { total_points = value; };

	void goto_next_point(time_t current_time, unsigned int current_seqnum = 0);
	void set_next_point(unsigned int value) { next_point = value; };
	unsigned int get_next_point() const { return next_point; };
	unsigned int get_current_point() const;
	bool is_point_array_full() const { return (total_points >= MAXVALUES); };

	void set_next_seqnum(unsigned int value) { next_seqnum = value; };
	unsigned int get_next_seqnum() const { return next_seqnum; };
	unsigned int get_current_seqnum() const { return next_seqnum == 0 ? 0 : next_seqnum - 1; };

	static void calc_start_last_index(unsigned int total_points, unsigned int next_point, unsigned int& start_index, unsigned int& last_index);
	void create_diff(const filterset_data& fd, unsigned int seqnum, unsigned int& diff_size, unsigned int& start_diffindex);

public:
#ifdef UTM_DEBUG
	static void test_all();
#endif
};

}

#endif // _UTM_FILTERSET_DATA_H