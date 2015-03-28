#ifndef _UTM_DHCPLEASELIST_H
#define _UTM_DHCPLEASELIST_H

#pragma once
#include <ubaselist.h>
#include <singledhcplease.h>

namespace utm {

class dhcpleaselist : public ubaselist<singledhcplease>
{
public:
	dhcpleaselist(void);
	~dhcpleaselist(void);

	bool operator==(const dhcpleaselist& rhs) const;

	bool has_write_protection() const { return false; };

	void clear();
    void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *keyname);
	void xml_catch_subnode_finished(const char *keyname);
};

}

#endif // _UTM_DHCPLEASELIST_H