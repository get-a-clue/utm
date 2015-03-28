// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "singledhcplease_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char singledhcplease_base::this_class_name[] = "singledhcplease_base";

singledhcplease_base::singledhcplease_base()
{
    clear();
}

singledhcplease_base::singledhcplease_base(const singledhcplease_base& rhs)
{
    operator=(rhs);
}

singledhcplease_base::singledhcplease_base(const singledhcplease_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

singledhcplease_base::~singledhcplease_base()
{
}

singledhcplease_base& singledhcplease_base::operator=(const singledhcplease_base& rhs)
{
    id = rhs.id;
    mac = rhs.mac;
    ipaddr = rhs.ipaddr;
    leasetime = rhs.leasetime;
    hostname = rhs.hostname;
    return *this;
}

bool singledhcplease_base::operator==(const singledhcplease_base& rhs) const
{
    if (!(id == rhs.id)) return false;
    if (!(mac == rhs.mac)) return false;
    if (!(ipaddr == rhs.ipaddr)) return false;
    if (!(leasetime == rhs.leasetime)) return false;
    if (!(hostname == rhs.hostname)) return false;

    return true;
}

void singledhcplease_base::clear()
{
    id = 0;
    mac.clear();
    ipaddr.clear();
    leasetime.clear();
    hostname.clear();
}

void singledhcplease_base::xml_create()
{
    singledhcplease_base orig;

    xml_append_root( SINGLEDHCPLEASE_XMLTAG_ROOT);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("Id", id, orig.id);
    xml_append_node("Mac", mac, orig.mac);
    xml_append_node("IPAddr", ipaddr, orig.ipaddr);
    xml_append_node("LeaseTime", leasetime, orig.leasetime);
    xml_append_node("Hostname", hostname, orig.hostname);
}

void singledhcplease_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "Id", keyvalue, id)) return;
    if (xml_check_value(keyname, "Mac", keyvalue, mac)) return;
    if (xml_check_value(keyname, "IPAddr", keyvalue, ipaddr)) return;
    if (xml_check_value(keyname, "LeaseTime", keyvalue, leasetime)) return;
    if (xml_check_value(keyname, "Hostname", keyvalue, hostname)) return;
}



#ifdef UTM_WIN

LONG singledhcplease_base::CreateRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_READ, &h);
    if (result == ERROR_FILE_NOT_FOUND)
    {
       result = RegCreateKeyEx(hk, pRegistryPath, NULL, NULL, NULL, KEY_ALL_ACCESS, NULL, &h, NULL);
    }

    if (result == ERROR_SUCCESS)
    {
       RegCloseKey(h);
    };

    return result;
}

LONG singledhcplease_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG singledhcplease_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_READ, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}
#endif // UTM_WIN

}
