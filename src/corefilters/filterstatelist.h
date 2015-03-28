#ifndef UTM_FILTERSTATELIST_H
#define UTM_FILTERSTATELIST_H

#pragma once
#include <utm.h>
#include <ubaselist.h>
#include <filterstate.h>

namespace utm {

class filterstatelist :	public ubaselist<utm::filterstate>
{
public:
	filterstatelist(void);
	~filterstatelist(void);

	bool operator==(const filterstatelist& rhs) const;

	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *keyname);
};

}

#endif // UTM_FILTERSTATELIST_H
