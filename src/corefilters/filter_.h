#ifndef UTM_FILTER_H
#define UTM_FILTER_H

#pragma once


#include "filter_base.h"
#include "filtercnt.h"

namespace utm {

class filter : public filter_base
{
public:
	static const char this_class_name[];

public:
	filter(void);
	virtual ~filter(void);

	const char *get_this_class_name() const { return "filter"; };

	void clear(bool dont_clear_parent = false);

	unsigned int seq_id;
	filtercnt cnt_sent;
	filtercnt cnt_recv;

	bool is_equal(const filter& f) const;

	void rule_add(const rule& r);
	void rules_clear();

    std::string get_workhours_str() const;
    void parse_workhours_string(const char *workhours_string);
    ubase* xml_catch_subnode(const char *tag_name, const char *class_name);

	int GetSpeed() { return m_nSpeed; };
	void SetSpeed(int nSpeed) { m_nSpeed = nSpeed; }; 
	int GetSpeed2() { return m_nSpeed2; };
	void SetSpeed2(int nSpeed2) { m_nSpeed2 = nSpeed2; }; 

	int GetSpeedMaster() { return m_nSpeedMaster; };
	void SetSpeedMaster(int nSpeedMaster) { m_nSpeedMaster = nSpeedMaster; };

	void select_diffspeed(unsigned int diff_size, unsigned int start_diffindex, unsigned short* diffsent, unsigned short* diffrecv) const;
	void apply_diffspeed(unsigned int diff_size, unsigned int start_diffindex, unsigned short* diffsent, unsigned short* diffrecv);

	static void get_proto_name(int n, std::string& protoname);
#ifdef _UNICODE
	static void get_proto_name(int n, gstring& protoname);
#endif

public:
	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num);
};

}

#endif // UTM_FILTER_H