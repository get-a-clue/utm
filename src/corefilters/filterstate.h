#ifndef UTM_FILTERSTATE_H
#define UTM_FILTERSTATE_H

#pragma once
#include <utm.h>

#include <filterstate_base.h>

namespace utm {

class filterstate :	public filterstate_base
{
public:
	static const char this_class_name[];

public:
	filterstate(void);
	~filterstate(void);

	const char *get_this_class_name() const { return this_class_name; };

	void set_id(unsigned int id) { filter_id = id; };
	unsigned int get_id() const { return filter_id; };
	const gstring& get_name() const { return filter_name; };

	void xml_catch_subnode_attribute(const char *attrname, const char* attrvalue);
};

}

#endif // UTM_FILTERSTATE_H