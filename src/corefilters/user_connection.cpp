#include "StdAfx.h"
#include "user_connection.h"

#include <stdio.h>
#include <stdlib.h>
#include <stringtools.h>

#include <string>
#include <vector>

namespace utm {

user_connection::user_connection(void)
{
}

user_connection::user_connection(const addrip_v4& _addr, const release_info& _agent_version)
{
	addr = _addr;
	agent_version = _agent_version;
	is_disabled = false;
	updatestamp.now();
}

user_connection::~user_connection(void)
{
}

bool user_connection::operator==(const user_connection& rhs) const
{
	if (!(addr == rhs.addr)) return false;
    if (!(updatestamp == rhs.updatestamp)) return false;
	if (!(is_disabled == rhs.is_disabled)) return false;
	if (!(agent_version == rhs.agent_version)) return false;

	return true;
}

void user_connection::to_string(unsigned int uid, std::string& str)
{
	char buf[100];
#if defined(_MSC_VER)
	sprintf_s(buf, 100, "%u,%s,%s,%d,%s", 
#else
	sprintf(buf, "%u,%s,%s,%d,%s",
#endif
		uid, addr.to_string().c_str(), updatestamp.to_string().c_str(), is_disabled, agent_version.to_string().c_str());

	str.assign(buf);
}

unsigned int user_connection::from_string(const char* p)
{
	std::vector<std::string> result;
	utm::stringtools::split(result, p, ',');

	if (result.size() != 5)
		return 0;

	unsigned int uid = atol(result[0].c_str());
	addr.from_string(result[1].c_str());
	updatestamp.from_string(result[2].c_str());
	is_disabled = result[3] == "1";
	agent_version.from_string(result[4].c_str());

	return uid;
}

}
