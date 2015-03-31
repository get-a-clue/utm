// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "trafficreport_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char trafficreport_base::this_class_name[] = "trafficreport_base";

trafficreport_base::trafficreport_base()
{
    clear();
}

trafficreport_base::trafficreport_base(const trafficreport_base& rhs)
{
    operator=(rhs);
}

trafficreport_base::trafficreport_base(const trafficreport_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

trafficreport_base::~trafficreport_base()
{
}

trafficreport_base& trafficreport_base::operator=(const trafficreport_base& rhs)
{
    filters = rhs.filters;
    return *this;
}

bool trafficreport_base::operator==(const trafficreport_base& rhs) const
{
    if (!(filters == rhs.filters)) return false;

    return true;
}

void trafficreport_base::clear()
{
    filters.clear();
}

void trafficreport_base::xml_create()
{
    trafficreport_base orig;

    xml_append_root( TR_TRAFFICREPORT_XMLTAG_ROOT);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("Filter", get_filters());
}

void trafficreport_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "Filter", keyvalue, boost::bind(&trafficreport_base::parse_filter_string,this,_1))) return;
}



#ifdef UTM_WIN

LONG trafficreport_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG trafficreport_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
