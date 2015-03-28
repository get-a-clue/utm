// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "filterstate_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char filterstate_base::this_class_name[] = "filterstate_base";

filterstate_base::filterstate_base()
{
    clear();
}

filterstate_base::filterstate_base(const filterstate_base& rhs)
{
    operator=(rhs);
}

filterstate_base::filterstate_base(const filterstate_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

filterstate_base::~filterstate_base()
{
}

filterstate_base& filterstate_base::operator=(const filterstate_base& rhs)
{
    filter_id = rhs.filter_id;
    filter_name = rhs.filter_name;
    bytes_sent = rhs.bytes_sent;
    bytes_recv = rhs.bytes_recv;
    limflags = rhs.limflags;
    lastreset = rhs.lastreset;
    return *this;
}

bool filterstate_base::operator==(const filterstate_base& rhs) const
{
    if (!(filter_id == rhs.filter_id)) return false;
    if (!(filter_name == rhs.filter_name)) return false;
    if (!(bytes_sent == rhs.bytes_sent)) return false;
    if (!(bytes_recv == rhs.bytes_recv)) return false;
    if (!(limflags == rhs.limflags)) return false;
    if (!(lastreset == rhs.lastreset)) return false;

    return true;
}

void filterstate_base::clear()
{
    filter_id = 0;
    filter_name.clear();
    bytes_sent = 0;
    bytes_recv = 0;
    limflags = 0;
    lastreset.clear();
}

void filterstate_base::xml_create()
{
    filterstate_base orig;

    xml_append_root( FILTERSTATE_XMLTAG_FILTER);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("Id", filter_id, orig.filter_id);
    xml_append_node("Name", filter_name, orig.filter_name);
    xml_append_node("Sent", bytes_sent, orig.bytes_sent);
    xml_append_node("Recv", bytes_recv, orig.bytes_recv);
    xml_append_node("LimFlags", limflags, orig.limflags);
    xml_append_node("LastReset", lastreset, orig.lastreset);
}

void filterstate_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "Id", keyvalue, filter_id)) return;
    if (xml_check_value(keyname, "Name", keyvalue, filter_name)) return;
    if (xml_check_value(keyname, "Sent", keyvalue, bytes_sent)) return;
    if (xml_check_value(keyname, "Recv", keyvalue, bytes_recv)) return;
    if (xml_check_value(keyname, "LimFlags", keyvalue, limflags)) return;
    if (xml_check_value(keyname, "LastReset", keyvalue, lastreset)) return;
}



#ifdef UTM_WIN

LONG filterstate_base::CreateRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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

LONG filterstate_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG filterstate_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
