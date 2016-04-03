#ifndef _UTM_MONITOR_RANGE_H
#define _UTM_MONITOR_RANGE_H

#pragma once
#include <utm.h>

#include "monitor_range_base.h"

namespace utm {

class monitor_range : public monitor_range_base
{
public:
	monitor_range(void);
	~monitor_range(void);

	ubase* xml_catch_subnode(const char *tag_name, const char *class_name);

	void set_id(unsigned int id) { this->id = id; };
	unsigned int get_id() const { return id; };
	const gstring& get_name() const { return comment; };

	static const gchar* get_ping_status_str(int status);
	unsigned int get_ping_status_color(int status) const;
	void set_ping_status_color(int status, unsigned int color);

	monitor_detail_record* find_detail_by_mac(const addrmac& mac);
	monitor_detail_record* find_detail_by_ip(const addrip_v4& ip);

public:
	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num);
};

}

#endif // _UTM_MONITOR_RANGE_H