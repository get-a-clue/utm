#ifndef UTM_TRAFFICREPORT_H
#define UTM_TRAFFICREPORT_H

#pragma once
#include "trafficreport_base.h"

namespace utm {

struct counterdata {
	unsigned int filterid;
	__int64 sent;
	__int64 recv;
	gstring filtername;

	counterdata(unsigned int filterid, __int64 sent, __int64 recv, const gstring& filtername)
	{
		this->filterid = filterid;
		this->sent = sent;
		this->recv = recv;
		this->filtername = filtername;
	};
};

class trafficreport : public trafficreport_base
{
public:
	static const char this_class_name[];

	trafficreport();
	~trafficreport();

	const char *get_this_class_name() const { return this_class_name; };

	ubase* xml_catch_subnode(const char *tag_name, const char *class_name);
	void xml_catch_subnode_finished(const char *keyname);

	void update_counters(const utime& ctm, const counterdata& cdata);
	void update_mass_counters(const utime& ctm, const std::list<counterdata>& cdatas);

	static bool do_refresh(const char *xmlfile, const utime& ctm, const std::list<counterdata>& cdatas);

#ifdef UTM_DEBUG
	static void test_all();
#endif
};

}

#endif // UTM_TRAFFICREPORT_H