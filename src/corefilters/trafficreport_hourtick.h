#ifndef UTM_TRAFFICREPORT_HOURTICK_H
#define UTM_TRAFFICREPORT_HOURTICK_H

#pragma once
#include "trafficreport_hourtick_base.h"

#include <string>
#include <stdio.h>

#include <utime.h>

namespace utm {

class trafficreport_hourtick : public trafficreport_hourtick_base
{
public:
	trafficreport_hourtick();
	~trafficreport_hourtick();

	bool xml_has_root_attr() const;
	void xml_get_root_attr(xmlattr_container& attr);
	void xml_catch_subnode_attribute(const char *attrname, const char* attrvalue);
	void xml_catch_rootnode_attribute(const char *attrname, const char* attrvalue);

	unsigned int get_id() const;
	void set_id(unsigned int id);

	utime ut;

	static void utime2hourts(const utime& tm, std::string& str);
	static void hourts2utime(const char* str, utime& tm);
	static unsigned int utime2id(const utime& ut);
	static void id2utime(unsigned int idts, utime& ut);

#ifdef UTM_DEBUG
	static void test_all();
#endif
};

}

#endif
