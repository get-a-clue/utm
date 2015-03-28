#include "StdAfx.h"
#include "monitor_minidump.h"

#include <stringtools.h>
#include <addrip_v4.h>
#include <addrmac.h>
#include <base64.h>
#include <gstring.h>
#include <ubase_test.h>

#include <list>
#include <vector>

namespace utm {

const char monitor_minidump::this_class_name[] = "monitor_minidump";


monitor_minidump::monitor_minidump(void)
{
}


monitor_minidump::~monitor_minidump(void)
{
}

void monitor_minidump::clear()
{
	dumpdata.clear();
	hostdata.clear();
}

bool monitor_minidump::operator==(const monitor_minidump& rhs) const
{
	if (!(dumpdata == rhs.dumpdata))
		return false;

	if (hostdata != rhs.hostdata)
		return false;

	return true;
}

void monitor_minidump::xml_create()
{
	std::string dumpstr;
	to_string_dumpdata(dumpdata, dumpstr);

	std::string hoststr;
	to_string_hostdata(hostdata, hoststr);

	xml_append_root(MONITOR_MINIDUMP_ROOT);
    xml_append_node("Dump", dumpstr, std::string(""));
    xml_append_node("Hosts", hoststr, std::string(""));
}

void monitor_minidump::xml_catch_value(const char *keyname, const char *keyvalue)
{
	if (strcmp(keyname, "Dump") == 0)
	{
		from_string_dumpdata(keyvalue, dumpdata);
	}

	if (strcmp(keyname, "Hosts") == 0)
	{
		from_string_hostdata(keyvalue, hostdata);
	}
}

void monitor_minidump::to_string_dumpdata(const monitor_miniresult_container& dump, std::string& dumpstr)
{
	dumpstr.clear();
	size_t total_items = dump.size();
	size_t item_size = sizeof(unsigned int) + sizeof(monitor_miniresult);
	size_t total_size = total_items * item_size;

	unsigned char *data = (unsigned char *)malloc(total_size);
	memset(data, 0, total_size);

	unsigned char *p = data;
	
	unsigned int addr;
	monitor_miniresult_container::const_iterator iter;
	for (iter = dump.begin(); iter != dump.end(); ++iter)
	{
		addr = iter->first;
		memcpy(p, &addr, sizeof(unsigned int)); p += sizeof(unsigned int);
		memcpy(p, &(iter->second), sizeof(monitor_miniresult)); p += sizeof(monitor_miniresult);
	}

	utm::base64::encode(data, total_size, dumpstr);
	free(data);
}

bool monitor_minidump::from_string_dumpdata(const char* dumpstr, monitor_miniresult_container& dump)
{
	std::vector<unsigned char> data;
	utm::base64::decode(dumpstr, data);

	size_t item_size = sizeof(unsigned int) + sizeof(monitor_miniresult);
	size_t total_size = data.size();
	size_t total_items = total_size / item_size;

	if (total_size != (item_size * total_items))
		false; // corrupted data

	const unsigned char* p = data.data();

	unsigned int addr;
	monitor_miniresult mr;
	while(total_size > 0)
	{
		memcpy(&addr, p, sizeof(unsigned int)); p += sizeof(unsigned int);
		memcpy(&mr, p, sizeof(monitor_miniresult)); p += sizeof(monitor_miniresult);
		dump.insert(std::make_pair(addr, mr));
		total_size -= item_size;
	}

	return true;
}

void monitor_minidump::to_string_hostdata(const monitor_ip2host_container& dump, std::string& hoststr)
{
	hoststr.clear();
	hoststr.reserve(dump.size() * 64);

	monitor_ip2host_container::const_iterator iter;
	for (iter = dump.begin(); iter != dump.end(); ++iter)
	{
		addrip_v4 addr(iter->first);
		
		if (!hoststr.empty())
			hoststr.append("|");

		hoststr.append(addr.to_string());
		hoststr.append(",");
		hoststr.append(iter->second.getutf8());
	}
}

void monitor_minidump::from_string_hostdata(const char* hoststr, monitor_ip2host_container& dump)
{
	std::list<std::string> itemstr;
	stringtools::split(itemstr, hoststr, '|');

	size_t dpos;
	std::string addr;
	gstring host;
	addrip_v4 a;
	for (std::list<std::string>::iterator iter = itemstr.begin(); iter != itemstr.end(); ++iter)
	{
		dpos = iter->find(',');
		if (dpos != std::string::npos)
		{
			addr = iter->substr(0, dpos);
			a.from_string(addr.c_str());
			host.assign_fromutf8(iter->c_str() + dpos + 1);
			dump.insert(std::make_pair(a.m_addr, host));
		}
	}
}

void monitor_minidump::test_fillparams(int test_num)
{
	clear();

	{
		addrip_v4 a1("192.168.255.255");
		addrmac m1("9a:8b:7c:6d:5e:4f");
		monitor_miniresult mm1;
		{
			memset(&mm1, 0, sizeof(mm1));
			mm1.lastReplyDt = 0xFFFFFF00;
			mm1.nRangeId = 1;
			mm1.nRrCounter = 777;
			mm1.nStatus = 2;
			memcpy(mm1.mac, m1.get(), MAX_ETHER_LENGTH);
		}
		dumpdata.insert(std::make_pair(a1.m_addr, mm1));

		addrip_v4 a2("204.222.0.255");
		addrmac m2("1a:2b:3c:4d:5e:6f");
		monitor_miniresult mm2;
		{
			memset(&mm2, 0, sizeof(mm2));
			mm2.lastReplyDt = 0x770F0F0F;
			mm2.nRangeId = 2;
			mm2.nRrCounter = 666;
			mm2.nStatus = 3;
			memcpy(mm2.mac, m2.get(), MAX_ETHER_LENGTH);
		}
		dumpdata.insert(std::make_pair(a2.m_addr, mm2));
	}

	{
		addrip_v4 a1("192.168.0.255");
		hostdata.insert(std::make_pair(a1.m_addr, gstring("www.w3.org")));
		addrip_v4 a2("10.30.111.1");
		hostdata.insert(std::make_pair(a2.m_addr, gstring("www.organization.com")));
	}
}

void monitor_minidump::test_all()
{
	test_case::classname.assign(this_class_name);

	TEST_CASE_CHECK(sizeof(monitor_miniresult), size_t(20));
	monitor_minidump minidump;
	minidump.test_fillparams();

	std::string dumpstr;
	{
		monitor_minidump::to_string_dumpdata(minidump.dumpdata, dumpstr);
		TEST_CASE_CHECK(dumpstr, std::string("//+owAD///8BAAAAAgAAAAkDmot8bV5P/wDezA8PD3cCAAAAAwAAAJoCGis8TV5v"));
	}

	{
		monitor_miniresult_container mini;
		monitor_minidump::from_string_dumpdata(dumpstr.c_str(), mini);

		TEST_CASE_CHECK(mini.size(), size_t(2));

		monitor_miniresult_container::iterator iter = mini.begin();
		addrip_v4 a1(iter->first);
		TEST_CASE_CHECK(a1.to_string(), std::string("192.168.255.255"));
		addrmac m1; m1.set(iter->second.mac); std::string c1 = m1.to_string();
		TEST_CASE_CHECK(c1, std::string("9a:8b:7c:6d:5e:4f"));
		TEST_CASE_CHECK(iter->second.lastReplyDt, 0xFFFFFF00);

		++iter;
		TEST_CASE_CHECK(iter->second.lastReplyDt, unsigned int(0x770F0F0F));
	}

	std::string hoststr;
	{
		monitor_minidump::to_string_hostdata(minidump.hostdata, hoststr);
		TEST_CASE_CHECK(hoststr, std::string("10.30.111.1,www.organization.com|192.168.0.255,www.w3.org"));
	}

	{
		monitor_ip2host_container hosts;
		monitor_minidump::from_string_hostdata(hoststr.c_str(), hosts);

		TEST_CASE_CHECK(hosts.size(), size_t(2));

		monitor_ip2host_container::iterator iter = hosts.begin();
		addrip_v4 a1(iter->first);
		TEST_CASE_CHECK(a1.to_string(), std::string("10.30.111.1"));
		TEST_CASE_CHECK(iter->second, gstring("www.organization.com"));

		addrip_v4 a2((++iter)->first);
		TEST_CASE_CHECK(a2.to_string(), std::string("192.168.0.255"));
		TEST_CASE_CHECK(iter->second, gstring("www.w3.org"));
	}

	return;
}

}