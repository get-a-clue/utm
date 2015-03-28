#include "stdafx.h"

#include "configdns.h"

#include <vector>
#include <string>
#include <map>
#include <gstring.h>
#include <stringtools.h>

#ifdef UTM_WIN
#include <RegistryHelper.h>
#endif

namespace utm {

configdns::configdns(void)
{
}

configdns::~configdns(void)
{
}

std::string configdns::create_allowedhosts_string() const
{
	std::string hosts;

	std::vector<addrpair_v4>::const_iterator iter;
	for (iter = allowedhosts.begin(); iter != allowedhosts.end(); ++iter)
	{
		if (!hosts.empty())
			hosts.append(" ");

		addrpair<addrip_v4>& pp = addrpair<addrip_v4>(*iter);
		hosts.append(pp.to_string('-'));
	}

	return hosts;
}

void configdns::parse_allowedhosts_string(const char *hosts)
{
	bool raise_exception = false;
	allowedhosts.clear();

	std::string filtered;
	const char *r = hosts;
	char sz[2] = { 0, 0 };
	while (*r != 0)
	{
		if ((isdigit(*r)) || (*r == '.') || (*r == '-') || (*r == ' '))
		{
			sz[0] = *r;
			filtered.append(sz);
		}
		r++;
	}

	std::vector<std::string> strings;
	stringtools::split(strings, filtered, ' ', utm::split::no_empties);

	std::vector<std::string>::iterator iter;
	for (iter = strings.begin(); iter != strings.end(); ++iter)
	{
		addrpair_v4 pp;
		pp.from_string((*iter).c_str(), '-');
		if (!pp.is_default())
		{
			if (pp.addr1.m_addr > pp.addr2.m_addr)
			{
				if (raise_exception)
				{
					std::string exc("The following pair is invalid: ");
					exc += pp.to_string();
					throw exc;
				}
			}
			else
				allowedhosts.push_back(pp);
		}
	}
}

void configdns::prepare_fwd_servers_for_dualserver(std::string& servers)
{
	servers.clear();

	if (!fwd_server1.is_zero())
	{
		servers = fwd_server1.to_string();
	}

	if (!fwd_server2.is_zero())
	{
		std::string s2 = fwd_server2.to_string();

		if (!servers.empty())
			servers.append(" ");

		servers.append(s2);
	}
}

void configdns::prepare_allowed_hosts_for_dualserver(const addrtablebase<addrip_v4>& locals, std::string& hosts)
{
	hosts.clear();

	const std::map<addrip_v4, addrip_v4> ipmask = locals.itemsref();
	std::map<addrip_v4, addrip_v4>::const_iterator mapiter;
	for (mapiter = ipmask.begin(); mapiter != ipmask.end(); ++mapiter)
	{
		addrip_v4 startip;
		addrip_v4 endip;

		addrip_v4::convert_ipmask_to_range((*mapiter).first, (*mapiter).second, startip, endip);
		addrpair<addrip_v4> ap(startip, endip, '-');

		if (!hosts.empty())
			hosts.append(" ");

		hosts.append(ap.to_string());
	}

	if (!allowedhosts.empty())
	{
		std::vector<addrpair_v4>::iterator iter;
		for (iter = allowedhosts.begin(); iter != allowedhosts.end(); ++iter)
		{
			if (!hosts.empty())
				hosts.append(" ");

			hosts.append((*iter).to_string('-'));
		}
	}
}

#ifdef UTM_WIN
LONG configdns::ReadFromRegistry(const TCHAR *pRegistryPath, const HKEY hkey)
{
	clear();

	HKEY hk;
	LONG result = RegOpenKeyEx(hkey, pRegistryPath, 0, KEY_READ, &hk);

	if (ERROR_SUCCESS == result)
	{
		DWORD dw;
		TCHAR szAllowedHosts[MAX_REGISTRY_VALUE];

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueDword(hk, _T(CONFIGDNS_REGISTRYKEY_ENABLED), dw))
			enabled = dw > 0 ? true : false;

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueDword(hk, _T(CONFIGDNS_REGISTRYKEY_FWDSRV1), dw))
			fwd_server1.m_addr = dw;

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueDword(hk, _T(CONFIGDNS_REGISTRYKEY_FWDSRV2), dw))
			fwd_server2.m_addr = dw;

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueString(hk, _T(CONFIGDNS_REGISTRYKEY_ALLOWEDHOSTS), sizeof(szAllowedHosts), szAllowedHosts))
		{
			utm::gstring hosts(szAllowedHosts);
			parse_allowedhosts_string(hosts.getmb().c_str());
		}

		RegCloseKey(hk);
	}

	return result;
}

LONG configdns::SaveToRegistry(const TCHAR *pRegistryPath, const HKEY hkey)
{
	HKEY hk;
	LONG result = RegOpenKeyEx(hkey, pRegistryPath, 0, KEY_ALL_ACCESS, &hk);

	LONG res;
	DWORD dwEnabled = 1;

	if (result == ERROR_SUCCESS)
	{
		if (enabled)
			res = RegSetValueEx(hk, _T(CONFIGDNS_REGISTRYKEY_ENABLED), 0, REG_DWORD, (LPBYTE)&dwEnabled, sizeof(DWORD));
		else
			res = ::RegDeleteValue(hk, _T(CONFIGDNS_REGISTRYKEY_ENABLED));

		if (!fwd_server1.is_default())
			res = RegSetValueEx(hk, _T(CONFIGDNS_REGISTRYKEY_FWDSRV1), 0, REG_DWORD, (LPBYTE)&fwd_server1.m_addr, sizeof(DWORD));
		else
			res = ::RegDeleteValue(hk, _T(CONFIGDNS_REGISTRYKEY_FWDSRV1));

		if (!fwd_server2.is_default())
			res = RegSetValueEx(hk, _T(CONFIGDNS_REGISTRYKEY_FWDSRV2), 0, REG_DWORD, (LPBYTE)&fwd_server2.m_addr, sizeof(DWORD));
		else
			res = ::RegDeleteValue(hk, _T(CONFIGDNS_REGISTRYKEY_FWDSRV2));

		if (!allowedhosts.empty())
		{
			std::string ahstr = create_allowedhosts_string();
			utm::RegistryHelper::SetValueString(hk, _T(CONFIGDNS_REGISTRYKEY_ALLOWEDHOSTS), ahstr);
		}
		else
			res = ::RegDeleteValue(hk, _T(CONFIGDNS_REGISTRYKEY_ALLOWEDHOSTS));

		RegCloseKey(hk);
	}

	return result;
}
#endif


void configdns::test_fillparams(int test_num)
{
	clear();

	enabled = true;
	fwd_server1.from_string("8.8.8.8");
	fwd_server2.from_string("8.8.4.4");

	addrip_v4 a1("192.168.1.0");
	addrip_v4 a2("192.168.1.255");
	addrpair_v4 p1(a1, a2);

	addrip_v4 a21("192.168.2.0");
	addrip_v4 a22("192.168.2.255");
	addrpair_v4 p2(a21, a22);

	allowedhosts.push_back(p1);
	allowedhosts.push_back(p2);
}

}