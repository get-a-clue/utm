#ifndef _UTM_RULE_H
#define _UTM_RULE_H

#pragma once

#if defined(_MSC_VER)
#define UTM_WIN
#endif

#ifdef UTM_WIN

#include <Windows.h>
#include <tchar.h>

#endif

#include <ubase.h>
#include <addrmac.h>
#include <gstring.h>

#include <rule_common.h>
#include <rule_base.h>

namespace utm {

class rule : public rule_base
{
public:
	rule(void);
	rule(int src_type, const char* src_ip, const char* src_mask, int dst_type, const char* dst_ip, const char* dst_mask);
	virtual ~rule(void);

	void set_id(unsigned int id) { this->id = id; };
	unsigned int get_id() const { return id; };
	const gstring& get_name() const { return descr; };

	const char* get_src_host_str() const { return src_host.get_host(); };
	const char* get_dst_host_str() const { return dst_host.get_host(); };
	void parse_src_host_string(const char *p) { src_host.set_host(p); };
	void parse_dst_host_string(const char *p) { dst_host.set_host(p); };


public:
	static int test_get_testcases_number() { return 2; };
	void test_fillparams(int test_num);
};

}

#endif // _UTM_RULE_H