#include "stdafx.h"

#include <string>
#include <ostream>

#include <SysNet.h>
#include "DualServer.h"
#include "configdns.h"
#include "configdhcp.h"
#include "confignat.h"
#include "configfwd.h"
#include <ubase_test.h>

void test()
{
#ifdef UTM_DEBUG
	utm::configdhcp cc;
	cc.CreateRegistry(NULL, NULL);

	utm::ubase_test<utm::configdns> test_configdns;
	test_configdns.test_all();

	utm::ubase_test<utm::configdhcp> test_configdhcp;
	test_configdhcp.test_all();

	utm::ubase_test<utm::confignat> test_confignat;
	test_confignat.test_all();

	utm::ubase_test<utm::configfwd> test_configfwd;
	test_configfwd.test_all();

	std::cout << "test passed!" << std::endl;
#endif
}

void PrintLogMess(char *logmess, char logLevel)
{
	std::cout << logmess << std::endl;
}

void test_ds()
{
	utm::CSysNet sn;
	sn.RefreshLocalAddresses();
	utm::addrtablebase<utm::addrip_v4> locals = sn.locals_ip_v4;

	utm::configdns cd;
	cd.test_fillparams(0);

	std::string allowed_hosts;
	cd.prepare_allowed_hosts_for_dualserver(locals, allowed_hosts);

    utm::CDualServer* pDnsReal = new utm::CDualServer();

	pDnsReal->m_bUseDnsCache = FALSE;

    pDnsReal->MySetCallbackLogMess((pCallbackLogMess)&PrintLogMess);
    pDnsReal->MySetDnsServerPort(53);
    pDnsReal->MySetDefaultLogLevel(2);

	pDnsReal->MySetDnsAllowedHosts((char *)allowed_hosts.c_str());

	std::string servers;
	cd.prepare_fwd_servers_for_dualserver(servers);

	pDnsReal->MySetDnsServers((char *)servers.c_str());
    pDnsReal->MyServiceDualMain();

    delete pDnsReal;
    pDnsReal = NULL;
}

int _tmain(int argc, _TCHAR* argv[])
{
	_tsetlocale(0, _T(""));

	try
	{
		while(1)
		{
			test();
			break;
		}
	}
	catch(const utm::test_exception& ex)
	{
		std::cout << ex.what() << std::endl;
		throw ex.what();
	}

//	test_ds();

}