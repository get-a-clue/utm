#ifndef _UTM_USERS_CONNECTION_H
#define _UTM_USERS_CONNECTION_H

#pragma once

#include "user_connection.h"

#include <boost/thread/mutex.hpp>

#include <addrip_v4.h>
#include <ubase.h>
#include <utimestamp.h>
#include <map>
#include <list>

typedef std::multimap<unsigned int, utm::user_connection> users_connected_container;
typedef std::map<utm::addrip_v4, std::pair<unsigned int, utm::utimestamp>> ip2user_container;
typedef std::list<utm::addrip_v4> addr4s_container;

namespace utm {

class users_connected :
	public ubase
{
public:
    static const char this_class_name[];

public:
	static unsigned int update_timeout_sec;

	users_connected(void);
	users_connected(const users_connected& rhs);
	virtual ~users_connected(void);

    bool  operator==(const users_connected& rhs) const;
	void copy_from_safe(const users_connected& source);
	void copy_to_safe(users_connected& destination) const;

	void update_user(unsigned int uid, const user_connection& request);
	bool check_for_double_login(unsigned int uid, const addrip_v4& addr, const utimestamp& current_timestamp);
	bool check_user_and_ip(unsigned int uid, const addrip_v4& addr, const utimestamp& current_timestamp);
	bool check_for_any_ip(const addrip_v4& addr, const utimestamp& current_timestamp);
	bool find_user_ips(unsigned int uid, addr4s_container& addrs, bool& is_disabled);
	void purge_disconnected_users(const utimestamp& current_timestamp);

    void clear();
    void xml_create();
    void xml_catch_value(const char *keyname, const char *keyvalue);
    virtual ubase* xml_catch_subnode(const char *name) { return NULL; };

protected:
	mutable boost::mutex guard;
	users_connected_container users;
	ip2user_container ips;

public:
#ifdef UTM_DEBUG
	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num);
#endif
};

}

#endif // _UTM_USERS_CONNECTION_H