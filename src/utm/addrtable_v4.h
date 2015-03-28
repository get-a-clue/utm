#ifndef _UTM_ADDRTABLE_V4_H
#define _UTM_ADDRTABLE_V4_H

#pragma once
#include <utm.h>

#include <addrip_v4.h>
#include <addrtable.h>
#include <gstring.h>

namespace utm {

class addrtable_v4 : public addrtable<addrip_v4>
{
public:
	static const char this_class_name[];

	addrtable_v4(void);
	~addrtable_v4(void);

	bool check_addr_network(const addrip_v4& addr) const;
	bool check_addr_network(const addrip_v4& addr, addrip_v4& net, addrip_v4& mask) const;
	bool import_from_txt(const gstring& input_filename);

#ifdef UTM_DEBUG
	static void test_all();
#endif
};

}

#endif // _UTM_ADDRTABLE_V4_H