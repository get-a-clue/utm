#ifndef _UTM_USER_CONNECTION_H
#define _UTM_USER_CONNECTION_H

#pragma once
#include <utm.h>

#include <utimestamp.h>
#include <addrip_v4.h>
#include <release_info.h>

namespace utm {

class user_connection
{
public:
	user_connection(void);
	user_connection(const addrip_v4& _addr, const release_info& _agent_version);
	~user_connection(void);

    bool  operator==(const user_connection& rhs) const;

	addrip_v4 addr;
    utimestamp updatestamp;
	bool  is_disabled;
	release_info agent_version;

	void to_string(unsigned int uid, std::string& str);
	unsigned int from_string(const char* p);
};

}

#endif // _UTM_USER_CONNECTION_H