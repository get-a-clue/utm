#pragma once

#if defined(_MSC_VER)
#define UTM_WIN
#endif

#ifdef UTM_WIN

#include <Windows.h>
#include <tchar.h>

#endif

#include "configdns_base.h"
#include <addrtablebase.h>

#define CONFIGDNS_REGISTRYKEY_ENABLED "DnsEnabled"
#define CONFIGDNS_REGISTRYKEY_FWDSRV1 "DnsForwardingServer"
#define CONFIGDNS_REGISTRYKEY_FWDSRV2 "DnsForwardingServer2"
#define CONFIGDNS_REGISTRYKEY_ALLOWEDHOSTS "DnsExtraAllowedHosts"

namespace utm {

class configdns : public configdns_base
{
public:
	configdns(void);
	~configdns(void);

    std::string create_allowedhosts_string() const;
    void parse_allowedhosts_string(const char *hosts);

public:
#ifdef UTM_WIN
	bool has_registry() { return true; };
	LONG ReadFromRegistry(const TCHAR *pRegistryPath, const HKEY hkey = HKEY_LOCAL_MACHINE);
	LONG SaveToRegistry(const TCHAR *pRegistryPath, const HKEY hkey = HKEY_LOCAL_MACHINE);
#endif

	void prepare_fwd_servers_for_dualserver(std::string& servers);
	void prepare_allowed_hosts_for_dualserver(const addrtablebase<addrip_v4>& locals, std::string& hosts);

	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num);
};

}

