#ifndef UTM_TRAFFICREPORT_DAYTICK_H
#define UTM_TRAFFICREPORT_DAYTICK_H

#pragma once
#include "trafficreport_daytick_base.h"

#include <utime.h>

namespace utm {

class trafficreport_daytick : public trafficreport_daytick_base
{
public:
	trafficreport_daytick();
	~trafficreport_daytick();

	ubase* xml_catch_subnode(const char *keyname);
	void xml_catch_subnode_finished(const char *keyname);
	void xml_catch_subnode_attribute(const char *attrname, const char* attrvalue);
	void xml_catch_rootnode_attribute(const char *attrname, const char* attrvalue);

	void set_id(unsigned int id);
	unsigned int get_id() const;

	bool xml_has_root_attr() const;
	void xml_get_root_attr(xmlattr_container& attr);

	static void dayts2utime(const char* str, utime& ut);
	static void utime2dayts(const utime& ut, std::string& str);
	static unsigned int utime2id(const utime& ut);
	static void id2utime(unsigned int idts, utime& ut);

	utime ut;

	void update_counters(const utime& ctm, __int64 sent, __int64 recv);

#ifdef UTM_DEBUG
	static void test_all();
	static trafficreport_daytick test_get_daytick();
#endif
};

}

#endif // UTM_TRAFFICREPORT_DAYTICK_H


