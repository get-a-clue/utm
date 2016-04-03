#include "StdAfx.h"
#include "users_connected.h"

#include <ubase_test.h>

namespace utm {

unsigned int users_connected::update_timeout_sec = 10;
const char users_connected::this_class_name[] = "users_connected";

users_connected::users_connected(void)
{
}

users_connected::~users_connected(void)
{
}

users_connected::users_connected(const users_connected& rhs)
{
	users = rhs.users;
	ips = rhs.ips;
}

users_connected& users_connected::operator=(const users_connected& rhs)
{
	// this operator is private and can't be used
	return *this;
}

bool users_connected::equals(const ubase* rhs) const
{
	const users_connected* p = dynamic_cast<const users_connected *>(rhs);
	if (p == NULL)
	{
		return false;
	}

	return operator==(*p);
}

bool users_connected::operator==(const users_connected& rhs) const
{
	boost::mutex::scoped_lock lock(guard);

	return users == rhs.users;
}

void users_connected::copy_from_safe(const users_connected& source)
{
	boost::mutex::scoped_lock lock(guard);
	users = source.users;
	ips = source.ips;
}

void users_connected::copy_to_safe(users_connected& destination) const
{
	boost::mutex::scoped_lock lock(guard);
	destination.users = users;
	destination.ips = ips;
}

void users_connected::update_user(unsigned int uid, const user_connection& request)
{
	boost::mutex::scoped_lock lock(guard);

	std::pair<users_connected_container::iterator, users_connected_container::iterator> key_range = users.equal_range(uid);

	bool addr_found = false;

	if (key_range.first != users.end())
	{
		users_connected_container::iterator iter;
		for (iter = key_range.first; iter != key_range.second; ++iter)
		{
			user_connection& uc = iter->second;
			if (uc.addr == request.addr)
			{
				uc = request;
				ips[request.addr] = std::pair<unsigned int, utimestamp>(uid, request.updatestamp);
				addr_found = true;
			}
		}
	}

	if (!addr_found)
	{
		users.insert(std::pair<unsigned int, user_connection>(uid, request));
		ips[request.addr] = std::pair<unsigned int, utimestamp>(uid, request.updatestamp);
	}
}

bool users_connected::check_for_double_login(unsigned int uid, const addrip_v4& addr, const utimestamp& current_timestamp)
{
	boost::mutex::scoped_lock lock(guard);

	std::pair<users_connected_container::iterator, users_connected_container::iterator> key_range = users.equal_range(uid);

	bool found_double = false;

	if (key_range.first != users.end())
	{
		users_connected_container::iterator iter;
		for (iter = key_range.first; iter != key_range.second; ++iter)
		{
			user_connection& uc = iter->second;
			if (!current_timestamp.is_greater_than_by_sec(uc.updatestamp, users_connected::update_timeout_sec))
			{
				if (uc.addr != addr)
				{
					found_double = true;
					break;
				}
			}
		}
	}

	return found_double;
}

bool users_connected::check_user_and_ip(unsigned int uid, const addrip_v4& addr, const utimestamp& current_timestamp)
{
	boost::mutex::scoped_lock lock(guard);

	std::pair<users_connected_container::iterator, users_connected_container::iterator> key_range = users.equal_range(uid);

	bool found_user = false;

	if (key_range.first != users.end())
	{
		users_connected_container::iterator iter;
		for (iter = key_range.first; iter != key_range.second; ++iter)
		{
			user_connection& uc = iter->second;
			if (uc.addr == addr)
			{
				if (!current_timestamp.is_greater_than_by_sec(uc.updatestamp, users_connected::update_timeout_sec))
				{
					found_user = true;
				}
				break;
			}
		}
	}

	return found_user;
}

bool users_connected::check_for_any_ip(const addrip_v4& addr, const utimestamp& current_timestamp)
{
	boost::mutex::scoped_lock lock(guard);

	ip2user_container::iterator iter = ips.find(addr);
	if (iter == ips.end())
		return false;

	if (current_timestamp.is_greater_than_by_sec(iter->second.second, users_connected::update_timeout_sec))
		return false;

	return true;
}

bool users_connected::find_user_ips(unsigned int uid, addr4s_container& addrs, bool& is_disabled)
{
	boost::mutex::scoped_lock lock(guard);

	std::pair<users_connected_container::iterator, users_connected_container::iterator> key_range = users.equal_range(uid);

	if (key_range.first == users.end())
		return false;

	users_connected_container::iterator iter;
	for (iter = key_range.first; iter != key_range.second; ++iter)
	{
		user_connection& uc = iter->second;
		addrs.push_back(uc.addr);
		is_disabled = uc.is_disabled;
	}

	return true;
}

void users_connected::purge_disconnected_users(const utimestamp& current_timestamp)
{
	boost::mutex::scoped_lock lock(guard);

	bool found_disconnected = false;

	users_connected_container::iterator iter;
	for (iter = users.begin(); iter != users.end(); )
	{
		if (current_timestamp.is_greater_than_by_sec(iter->second.updatestamp, users_connected::update_timeout_sec))
		{
			iter = users.erase(iter);
			found_disconnected = true;
		}
		else
		{
			++iter;
		}
	}

	if (found_disconnected)
	{
		ips.clear();
		for (iter = users.begin(); iter != users.end(); ++iter)
		{
			ips[iter->second.addr] = std::pair<unsigned int, utimestamp>(iter->first, iter->second.updatestamp);
		}
	}
}

void users_connected::clear()
{
	boost::mutex::scoped_lock lock(guard);
	users.clear();
	ips.clear();
}

void users_connected::xml_create()
{
	users_connected_container users_copy;

	{
		boost::mutex::scoped_lock lock(guard);
		users_copy = users;
	}

    xml_append_root("UsersConnected");

	std::string dummy;
	users_connected_container::iterator iter;
	for (iter = users_copy.begin(); iter != users_copy.end(); ++iter)
	{
		iter->second.to_string(iter->first, dummy);
		xml_append_node("uc", dummy.c_str());
	}
}

void users_connected::xml_catch_value(const char *keyname, const char *keyvalue)
{
	if (strcmp(keyname, "uc") == 0)
	{
		user_connection uc;
		unsigned int uid = uc.from_string(keyvalue);

		if (uid > 0)
		{
			boost::mutex::scoped_lock lock(guard);
			users.insert(std::pair<unsigned int, user_connection>(uid, uc));

			ip2user_container::iterator iter = ips.find(uc.addr);
			if (iter != ips.end())
			{
				if (iter->second.second < uc.updatestamp)
				{
					iter->second = std::pair<unsigned int, utimestamp>(uid, uc.updatestamp);
				}
			}
			else
			{
				ips[uc.addr] = std::pair<unsigned int, utimestamp>(uid, uc.updatestamp);
			}
		}
	}
}

#ifdef UTM_DEBUG
void users_connected::test_fillparams(int test_num)
{
	clear();

	{
		user_connection uc;
		uc.addr = utm::addrip_v4("192.168.1.1");
		uc.updatestamp.now();
		uc.is_disabled = false;
		uc.agent_version.major = 1;
		uc.agent_version.minor = 4;
		uc.agent_version.build = 777;
		update_user(10, uc);

		uc.updatestamp.ts++;
		update_user(10, uc);

		uc.updatestamp.ts++;
		update_user(10, uc);
	}

	{
		user_connection uc;
		uc.addr = utm::addrip_v4("192.168.1.2");
		uc.updatestamp.now();
		uc.is_disabled = false;
		uc.agent_version.major = 1;
		uc.agent_version.minor = 4;
		uc.agent_version.build = 777;
		update_user(15, uc);
		update_user(16, uc);
		update_user(17, uc);
	}

	test_case::classname.assign(this_class_name);
	TEST_CASE_CHECK(users.size(), size_t(4));
	TEST_CASE_CHECK(ips.size(), size_t(2));

	return;
}
#endif

}