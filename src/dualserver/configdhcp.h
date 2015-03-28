#pragma once

#include <ubase.h>
#include <addrip_v4.h>
#include <addrpair.h>
#include <addrmac.h>
#include <gstring.h>

#include <string>
#include <vector>
#include <map>
#include <boost/thread/mutex.hpp>

#include "configdhcp_statrecord.h"

#define CONFIGDHCP_XMLTAG_DHCP "DhcpSettings"
#define CONFIGDHCP_XMLTAG_ENABLED "DhcpEnabled"
#define CONFIGDHCP_XMLTAG_RANGE "Range"
#define CONFIGDHCP_XMLTAG_ROUTER "Router"
#define CONFIGDHCP_XMLTAG_MASK "Mask"
#define CONFIGDHCP_XMLTAG_DNS "Dns"
#define CONFIGDHCP_XMLTAG_STATIC_SORTMODE "StaticSort"

#define CONFIGDHCP_REGISTRYKEY_ENABLED "DhcpEnabled"
#define CONFIGDHCP_REGISTRYKEY_RANGE "DhcpRange"
#define CONFIGDHCP_REGISTRYKEY_ROUTER "DhcpOptionRouter"
#define CONFIGDHCP_REGISTRYKEY_MASK "DhcpOptionMask"
#define CONFIGDHCP_REGISTRYKEY_DNS "DhcpOptionDns"
#define CONFIGDHCP_REGISTRYKEY_STATIC_SORTMODE "DhcpStaticSort"
#define CONFIGDHCP_REGISTRYKEY_STATIC "DhcpStatic"

namespace utm {

template <typename T, typename U>
struct CompareByMember {
    // This is a pointer-to-member, it represents a member of class T
    // The data member has type U
    U T::*field;
    CompareByMember(U T::*f) : field(f) {}
    bool operator()(const T &lhs, const T &rhs) {
        return lhs.*field < rhs.*field;
    }
};

typedef std::vector<utm::configdhcp_statrecord> configdhcp_statrecord_container;
typedef std::vector<utm::configdhcp_statrecord>::iterator configdhcp_statrecord_iterator;
typedef std::vector<utm::configdhcp_statrecord>::const_iterator configdhcp_statrecord_constiterator;

class configdhcp : public ubase
{
public:
	static const char this_class_name[];

	enum statrecord_sortmodes
	{
		statrecord_sortmode_none = 0,
		statrecord_sortmode_mac = 1,
		statrecord_sortmode_ip = 2,
		statrecord_sortmode_comment = 3
	};

	configdhcp(void);
	configdhcp(const configdhcp& rhs);
	~configdhcp(void);

	configdhcp& operator=(const configdhcp& rhs);
	bool operator ==(const configdhcp& rhs) const;
	mutable boost::mutex guard_threadsafe_rw;

	bool enabled;
	addrpair<addrip_v4> range;
	addrip_v4 router;
	addrip_v4 mask;
	addrpair<addrip_v4> dns;

	int statrecord_sortmode;
	configdhcp_statrecord_container statrecords;

	void statrecords_sort();
	void statrecord_get(size_t index, configdhcp_statrecord& srecord);
	void statrecord_add(const configdhcp_statrecord& srecord, size_t insert_pos = INT_MAX, bool never_sort = false);
	void statrecord_replace(const configdhcp_statrecord& srecord, size_t index);
	void statrecord_delete(size_t index, bool never_sort = false);
	void statrecord_moveup(size_t index);
	void statrecord_movedown(size_t index);

	void clear();
	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue);
	ubase* xml_catch_subnode(const char *name) { return NULL; };

    static void threadsafe_copyobj(bool lock_src, const configdhcp& src, configdhcp& dst);

#ifdef UTM_WIN
	bool has_registry() { return true; };
	LONG ReadFromRegistry(const TCHAR *pRegistryPath, const HKEY hkey = HKEY_LOCAL_MACHINE);
	LONG SaveToRegistry(const TCHAR *pRegistryPath, const HKEY hkey = HKEY_LOCAL_MACHINE);
	void statrecords_to_xml(std::string& xml);
#endif

	void prepare_for_dualserver(std::string& dhcprange, std::string& dhcpoptions, configdhcp_statrecord_container dhcpstatrecords);

private:
	void prepare_range_for_dualserver(std::string& dhcprange);
	void prepare_options_for_dualserver(std::string& dhcpoptions);

public:
	static int test_get_testcases_number() { return 4; };
	void test_fillparams(int test_num);
};

}