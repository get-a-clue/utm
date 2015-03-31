// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "trafficreport_daytick_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char trafficreport_daytick_base::this_class_name[] = "trafficreport_daytick_base";

trafficreport_daytick_base::trafficreport_daytick_base()
{
    clear();
}

trafficreport_daytick_base::trafficreport_daytick_base(const trafficreport_daytick_base& rhs)
{
    operator=(rhs);
}

trafficreport_daytick_base::trafficreport_daytick_base(const trafficreport_daytick_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

trafficreport_daytick_base::~trafficreport_daytick_base()
{
}

trafficreport_daytick_base& trafficreport_daytick_base::operator=(const trafficreport_daytick_base& rhs)
{
    sent = rhs.sent;
    recv = rhs.recv;
    hourticks = rhs.hourticks;
    return *this;
}

bool trafficreport_daytick_base::operator==(const trafficreport_daytick_base& rhs) const
{
    if (!(sent == rhs.sent)) return false;
    if (!(recv == rhs.recv)) return false;
    if (!(hourticks == rhs.hourticks)) return false;

    return true;
}

void trafficreport_daytick_base::clear()
{
    sent = 0;
    recv = 0;
    hourticks.clear();
}

void trafficreport_daytick_base::xml_create()
{
    trafficreport_daytick_base orig;

    xml_append_root( TR_DAYTICK_XMLTAG_ROOT);
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
    xml_append_node("Day", get_hourticks());
}

void trafficreport_daytick_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "Sent", keyvalue, sent)) return;
    if (xml_check_value(keyname, "Recv", keyvalue, recv)) return;
    if (xml_check_value(keyname, "Day", keyvalue, boost::bind(&trafficreport_daytick_base::parse_hourticks_string,this,_1))) return;
}



#ifdef UTM_WIN

LONG trafficreport_daytick_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG trafficreport_daytick_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
