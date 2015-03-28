// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "trafficreport_filter_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char trafficreport_filter_base::this_class_name[] = "trafficreport_filter_base";

trafficreport_filter_base::trafficreport_filter_base()
{
    clear();
}

trafficreport_filter_base::trafficreport_filter_base(const trafficreport_filter_base& rhs)
{
    operator=(rhs);
}

trafficreport_filter_base::trafficreport_filter_base(const trafficreport_filter_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

trafficreport_filter_base::~trafficreport_filter_base()
{
}

trafficreport_filter_base& trafficreport_filter_base::operator=(const trafficreport_filter_base& rhs)
{
    sent = rhs.sent;
    recv = rhs.recv;
    name = rhs.name;
    color = rhs.color;
    dayticks = rhs.dayticks;
    return *this;
}

bool trafficreport_filter_base::operator==(const trafficreport_filter_base& rhs) const
{
    if (!(sent == rhs.sent)) return false;
    if (!(recv == rhs.recv)) return false;
    if (!(name == rhs.name)) return false;
    if (!(color == rhs.color)) return false;
    if (!(dayticks == rhs.dayticks)) return false;

    return true;
}

void trafficreport_filter_base::clear()
{
    sent = 0;
    recv = 0;
    name.clear();
    color = 0;
    dayticks.clear();
}

void trafficreport_filter_base::xml_create()
{
    trafficreport_filter_base orig;

    xml_append_root( TR_FILTER_XMLTAG_ROOT);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("Sent", sent, orig.sent);
    xml_append_node("Recv", recv, orig.recv);
    xml_append_node("Name", name, orig.name);
    xml_append_node("Color", color, orig.color);
    xml_append_node("Day", get_dayticks());
}

void trafficreport_filter_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "Sent", keyvalue, sent)) return;
    if (xml_check_value(keyname, "Recv", keyvalue, recv)) return;
    if (xml_check_value(keyname, "Name", keyvalue, name)) return;
    if (xml_check_value(keyname, "Color", keyvalue, color)) return;
    if (xml_check_value(keyname, "Day", keyvalue, boost::bind(&trafficreport_filter_base::parse_dayticks_string,this,_1))) return;
}



#ifdef UTM_WIN

LONG trafficreport_filter_base::CreateRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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

LONG trafficreport_filter_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG trafficreport_filter_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
