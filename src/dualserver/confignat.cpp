#include "stdafx.h"
#include "confignat.h"

#include <iostream>
#include <string>
#include <vector>

#ifdef UTM_WIN
#include <RegistryHelper.h>
#include <gstring.h>
#endif

#include "confignat_portrdr_raw_base.h"

namespace utm {

const char confignat::this_class_name[] = "confignat";

confignat::confignat(void)
{
}

confignat::~confignat(void)
{
	portrdr.clear();
}

std::vector<std::string> confignat::create_portrdr_string() const
{
	std::vector<std::string> retval;

	confignat_portrdr_container::const_iterator iter;
	for (iter = portrdr.begin(); iter != portrdr.end(); ++iter)
	{
		std::ostringstream oss;
		addrip_v4 adr((*iter).remote_ip); 

		oss << (*iter).proto << "," << (*iter).public_port << "," << adr << "," << (*iter).remote_port;
		retval.push_back(oss.str());
	}

	return retval;
}

void confignat::parse_portrdr_string(const char *portrdr_string)
{
	if (portrdr_string == NULL)
		return;

	std::vector<std::string> fields;

	stringtools::split(fields, portrdr_string, ',');
	if (fields.size() == 4)
	{
		confignat_portrdr rdr;

		rdr.proto = atoi(fields[0].c_str());
		rdr.public_port = static_cast<unsigned short>(atoi(fields[1].c_str()));

		addrip_v4 adr(fields[2].c_str());
		rdr.remote_ip = adr.m_addr;

		rdr.remote_port = static_cast<unsigned short>(atoi(fields[3].c_str()));

		portrdr.push_back(rdr);
	}
}

#ifdef UTM_WIN
LONG confignat::ReadFromRegistry(const TCHAR *pRegistryPath, const HKEY hkey)
{
	clear();

	HKEY hk;
	LONG result = RegOpenKeyEx(hkey, pRegistryPath, 0, KEY_READ, &hk);

	if (ERROR_SUCCESS == result)
	{
		DWORD dw;

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueDword(hk, _T(CONFIGNAT_REGISTRYKEY_ENABLED), dw))
			enabled = dw > 0 ? true : false;

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueDword(hk, _T(CONFIGNAT_REGISTRYKEY_EXCLUDEFIRST), dw))
			exclude_first_filter = dw > 0 ? true : false;

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueDword(hk, _T(CONFIGNAT_REGISTRYKEY_AUTOPUBLIC), dw))
			auto_public = dw > 0 ? true : false;

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueDword(hk, _T(CONFIGNAT_REGISTRYKEY_PUBLICIP), dw))
			public_ip.m_addr = dw;

		DWORD datasize = 0;
		LPBYTE pdata = NULL;
		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueBinary(hk, _T(CONFIGNAT_REGISTRYKEY_PORTRDR), datasize, &pdata))
		{
			if (pdata != NULL)
			{
				DWORD elesize = sizeof(confignat_portrdr_raw_base);
				if ((datasize % elesize) == 0)
				{
					DWORD items = datasize / elesize;
					for (UINT i = 0; i < items; i++)
					{
						LPBYTE p = pdata + (i * elesize);
						confignat_portrdr_raw_base pr_raw;
						memcpy(&pr_raw, p, sizeof(confignat_portrdr_raw_base));

						confignat_portrdr pr;
						pr.proto = pr_raw.proto;
						pr.public_port = pr_raw.public_port;
						pr.remote_port = pr_raw.remote_port;
						pr.remote_ip = pr_raw.remote_ip;
						portrdr.push_back(pr);
					}
				}

				delete[] pdata;
			}
		}

		RegCloseKey(hk);
	}

	return result;
}

LONG confignat::SaveToRegistry(const TCHAR *pRegistryPath, const HKEY hkey)
{
	HKEY hk;
	LONG result = RegOpenKeyEx(hkey, pRegistryPath, 0, KEY_ALL_ACCESS, &hk);

	LONG res;
	DWORD dwEnabled = 1;

	if (result == ERROR_SUCCESS)
	{
		if (enabled)
			res = RegSetValueEx(hk, _T(CONFIGNAT_REGISTRYKEY_ENABLED), 0, REG_DWORD, (LPBYTE)&dwEnabled, sizeof(DWORD));
		else
			res = ::RegDeleteValue(hk, _T(CONFIGNAT_REGISTRYKEY_ENABLED));

		if (exclude_first_filter)
			res = RegSetValueEx(hk, _T(CONFIGNAT_REGISTRYKEY_EXCLUDEFIRST), 0, REG_DWORD, (LPBYTE)&dwEnabled, sizeof(DWORD));
		else
			res = ::RegDeleteValue(hk, _T(CONFIGNAT_REGISTRYKEY_EXCLUDEFIRST));

		if (auto_public)
			res = RegSetValueEx(hk, _T(CONFIGNAT_REGISTRYKEY_AUTOPUBLIC), 0, REG_DWORD, (LPBYTE)&dwEnabled, sizeof(DWORD));
		else
			res = ::RegDeleteValue(hk, _T(CONFIGNAT_REGISTRYKEY_AUTOPUBLIC));

		if (!public_ip.is_default())
			res = RegSetValueEx(hk, _T(CONFIGNAT_REGISTRYKEY_PUBLICIP), 0, REG_DWORD, (LPBYTE)&public_ip.m_addr, sizeof(DWORD));
		else
			res = ::RegDeleteValue(hk, _T(CONFIGNAT_REGISTRYKEY_PUBLICIP));

		if (portrdr.size() > 0)
		{
			DWORD datasize = portrdr.size() * sizeof(confignat_portrdr_raw_base);
			confignat_portrdr_raw_base* praw = (confignat_portrdr_raw_base *)malloc(datasize);
			confignat_portrdr_raw_base* p = praw;

			for (auto iter = portrdr.begin(); iter != portrdr.end(); ++iter, p++)
			{
				p->proto = iter->proto;
				p->public_port = iter->public_port;
				p->remote_port = iter->remote_port;
				p->remote_ip = iter->remote_ip;
			}

			res = RegSetValueEx(hk, _T(CONFIGNAT_REGISTRYKEY_PORTRDR), 0, REG_BINARY, (BYTE *)praw, datasize);
		}
		else
			res = ::RegDeleteValue(hk, _T(CONFIGNAT_REGISTRYKEY_PORTRDR));

		RegCloseKey(hk);
	}

	return result;
}
#endif


#ifdef UTM_DEBUG
void confignat::test_fillparams(int test_num)
{
	clear();

	enabled = true;
	exclude_first_filter = true;
	auto_public = 1;
	public_ip.from_string("102.203.98.12");

	confignat_portrdr r1;
	r1.proto = 6;
	r1.public_port = 2;
	r1.remote_ip = 9876554;
	r1.remote_port = 65535;

	confignat_portrdr r2;
	r2.proto = 17;
	r2.public_port = 65535;
	r2.remote_ip = 19876554;
	r2.remote_port = 123;

	portrdr.push_back(r1);
	portrdr.push_back(r2);

	return;
}
#endif

}