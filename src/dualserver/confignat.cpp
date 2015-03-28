#include "stdafx.h"
#include "confignat.h"

#include <iostream>
#include <string>
#include <vector>

#ifdef UTM_WIN
#include <RegistryHelper.h>
#include <gstring.h>
#endif

namespace utm {

confignat::confignat(void)
{
}

confignat::~confignat(void)
{
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

/*
void confignat::xml_catch_value(const char *keyname, const char *keyvalue)
{
	std::string tmp;

	while(1)
	{
		if (xml_check_value(keyname, CONFIGNAT_XMLTAG_ENABLED, keyvalue, enabled)) break;
		if (xml_check_value(keyname, CONFIGNAT_XMLTAG_EXCLUDEFIRST, keyvalue, exclude_first_filter)) break;
		if (xml_check_value(keyname, CONFIGNAT_XMLTAG_AUTOPUBLIC, keyvalue, auto_public)) break;
		if (xml_check_value(keyname, CONFIGNAT_XMLTAG_PUBLICIP, keyvalue, public_ip)) break;

		if (xml_check_value(keyname, CONFIGNAT_XMLTAG_PORTRDR, keyvalue, tmp))
		{
			std::vector<std::string> fields;

			stringtools::split(fields, tmp.c_str(), ',');
			if (fields.size() == 4)
			{
				confignat_portrdr_base rdr;

				rdr.proto = atoi(fields[0].c_str());
				rdr.public_port = static_cast<unsigned short>(atoi(fields[1].c_str()));

				addrip_v4 adr(fields[2].c_str());
				rdr.remote_ip = adr.m_addr;

				rdr.remote_port = static_cast<unsigned short>(atoi(fields[3].c_str()));

				portrdr.push_back(rdr);
			}
			break;
		};

		break;
	}
}
*/
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
				DWORD elesize = sizeof(confignat_portrdr);
				if ((datasize % elesize) == 0)
				{
					DWORD items = datasize / elesize;
					for (UINT i = 0; i < items; i++)
					{
						LPBYTE p = pdata + (i * elesize);
						confignat_portrdr pr;
						memcpy(&pr, p, sizeof(confignat_portrdr));
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
			BYTE* rawdata = (BYTE *)&(*(portrdr.begin()));
			DWORD datasize = portrdr.size() * sizeof(confignat_portrdr);
			res = RegSetValueEx(hk, _T(CONFIGNAT_REGISTRYKEY_PORTRDR), 0, REG_BINARY, (LPBYTE)rawdata, datasize);
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
}
#endif

}