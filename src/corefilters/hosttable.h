#ifndef _UTM_HOSTTABLE_H
#define _UTM_HOSTTABLE_H

#pragma once

#include <addrip_v4.h>
#include <map>

#include "hostname.h"

typedef std::multimap<unsigned int, utm::hostname_ex> maphost;

namespace utm {

class hosttable
{
public:
	static const char this_class_name[];

	hosttable(void);
	~hosttable(void);

	void add_host(const char* hostname);
	void clear();
	size_t size() const { return hosts.size(); };

	inline bool checkaddr(const utm::hostname& hs, const utm::addrip_v4& addr4) { return checkaddr(hs.get_hash(), hs.get_host(), addr4); };
	bool checkaddr(unsigned int hostname_crc32, const char *hostname, const utm::addrip_v4& addr4);
	void refresh_hosttable();

private:
	maphost hosts;

public:
	static void test_all();

};

}

#endif // _UTM_HOSTTABLE_H
