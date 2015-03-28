// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "filtercons_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char filtercons_base::this_class_name[] = "filtercons_base";

filtercons_base::filtercons_base()
{
    clear();
}

filtercons_base::filtercons_base(const filtercons_base& rhs)
{
    operator=(rhs);
}

filtercons_base::filtercons_base(const filtercons_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

filtercons_base::~filtercons_base()
{
}

filtercons_base& filtercons_base::operator=(const filtercons_base& rhs)
{
    filter_id = rhs.filter_id;
    bytes_sent = rhs.bytes_sent;
    bytes_recv = rhs.bytes_recv;
    speed_array = rhs.speed_array;
    return *this;
}

bool filtercons_base::operator==(const filtercons_base& rhs) const
{
    if (!(filter_id == rhs.filter_id)) return false;
    if (!(bytes_sent == rhs.bytes_sent)) return false;
    if (!(bytes_recv == rhs.bytes_recv)) return false;
    if (!(speed_array == rhs.speed_array)) return false;

    return true;
}

void filtercons_base::clear()
{
    filter_id = 0;
    bytes_sent = 0;
    bytes_recv = 0;
    speed_array.clear();
}

void filtercons_base::xml_create()
{
    filtercons_base orig;

    xml_append_root( FILTERCONS_XMLTAG_FILTERCONS);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("I", filter_id, orig.filter_id);
    xml_append_node("S", bytes_sent, orig.bytes_sent);
    xml_append_node("R", bytes_recv, orig.bytes_recv);
    xml_append_node("SP", speed_array, orig.speed_array);
}

void filtercons_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "I", keyvalue, filter_id)) return;
    if (xml_check_value(keyname, "S", keyvalue, bytes_sent)) return;
    if (xml_check_value(keyname, "R", keyvalue, bytes_recv)) return;
    if (xml_check_value(keyname, "SP", keyvalue, speed_array)) return;
}



#ifdef UTM_WIN

LONG filtercons_base::CreateRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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

LONG filtercons_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG filtercons_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
