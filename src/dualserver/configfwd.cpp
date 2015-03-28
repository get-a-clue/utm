#include "stdafx.h"
#include "configfwd.h"

#ifdef UTM_WIN
#include <RegistryHelper.h>
#endif

namespace utm {

configfwd::configfwd(void)
{
	clear();
}

configfwd::~configfwd(void)
{
}

#ifdef UTM_WIN
LONG configfwd::ReadFromRegistry(const TCHAR *pRegistryPath, const HKEY hkey)
{
	clear();

	HKEY hk;
	LONG result = RegOpenKeyEx(hkey, pRegistryPath, 0, KEY_READ, &hk);

	if (ERROR_SUCCESS == result)
	{
		DWORD dw;

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueDword(hk, _T(CONFIGFWD_REGISTRYKEY_IP), dw))
			fwd_ip.m_addr = dw;

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueDword(hk, _T(CONFIGFWD_REGISTRYKEY_PORT), dw))
			fwd_port = static_cast<unsigned short>(dw);

		RegCloseKey(hk);
	}

	return result;
}

LONG configfwd::SaveToRegistry(const TCHAR *pRegistryPath, const HKEY hkey)
{
	HKEY hk;
	LONG result = RegOpenKeyEx(hkey, pRegistryPath, 0, KEY_ALL_ACCESS, &hk);

	LONG res;
	DWORD dwPort = static_cast<DWORD>(fwd_port);

	if (result == ERROR_SUCCESS)
	{
		if (!fwd_ip.is_default())
			res = RegSetValueEx(hk, _T(CONFIGFWD_REGISTRYKEY_IP), 0, REG_DWORD, (LPBYTE)&fwd_ip.m_addr, sizeof(DWORD));
		else
			res = ::RegDeleteValue(hk, _T(CONFIGFWD_REGISTRYKEY_IP));

		if (fwd_port > 0)
			res = RegSetValueEx(hk, _T(CONFIGFWD_REGISTRYKEY_PORT), 0, REG_DWORD, (LPBYTE)&dwPort, sizeof(DWORD));
		else
			res = ::RegDeleteValue(hk, _T(CONFIGFWD_REGISTRYKEY_PORT));

		RegCloseKey(hk);
	}

	return result;
}
#endif

#ifdef UTM_DEBUG
void configfwd::test_fillparams(int test_num)
{
	clear();

	fwd_ip.from_string("202.103.192.96");
	fwd_port = 65535;
}
#endif

}
