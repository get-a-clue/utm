#ifndef UTM_ARP_TABLE_H
#define UTM_ARP_TABLE_H

#pragma once
#include <utm.h>

#include <addrip_v4.h>
#include <addrmac.h>

#include <map>

#include <boost/thread/mutex.hpp>

namespace utm {

typedef std::map<utm::addrip_v4, utm::addrmac> arptable_container;

class arp_table
{
public:
	static const char this_class_name[];

	arp_table(void);
	~arp_table(void);

	void clear();
	size_t size() const;
	void fill();

	bool findmac(const addrip_v4& ip, addrmac& mac) const;

private:
	mutable boost::mutex guard;
	arptable_container entries;

public:
	static void test_all();
};

}

#endif // UTM_ARP_TABLE_H
