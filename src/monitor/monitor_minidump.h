#ifndef UTM_MONITOR_MINIDUMP_H
#define UTM_MONITOR_MINIDUMP_H

#pragma once
#include <utm.h>

#include <map>

#include <gstring.h>
#include <ubase.h>

#define MONITOR_MINIDUMP_ROOT "MonitorMiniDump"

namespace utm {

struct monitor_miniresult 
{
	unsigned int lastReplyDt;
	unsigned int nRangeId;
	int		nStatus;
	unsigned short nRrCounter;
	unsigned char mac[6];

	bool operator==(const monitor_miniresult& rhs) const
	{
		if (lastReplyDt != rhs.lastReplyDt) return false;
		if (nRangeId != rhs.nRangeId) return false;
		if (nStatus != rhs.nStatus) return false;
		if (nRrCounter != rhs.nRrCounter) return false;
		
		return (memcmp(mac, rhs.mac, 6) == 0);
	}
};

typedef std::map<unsigned int, monitor_miniresult> monitor_miniresult_container;
typedef std::map<unsigned int, gstring> monitor_ip2host_container;

class monitor_minidump : public ubase
{
public:
	static const char this_class_name[];

	monitor_minidump(void);
	~monitor_minidump(void);

	bool equals(const ubase* rhs) const;
	bool operator==(const monitor_minidump& rhs) const;

	monitor_miniresult_container dumpdata;
	monitor_ip2host_container hostdata;

	static void to_string_dumpdata(const monitor_miniresult_container& dump, std::string& dumpstr);
	static bool from_string_dumpdata(const char* dumpstr, monitor_miniresult_container& dump);
	static void to_string_hostdata(const monitor_ip2host_container& dump, std::string& hoststr);
	static void from_string_hostdata(const char* hoststr, monitor_ip2host_container& dump);

	void clear();
    void xml_create();
    void xml_catch_value(const char *keyname, const char *keyvalue);
    virtual ubase* xml_catch_subnode(const char *tag_name, const char *classname) { return NULL; };


	static void test_all();
	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num = 1);
};

}

#endif // UTM_MONITOR_MINIDUMP_H
