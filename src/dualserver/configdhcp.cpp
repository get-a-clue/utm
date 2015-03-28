#include "stdafx.h"

#include "configdhcp.h"
#include "configdhcp.h"

#include <string>
#include <ostream>
#include <iomanip>

#ifdef UTM_WIN
#include <RegistryHelper.h>
#endif

namespace utm {

const char configdhcp::this_class_name[] = "configdhcp";

configdhcp::configdhcp(void)
{
	clear();
}

configdhcp::configdhcp(const configdhcp& rhs)
{
	operator=(rhs);
}

configdhcp::~configdhcp(void)
{
}

void configdhcp::clear()
{
	enabled = false;
	range.clear();
	router.clear();
	mask.clear();
	dns.clear();
	statrecord_sortmode = (int)statrecord_sortmode_none;
	statrecords.clear();
}

configdhcp& configdhcp::operator=(const configdhcp& rhs)
{
	enabled = rhs.enabled;
	range = rhs.range;
	router = rhs.router;
	mask = rhs.mask;
	dns = rhs.dns;
	statrecord_sortmode = rhs.statrecord_sortmode;
	statrecords = rhs.statrecords;
	return *this;
}

bool configdhcp::operator==(const configdhcp& rhs) const
{
	if (enabled != rhs.enabled)
		return false;

	if (range != rhs.range)
		return false;

	if (router != rhs.router)
		return false;

	if (mask != rhs.mask)
		return false;

	if (dns != rhs.dns)
		return false;

	if (statrecord_sortmode != rhs.statrecord_sortmode)
		return false;

	if (statrecords.size() != rhs.statrecords.size())
		return false;

	configdhcp_statrecord_constiterator iter;
	configdhcp_statrecord_constiterator iter2 = rhs.statrecords.begin();
	for (iter = statrecords.begin(); iter != statrecords.end(); ++iter, ++iter2)
	{
		if (!((*iter) == (*iter2)))
			return false;
	}

	return true;
}

void configdhcp::threadsafe_copyobj(bool lock_src, const configdhcp& src, configdhcp& dst)
{
    if (lock_src)
        boost::mutex::scoped_lock lock(src.guard_threadsafe_rw);
    else
        boost::mutex::scoped_lock lock(dst.guard_threadsafe_rw);
    
    dst = src;
}


void configdhcp::statrecords_sort()
{
	if (statrecord_sortmode == (int)statrecord_sortmode_ip)
	{
		std::sort(statrecords.begin(), statrecords.end(),
			CompareByMember<configdhcp_statrecord, addrip_v4>(&configdhcp_statrecord::ip));
	}

	if (statrecord_sortmode == (int)statrecord_sortmode_mac)
	{
		std::sort(statrecords.begin(), statrecords.end(),
			CompareByMember<configdhcp_statrecord, addrmac>(&configdhcp_statrecord::mac));
	}

	if (statrecord_sortmode == (int)statrecord_sortmode_comment)
	{
		std::sort(statrecords.begin(), statrecords.end(),
			CompareByMember<configdhcp_statrecord, gstring>(&configdhcp_statrecord::comment));
	}
}

void configdhcp::xml_create()
{
	xml_append_root(CONFIGDHCP_XMLTAG_DHCP);
	xml_append_node(CONFIGDHCP_XMLTAG_ENABLED, enabled);
	xml_append_node(CONFIGDHCP_XMLTAG_RANGE, range.to_string());
	xml_append_node(CONFIGDHCP_XMLTAG_ROUTER, router);
	xml_append_node(CONFIGDHCP_XMLTAG_MASK, mask);
	xml_append_node(CONFIGDHCP_XMLTAG_DNS, dns.to_string(',', true));
	xml_append_node(CONFIGDHCP_XMLTAG_STATIC_SORTMODE, statrecord_sortmode);

	std::vector<configdhcp_statrecord>::iterator iter;
	for (iter = statrecords.begin(); iter != statrecords.end(); ++iter)
	{
		iter->xml_create();
		xml_append_child(iter->get_first_child());
	}
}

void configdhcp::xml_catch_value(const char *keyname, const char *keyvalue)
{
	std::string tmp;

	while(1)
	{
		if (xml_check_value(keyname, CONFIGDHCP_XMLTAG_ENABLED, keyvalue, enabled)) break;
		if (xml_check_value(keyname, CONFIGDHCP_XMLTAG_RANGE, keyvalue, tmp))
		{
			range.from_string(tmp.c_str());
			break;
		}

		if (xml_check_value(keyname, CONFIGDHCP_XMLTAG_ROUTER, keyvalue, router)) break;
		if (xml_check_value(keyname, CONFIGDHCP_XMLTAG_MASK, keyvalue, mask)) break;

		if (xml_check_value(keyname, CONFIGDHCP_XMLTAG_DNS, keyvalue, tmp))
		{
			dns.from_string(tmp.c_str(), ',', true);
			break;
		};

		if (xml_check_value(keyname, CONFIGDHCP_XMLTAG_STATIC_SORTMODE, keyvalue, statrecord_sortmode)) break;

		if (xml_check_value(keyname, CONFIGDHCP_XMLTAG_DHCPSTATRECORD, keyvalue, tmp))
		{
			configdhcp_statrecord cds;
			cds.xml_catch_value(keyname, keyvalue);
			statrecords.push_back(cds);
			break;
		};

		break;
	}
}

void configdhcp::prepare_for_dualserver(std::string& dhcprange, std::string& dhcpoptions, configdhcp_statrecord_container dhcpstatrecords)
{
	prepare_range_for_dualserver(dhcprange);
	prepare_options_for_dualserver(dhcpoptions);
	dhcpstatrecords = statrecords;
}

void configdhcp::prepare_range_for_dualserver(std::string& dhcprange)
{
	dhcprange.clear();

	std::ostringstream oss;

	if (!range.is_default())
	{
		oss << "DHCP_Range=" << range.to_string('-') << "\r\n";
		dhcprange = oss.str();
	}
}

void configdhcp::prepare_options_for_dualserver(std::string& dhcpoptions)
{
	dhcpoptions.clear();

	std::ostringstream oss;

	if (!router.is_default())
		oss << "Router=" << router.to_string() << "\r\n";

	if (!mask.is_default())
		oss << "SubNet_Mask=" << mask.to_string() << "\r\n";

	if (!dns.is_default())
		oss << "DNS_Server=" << dns.to_string(',', true) << "\r\n";

	dhcpoptions = oss.str();
}

void configdhcp::statrecord_get(size_t index, configdhcp_statrecord& srecord)
{
	size_t k = 0;

	configdhcp_statrecord_container::iterator iter;
	for (iter = statrecords.begin(); iter != statrecords.end(); ++iter)
	{
		if (index == k)
		{
			srecord = *iter;
			break;
		}
		k++;
	}

	if (index != k)
	{
		std::ostringstream oss;
		oss << "The record (index=" << index << ") is not found.";
		throw std::exception(oss.str().c_str());
	}
}

void configdhcp::statrecord_add(const configdhcp_statrecord& srecord, size_t insert_pos, bool never_sort)
{
	if (statrecords.size() < insert_pos)
		statrecords.push_back(srecord);
	else
	{
		configdhcp_statrecord_container::iterator iter = statrecords.begin();
		while (insert_pos > 0)
		{
			++iter;
			insert_pos--;
		}

		statrecords.insert(iter, srecord);
	}

	if (!never_sort)
	{
		statrecords_sort();
	}
}

void configdhcp::statrecord_replace(const configdhcp_statrecord& srecord, size_t index)
{
	statrecord_add(srecord, index, true);
	statrecord_delete(index + 1, true);
	statrecords_sort();
}

void configdhcp::statrecord_delete(size_t index, bool never_sort)
{
	configdhcp_statrecord_container::iterator iter;
	for (iter = statrecords.begin(); iter != statrecords.end(); ++iter)
	{
		if (index == 0)
		{
			statrecords.erase(iter);
			break;
		}
		index--;
	}

	if (!never_sort)
	{
		statrecords_sort();
	}
}

void configdhcp::statrecord_moveup(size_t index)
{
	size_t recordscount = statrecords.size();

	if ((index == 0) || (recordscount < 2) || (index >= recordscount))
	{
		std::ostringstream oss;
		oss << "The record (index=" << index << ") cannot be moved up.";
		throw std::exception(oss.str().c_str());
	}

	index--;
	configdhcp_statrecord_container::iterator iter = statrecords.begin(); ++iter;
	configdhcp_statrecord_container::iterator iter2 = statrecords.begin();
	for(; iter != statrecords.end(); ++iter, ++iter2, index--)
	{
		if (index == 0)
			break;
	}

	std::swap(*iter, *iter2);
}

void configdhcp::statrecord_movedown(size_t index)
{
	size_t recordscount = statrecords.size();

	if ((recordscount < 2) || (index >= (recordscount - 1)))
	{
		std::ostringstream oss;
		oss << "The record (index=" << index << ") cannot be moved down.";
		throw std::exception(oss.str().c_str());
	}

	configdhcp_statrecord_container::iterator iter = statrecords.begin();
	configdhcp_statrecord_container::iterator iter2 = statrecords.begin(); ++iter2;
	for(; iter != statrecords.end(); ++iter, ++iter2, index--)
	{
		if (index == 0)
			break;
	}

	std::swap(*iter, *iter2);
}

#ifdef UTM_WIN
LONG configdhcp::ReadFromRegistry(const TCHAR *pRegistryPath, const HKEY hkey)
{
	clear();

	HKEY hk;
	LONG result = RegOpenKeyEx(hkey, pRegistryPath, 0, KEY_READ, &hk);

	if (ERROR_SUCCESS == result)
	{
		DWORD dw;
		TCHAR szOption[MAX_REGISTRY_VALUE];

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueDword(hk, _T(CONFIGDHCP_REGISTRYKEY_ENABLED), dw))
			enabled = dw > 0 ? true : false;

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueString(hk, _T(CONFIGDHCP_REGISTRYKEY_RANGE), sizeof(szOption), szOption))
		{
			utm::gstring tmp(szOption);
			range.from_string(tmp.getmb().c_str(), '-');
		}

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueString(hk, _T(CONFIGDHCP_REGISTRYKEY_ROUTER), sizeof(szOption), szOption))
		{
			utm::gstring tmp(szOption);
			router.from_string(tmp.getmb().c_str());
		}

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueString(hk, _T(CONFIGDHCP_REGISTRYKEY_MASK), sizeof(szOption), szOption))
		{
			utm::gstring tmp(szOption);
			mask.from_string(tmp.getmb().c_str());
		}

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueString(hk, _T(CONFIGDHCP_REGISTRYKEY_DNS), sizeof(szOption), szOption))
		{
			utm::gstring tmp(szOption);
			dns.from_string(tmp.getmb().c_str(), ',', true);
		}

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueDword(hk, _T(CONFIGDHCP_REGISTRYKEY_STATIC_SORTMODE), dw))
		{
			statrecord_sortmode = (int)dw;
		}

		if (ERROR_SUCCESS == utm::RegistryHelper::QueryValueString(hk, _T(CONFIGDHCP_REGISTRYKEY_STATIC), sizeof(szOption), szOption))
		{
			utm::gstring tmp(szOption);

			std::ostringstream oss;
			oss << "<" << CONFIGDHCP_XMLTAG_DHCP << ">" << tmp.getmb() << "</" << CONFIGDHCP_XMLTAG_DHCP << ">";
			std::string tmpxml = oss.str();

			configdhcp cdh;
			cdh.xml_parse(tmpxml.c_str());

			statrecords = cdh.statrecords;
		}

		RegCloseKey(hk);
	}

	return result;
}

