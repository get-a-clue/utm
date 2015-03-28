#pragma once

#if defined(_MSC_VER)
#define UTM_WIN
#endif

#ifdef UTM_WIN

#include <Windows.h>
#include <tchar.h>

#endif

#include "conditem.h"

#include <addrip_v4.h>

namespace utm {

class conditem_addrip :	public conditem
{
public:
	static const char this_class_name[];
	const char* get_this_class_name() const { return conditem_addrip::this_class_name; };

	enum addrtypes { addr_none = 0, addr_any = 1, addr_single = 2, addr_network = 3, addr_range = 4 };

	conditem_addrip(void);
	conditem_addrip(addrtypes _addrtype, const addrip_v4& _addr1, const addrip_v4& _addr2);
	~conditem_addrip(void);

	void copy_properties(const conditem& rhs);
	void set_conditem_addrip(addrtypes _addrtype, const addrip_v4& _addr1, const addrip_v4& _addr2);

	void clear();
	bool parse(const std::string& str);
	std::string to_string() const;

	condtypes get_conditem_type() const	{ return condtype_srcip; };
	bool do_check(proxyrequest_params *params);

	addrtypes addrtype;
	addrip_v4 addr1;
	addrip_v4 addr2;
};

}