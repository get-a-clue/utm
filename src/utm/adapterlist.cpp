#include "stdafx.h"

#include "adapterinfo.h"
#include "adapterlist.h"

#include <algorithm>
#include <boost/algorithm/string.hpp>

//#include "pugixml.hpp"

namespace utm {

const char adapterlist::this_class_name[] = "adapterlist";

adapterlist::adapterlist()
{
	clear();
}

adapterlist::~adapterlist()
{
}

void adapterlist::assign(const adapterlist& adlist)
{
	adapters = adlist.adapters;
	capture_mode = adlist.capture_mode;
}

void adapterlist::clear()
{
	capture_mode = 1;
	adapters.clear();
}

bool adapterlist::equals(const ubase* rhs) const
{
	const adapterlist* p = dynamic_cast<const adapterlist *>(rhs);
	if (p == NULL)
	{
		return false;
	}

	return operator==(*p);
}

bool adapterlist::operator==(const adapterlist& rhs) const
{
	if (capture_mode != rhs.capture_mode)
		return false;

	return (adapters == rhs.adapters);
}

size_t adapterlist::size_selected() const
{
	size_t selected_count = 0;

	for (adapters_container::const_iterator iter = adapters.begin(); iter != adapters.end(); ++iter)
	{
		if (iter->is_selected)
			selected_count++;
	}

	return selected_count;
}

size_t adapterlist::size_public() const
{
	size_t public_count = 0;

	for (adapters_container::const_iterator iter = adapters.begin(); iter != adapters.end(); ++iter)
	{
		if (iter->alias == NICALIAS_PUBLIC)
			public_count++;
	}

	return public_count;
}

void adapterlist::xml_catch_value(const char *keyname, const char *keyvalue)
{
	xml_check_value(keyname, ADAPTERINFO_XMLTAG_CAPTUREMODE, keyvalue, this->capture_mode);
}

ubase* adapterlist::xml_catch_subnode(const char *keyname, const char *class_name)
{
	if (strcmp(keyname, ADAPTERINFO_XMLTAG_NIC) == 0)
	{
		ubase *u = insert_element_and_get_lastptr(adapters);
		return u;
	}

	return NULL;
}


void adapterlist::xml_create()
{
	xml_append_root(ADAPTERINFO_XMLTAG_NICLIST);

	xml_append_node(ADAPTERINFO_XMLTAG_CAPTUREMODE, capture_mode);
	xml_append_node(ADAPTERINFO_XMLTAG_MAXSELECTION, MAX_CAPTURE_THREADS);
	xml_append_node(ADAPTERINFO_XMLTAG_COUNT, adapters.size());

	for (adapters_container::iterator iter = adapters.begin(); iter != adapters.end(); ++iter)
	{
		iter->xml_create();
		xml_append_child(iter->get_first_child());
	}
}

int adapterlist::get_publicnic_index() const
{
	int retval = 0;
	for (adapters_container::const_iterator iter = adapters.begin(); iter != adapters.end(); ++iter)
	{
		if (iter->alias == NICALIAS_PUBLIC)
		{
			return retval;
		}
		retval++;
	}

	return PUBLIC_NIC_NOT_FOUND;
}

void adapterlist::AddNetflowAdapter()
{
	if (!HasNetflowAdapter())
	{
		adapterinfo ai;
		ai.alias = NICALIAS_NETFLOW;
		ai.medium = utm::adapterinfo::ADAPTER_MEDIUM_NETFLOW;
		ai.name.assign("\\DEVICE\\Cisco Netflow Collector Protocol v.5 (v.9)");
		ai.descr.assign("Cisco Netflow Collector Protocol v.5 (v.9)");

		adapters.push_back(ai);
	}
}

bool adapterlist::HasNetflowAdapter() const
{
	for (adapters_container::const_iterator iter = adapters.begin(); iter != adapters.end(); ++iter)
	{
		if (iter->alias == NICALIAS_NETFLOW)
			return true;
	}

	return false;
}

void adapterlist::DeleteNetflowAdapter()
{
	for (adapters_container::iterator iter = adapters.begin(); iter != adapters.end(); )
	{
		if (iter->alias == NICALIAS_NETFLOW)
		{
			iter = adapters.erase(iter);
		}
		else
			++iter;
	}
}

void adapterlist::RevalidateSelectedList(const adapterlist& selected, adapterlist& revalidated, bool include_only_selected) const
{
	revalidated.adapters.clear();
	revalidated.capture_mode = selected.capture_mode;

	for (adapters_container::const_iterator iter = adapters.begin(); iter != adapters.end(); ++iter)
	{
		if ((*iter).medium == adapterinfo::ADAPTER_MEDIUM_UNKNOWN)
		{
			continue;
		}

		bool found = false;

		adapters_container::const_iterator terra;
		for (terra = selected.adapters.begin(); terra != selected.adapters.end(); ++terra)
		{
			size_t pos = iter->name.find(terra->name.c_str(), 0);

			if (pos != std::string::npos)
			{
				// Убедимся, что адаптер оканчивается на искомую фразу.
				// Т.е. если выбран адаптер "NDISWANIP", то он не должен
				// срабатывать на "\DEVICE\NDISWANIPV6"

				if ((pos + terra->name.size()) == (iter->name.size()))
				{
					adapterinfo ai(*iter);
					ai.is_selected = true;
					ai.is_promiscuous = terra->is_promiscuous;
					ai.alias = terra->alias;
					revalidated.adapters.push_back(ai);
					found = true;
					break;
				}
			};						
		}

		if ((!found) && (!include_only_selected))
		{
			adapterinfo ai(*iter);
			revalidated.adapters.push_back(ai);
		}
	}
}

void adapterlist::ExtractSelectedList(adapterlist& selected) const
{
	selected.adapters.clear();
	for (adapters_container::const_iterator iter = adapters.begin(); iter != adapters.end(); ++iter)
	{
		if (iter->is_selected)
			selected.adapters.push_back(*iter);
	}

	selected.capture_mode = capture_mode;
}

#ifdef UTM_WIN

void adapterlist::FillFromTcpAdapterList(TCP_AdapterList *pAdList, PIP_ADAPTER_INFO ppai)
{
	AddNetflowAdapter();

	if ((pAdList == NULL) || (ppai == NULL))
		return;

	for (unsigned long i = 0; i < pAdList->m_nAdapterCount; i++)
	{
		adapterinfo ai;
		ai.original_index = i;
#ifdef UTM_WIN
		ai.handle = pAdList->m_nAdapterHandle[i];
#endif
		ai.medium = pAdList->m_nAdapterMediumList[i];
		ai.name.assign((const char *)pAdList->m_szAdapterNameList[i]);
		ai.mac_current.set(pAdList->m_czCurrentAddress[i]);

		std::string adaptername = ai.name.getmb();
		FindAdapterInfoByName(ppai, adaptername.c_str(), ai);
		adapters.push_back(ai);
	};
}

bool adapterlist::FindAdapterInfoByName(PIP_ADAPTER_INFO ppai, const char* szAdapterName, adapterinfo& ai) const
{
	if (szAdapterName == NULL)
		return false;

	if (NULL != strstr(szAdapterName, "NDISWANIPV6"))
	{
		ai.descr.assign("NDISWANIPV6");
		return true;
	};

	if (NULL != strstr(szAdapterName, "NDISWANIP"))
	{
		ai.descr.assign("NDISWANIP");
		return true;
	};

	PIP_ADAPTER_INFO pai = ppai;
	while (pai != NULL)
	{
		if (pai->AdapterName[0] == '{')
		{
			if (NULL != strstr(szAdapterName, pai->AdapterName))
			{
				std::string descr(pai->Description);
				boost::algorithm::trim(descr);
				ai.descr.assign(descr);

				addrip_v4 ip((const char *)pai->IpAddressList.IpAddress.String);
				ai.ipaddr_v4 = ip.m_addr;

				addrip_v4 mask((const char *)pai->IpAddressList.IpMask.String);
				ai.mask_v4 = mask.m_addr;

				return true;
			};
		};

		pai = pai->Next;
	};

	if (adapterinfo::has_device_prefix(szAdapterName))
		ai.descr.assign(szAdapterName + DEVICE_PREFIX_LEN);

	return false;
}

// 
// Процедура получения внешнего IP адреса, необходимого
// для запуска NAT
//

std::vector<unsigned int> adapterlist::get_publicip(CSysNet *ps, DWORD desiredPublicIp)
{
	std::vector<unsigned int> ips;

	ps->DetectAdaptersIphlpapi();
	int adcount = ps->GetAPINumAdapters();

	adapters_container::iterator iter;
	for (iter = adapters.begin(); iter != adapters.end(); ++iter)
	{
		if (!iter->is_selected)
			continue;

		if (iter->alias != NICALIAS_PUBLIC)
			continue;

		if (iter->medium == 0)
		{
			// Public-адаптер - это Ethernet
			// Ищем данный адаптер с списке задетектированных адаптеров от SysInfo
			// Поиск будем производить по MAC-адресу

			for (int j = 0; j < adcount; j++)
			{
				byte *phw = ps->GetAPIAdapterHwAddressPtr(j);
				if (phw == NULL)
				{
					continue;
				};

				if (iter->mac_current.is_equal(phw))
				{
					// Мак-адреса совпали,
					// теперь получаем количество IP-адресов, которые привязаны к этому адаптеру

					int nIpCount = ps->GetAPINumAddresses(j);
					for (int y = 0; y < nIpCount; y++)
					{
						DWORD ip = ps->GetAPIAdapterIP(j, y);
//						if (ip == 0)
//							continue;

						ips.push_back(ip);
					}

				
					if ((ips.size() == 1) && (desiredPublicIp == 0))
					{
						// Если кол-во IP адресов не более 1
						// и установлено автоматическое распознавание внешнего IP,
						// то это нормальная ситуация :)
					}

					if (desiredPublicIp != 0)
					{
						// Если внешний IP адрес задан в настройках NAT, то он должен найтись в списке
						// IP-адресов внешнего адаптера.

						if (std::find(ips.begin(), ips.end(), desiredPublicIp) != ips.end())
						{
							ips.clear();
							ips.push_back(desiredPublicIp);
						}
						else
							ips.clear();
					}

					break;
				}
			}
		}
		else
		{
			// Публичный интерфейс - адаптер NDISWAN

			int nWanCount = 0;
			int nWanIndex = 0;

			// Подсчитываем количество активных PPP-соединений

			for (int j = 0; j < adcount; j++)
			{
				if (ps->GetAPIAdapterType(j) == MIB_IF_TYPE_PPP)
				{
					nWanCount++;
					nWanIndex = j;
				};
			}

			if (nWanCount == 0)
			{
				// активных WAN-адаптеров не обнаруженo,
				// следовательно публичный IP выдать не можем
			}
			else
			if ((nWanCount == 1) && (desiredPublicIp == 0))
			{
				// Если установлено автоматическое определение публичного адреса и WAN-адаптер - единственный,
				// то берем адрес этого адаптера в качестве публичного...

				DWORD ip = ps->GetAPIAdapterIP(nWanIndex, 0);
				if (ip != 0)
					ips.push_back(ip);
			}
			else
			if ((nWanCount > 1) && (desiredPublicIp == 0))
			{
				// WAN-адаптер не единственный и установлено автоматическое определение публичного адреса,
				// поэтому рассматриваем эту ситуацию как неразрешимую.
			}
			else
			{
				// Установлен принудительный публичный IP адрес, поэтому находим этот IP адрес
				// среди IP адресов всех WAN-адаптеров

				for (int w = 0; w < ps->GetAPINumAdapters(); w++)
				{
					if (ps->GetAPIAdapterType(w) != MIB_IF_TYPE_PPP)
						continue;

					int num = ps->GetAPINumAddresses(w);

					for (int t = 0; t < num; t++)
					{
						DWORD ip = ps->GetAPIAdapterIP(w, t);
						if (ip == 0)
							continue;

						ips.push_back(ip);
					}

					if (std::find(ips.begin(), ips.end(), desiredPublicIp) != ips.end())
					{
						ips.clear();
						ips.push_back(desiredPublicIp);
					}
					else
						ips.clear();
				}
			}
		}
	};

	return ips;
}

LONG adapterlist::ReadSelectedListFromRegistry(const TCHAR *pRegistryPath)
{
	return ReadSelectedListFromRegistry(HKEY_LOCAL_MACHINE, pRegistryPath);
}

LONG adapterlist::ReadSelectedListFromRegistry(HKEY hkey, const TCHAR *pRegistryPath)
{
	HKEY hk;
	LONG result = RegOpenKeyEx(hkey, pRegistryPath, 0, KEY_READ, &hk);

	if (result == ERROR_SUCCESS)
	{
		// Read selected network adapters from registry

		DWORD dw, dwType, dwSize; 
		TCHAR szKey[32];
		TCHAR szValue[MAX_PATH];

		for (int k = 0; k < MAX_CAPTURE_THREADS; k++)
		{
			_stprintf_s(szKey, 32, _T("NIC_Name%02d"), k);
			dwType = REG_SZ;
			dwSize = MAX_PATH;

			memset(szValue, 0, sizeof(szValue));
			result = RegQueryValueEx(hk, szKey, 0, &dwType, (LPBYTE)&szValue, &dwSize);
			if (ERROR_SUCCESS != result)
				break;

			adapterinfo ai;
			ai.name.assign(szValue);
			
			_stprintf_s(szKey, 32, _T("NIC_Promiscuous%02d"), k);
			dwType = REG_DWORD;
			dwSize = sizeof(DWORD);
			dw = 0;
			result = RegQueryValueEx(hk, szKey, 0, &dwType, (LPBYTE)&dw, &dwSize);
			if (ERROR_SUCCESS == result)
			{
				ai.is_promiscuous = (dw > 0) ? true : false;
			}

			_stprintf_s(szKey, 32, _T("NIC_Alias%02d"), k);
			dwType = REG_DWORD;
			dwSize = sizeof(DWORD);
			dw = 0;
			result = RegQueryValueEx(hk, szKey, 0, &dwType, (LPBYTE)&dw, &dwSize);
			if (ERROR_SUCCESS == result)
			{
				ai.alias = dw;
			}

			adapters.push_back(ai);
		};

		{
			dwType = REG_DWORD;
			dwSize = sizeof(DWORD);
			dw = 0;
			result = RegQueryValueEx(hk, _T("CaptureMode"), 0, &dwType, (LPBYTE)&dw, &dwSize);
			if (ERROR_SUCCESS == result)
			{
				capture_mode = dw;
			}
		}

		RegCloseKey(hk);
	}

	return result;
}

LONG adapterlist::SaveSelectedListToRegistry(const TCHAR *pRegistryPath)
{
	return SaveSelectedListToRegistry(HKEY_LOCAL_MACHINE, pRegistryPath);
}

LONG adapterlist::SaveSelectedListToRegistry(HKEY hkey, const TCHAR *pRegistryPath)
{
	HKEY hk;
	LONG result = RegOpenKeyEx(hkey, pRegistryPath, 0, KEY_ALL_ACCESS, &hk);

	TCHAR szKey[MAX_PATH];
	TCHAR szValue[MAX_PATH];
	DWORD dwValue;

	if (result == ERROR_SUCCESS)
	{
		unsigned int j = 0;

		adapters_container::iterator iter;
		for (iter = adapters.begin(); iter != adapters.end(); ++iter)
		{
			if (!iter->is_selected)
				continue;

			_stprintf_s(szKey, MAX_PATH, _T("NIC_Name%02d"), j);

			const TCHAR *pValue = iter->name.c_str();
			std::string adname = iter->name.getmb();
			if (adapterinfo::has_device_prefix(adname.c_str()))
				pValue = pValue + DEVICE_PREFIX_LEN;

			_tcsncpy_s(szValue, MAX_PATH, pValue, _TRUNCATE);

			DWORD dwSize = (DWORD)(_tcslen(szValue) * sizeof(TCHAR));

			result = RegSetValueEx(hk,
				szKey,
				0,
				REG_SZ,
				(LPBYTE)szValue,
				dwSize);

			if (ERROR_SUCCESS != result)
				break;

			_stprintf_s(szKey, MAX_PATH, _T("NIC_Promiscuous%02d"), j);
			dwValue = iter->is_promiscuous ? 1 : 0;

			result = RegSetValueEx(hk,
				szKey,
				0,
				REG_DWORD,
				(LPBYTE)&dwValue,
				sizeof(DWORD));

			if (ERROR_SUCCESS != result)
				break;

			_stprintf_s(szKey, MAX_PATH, _T("NIC_Alias%02d"), j);
			dwValue = (DWORD)iter->alias;

			result = RegSetValueEx(hk,
				szKey,
				0,
				REG_DWORD,
				(LPBYTE)&dwValue,
				sizeof(DWORD));

			if (ERROR_SUCCESS != result)
				break;

			j++;
		}

		if (ERROR_SUCCESS == result)
		{
			for (unsigned int i = j; i < MAX_CAPTURE_THREADS; i++)
			{
			 	_stprintf_s(szKey, MAX_PATH, _T("NIC_Name%02d"), i);
				RegDeleteValue(hk, szKey);

				_stprintf_s(szKey, MAX_PATH, _T("NIC_Promiscuous%02d"), i);
				RegDeleteValue(hk, szKey);

				_stprintf_s(szKey, MAX_PATH, _T("NIC_Alias%02d"), j);
				RegDeleteValue(hk, szKey);
			}
		}

		dwValue = (DWORD)capture_mode;
		result = RegSetValueEx(hk,
			_T("CaptureMode"),
			0,
			REG_DWORD,
			(LPBYTE)&dwValue,
			sizeof(DWORD));
	}

	RegCloseKey(hk);

	return result;
}

#endif

#ifdef UTM_DEBUG
void adapterlist::test_fillparams(int test_num)
{
	clear();

	capture_mode = 1;

	adapterinfo ai;
	ai.ipaddr_v4 = 98765;
	ai.mask_v4 = 321236789;
	ai.name.assign("\\device\\{012345-6789}");
	ai.descr.assign("Realtek test adapter");
	ai.mac_current.from_string("f1:f2:f3:f4:f5:f6");
	ai.mac_permanent.from_string("a1:a2:a3:a4:a5:a6");
	
	if (test_num == 1)
		ai.is_promiscuous = true;

	if (test_num == 2)
		ai.is_selected = true;

	adapters.push_back(ai);

	adapterinfo ai2;
	ai2.ipaddr_v4 = 321236789;
	ai2.mask_v4 = 98765;
	ai2.name.assign("\\device\\NDISWANIP");
	ai2.descr.assign("NDISWANIP");
	ai2.medium = utm::adapterinfo::ADAPTER_MEDIUM_RAS;
	ai2.alias = NICALIAS_PUBLIC;

	adapters.push_back(ai2);
}

void adapterlist::test_all()
{
	adapterlist ls;
	ls.test_fillparams(1);

	std::string x;
	ls.xml_create();
	ls.xml_get_string(x);

	return;
}

#endif
}