LONG configdhcp::SaveToRegistry(const TCHAR *pRegistryPath, const HKEY hkey)
{
	HKEY hk;
	LONG result = RegOpenKeyEx(hkey, pRegistryPath, 0, KEY_ALL_ACCESS, &hk);

	LONG res;
	DWORD dwEnabled = 1;

	if (result == ERROR_SUCCESS)
	{
		if (enabled)
			res = RegSetValueEx(hk, _T(CONFIGDHCP_REGISTRYKEY_ENABLED), 0, REG_DWORD, (LPBYTE)&dwEnabled, sizeof(DWORD));
		else
			res = ::RegDeleteValue(hk, _T(CONFIGDHCP_REGISTRYKEY_ENABLED));

		if (!range.is_default())
		{
			std::string str = range.to_string('-');
			utm::RegistryHelper::SetValueString(hk, _T(CONFIGDHCP_REGISTRYKEY_RANGE), str);
		}
		else
			res = ::RegDeleteValue(hk, _T(CONFIGDHCP_REGISTRYKEY_RANGE));

		if (!router.is_default())
		{
			std::string str = router.to_string();
			utm::RegistryHelper::SetValueString(hk, _T(CONFIGDHCP_REGISTRYKEY_ROUTER), str);
		}
		else
			res = ::RegDeleteValue(hk, _T(CONFIGDHCP_REGISTRYKEY_ROUTER));

		if (!mask.is_default())
		{
			std::string str = mask.to_string();
			utm::RegistryHelper::SetValueString(hk, _T(CONFIGDHCP_REGISTRYKEY_MASK), str);
		}
		else
			res = ::RegDeleteValue(hk, _T(CONFIGDHCP_REGISTRYKEY_MASK));

		if (!dns.is_default())
		{
			std::string str = dns.to_string(',');
			utm::RegistryHelper::SetValueString(hk, _T(CONFIGDHCP_REGISTRYKEY_DNS), str);
		}
		else
			res = ::RegDeleteValue(hk, _T(CONFIGDHCP_REGISTRYKEY_DNS));

		if (statrecord_sortmode != (int)statrecord_sortmode_none)
			res = RegSetValueEx(hk, _T(CONFIGDHCP_REGISTRYKEY_STATIC_SORTMODE), 0, REG_DWORD, (LPBYTE)&statrecord_sortmode, sizeof(DWORD));
		else
			res = ::RegDeleteValue(hk, _T(CONFIGDHCP_REGISTRYKEY_STATIC_SORTMODE));

		if (!statrecords.empty())
		{
			std::string str;
			statrecords_to_xml(str);
			utm::RegistryHelper::SetValueString(hk, _T(CONFIGDHCP_REGISTRYKEY_STATIC), str);
		}
		else
			res = ::RegDeleteValue(hk, _T(CONFIGDHCP_REGISTRYKEY_STATIC));

		RegCloseKey(hk);
	}

	return result;
}

