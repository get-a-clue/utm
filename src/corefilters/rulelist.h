#ifndef _UTM_RULELIST_H
#define _UTM_RULELIST_H

#pragma once
#include <utm.h>
#include <ubaselist.h>
#include <rule.h>

namespace utm {

class rulelist : public ubaselist<rule>
{
public:
	rulelist();
	~rulelist();

	bool operator==(const rulelist& rhs) const;

	void clear();
	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *keyname);
};

}

#endif // _UTM_RULELIST_H