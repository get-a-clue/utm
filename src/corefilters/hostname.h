#ifndef _UTM_HOSTNAME_H
#define _UTM_HOSTNAME_H

#pragma once

#include <string>
#include <set>
#include <addrip_v4.h>
#include <utime.h>

#include <boost/thread/mutex.hpp>

namespace utm {

class hostname
{
public:
	hostname(void);
	virtual ~hostname(void);

	bool  operator==(const hostname& rhs) const;

	void clear();
	void set_host(const char *hostname);
	const char* get_host() const { return host.c_str(); };
	unsigned int get_hash() const { return hash; }

protected:
	std::string host;
	unsigned int hash;
};

class hostname_ex : public hostname
{
public:
	hostname_ex();
	hostname_ex(const hostname_ex& hse);
	~hostname_ex();


	void update_addr();
	bool check_addr(unsigned int addr);
	bool check_addr(const utm::addrip_v4& addr);

	std::set<utm::addrip_v4> addrs_v4;
	utm::utime last_update;

	boost::mutex guard;

	static void test_all();
};

}

#endif // _UTM_HOSTNAME_H