void configdhcp::statrecords_to_xml(std::string& xml)
{
	xml_init();
	xml_append_root(CONFIGDHCP_XMLTAG_DHCP);

	std::vector<configdhcp_statrecord>::iterator iter;
	for (iter = statrecords.begin(); iter != statrecords.end(); ++iter)
	{
		iter->xml_create();
		xml_append_child(iter->get_first_child());
	}

	std::string str;
	xml_get_string(str, false);

	std::string::size_type pos_start = str.find(CONFIGDHCP_XMLTAG_DHCPSTATRECORD_OPEN);
	std::string::size_type pos_end = str.rfind(CONFIGDHCP_XMLTAG_DHCPSTATRECORD_CLOSE);

	if ((pos_start == std::string::npos) || (pos_end == std::string::npos))
		return;

	xml = str.substr(pos_start, pos_end - pos_start + sizeof(CONFIGDHCP_XMLTAG_DHCPSTATRECORD_CLOSE));
}

#endif

void configdhcp::test_fillparams(int test_num)
{
	clear();

	enabled = true;
	range = addrpair<addrip_v4>(addrip_v4("192.168.1.2"), addrip_v4("192.168.1.128"));
	router.from_string("192.168.1.1");
	mask.from_string("255.255.255.0");

	if (test_num == 0)
		dns = addrpair<addrip_v4>(addrip_v4("8.8.8.8"), addrip_v4("8.8.4.4"));

	if (test_num == 1)
		dns = addrpair<addrip_v4>(addrip_v4("8.8.8.8"), addrip_v4("0.0.0.0"));

	if (test_num == 2)
		dns = addrpair<addrip_v4>(addrip_v4("0.0.0.0"), addrip_v4("8.8.4.4"));

	if (test_num == 3)
	{
		dns = addrpair<addrip_v4>(addrip_v4("8.8.8.8"), addrip_v4("8.8.4.4"));
		statrecord_sortmode = statrecord_sortmode_ip;
	}

	for (size_t i = 0; i < 256; i++)
	{
		std::ostringstream oss_ip; oss_ip << "192.168.1." << i;
		std::ostringstream oss_mac; oss_mac << "01:02:03:04:05:" << std::hex << std::setfill('0') << std::setw(2) << i;
		std::ostringstream oss_comment; oss_comment << "Comment for " << i;

		configdhcp_statrecord sr;
		sr.ip.from_string(oss_ip.str().c_str());
		sr.mac.from_string(oss_mac.str().c_str());
		sr.comment.assign(oss_comment.str().c_str());
		statrecord_add(sr);
	}

	return;
}

}