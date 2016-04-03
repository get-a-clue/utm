// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "filteragent_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char filteragent_base::this_class_name[] = "filteragent_base";

filteragent_base::filteragent_base()
{
    clear();
}

filteragent_base::filteragent_base(const filteragent_base& rhs)
{
    operator=(rhs);
}

filteragent_base::filteragent_base(const filteragent_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

filteragent_base::~filteragent_base()
{
}

filteragent_base& filteragent_base::operator=(const filteragent_base& rhs)
{
    filter_id = rhs.filter_id;
    filter_name = rhs.filter_name;
    disabled = rhs.disabled;
    speed = rhs.speed;
    traffic_limit_mb = rhs.traffic_limit_mb;
    traffic_limit_type = rhs.traffic_limit_type;
    bytes_sent = rhs.bytes_sent;
    speed_array_sent = rhs.speed_array_sent;
    bytes_recv = rhs.bytes_recv;
    speed_array_recv = rhs.speed_array_recv;
    return *this;
}

bool filteragent_base::operator==(const filteragent_base& rhs) const
{
    return equals(&rhs);
}

bool filteragent_base::equals(const ubase* rhs) const
{
    if (rhs == NULL)
    {
       return false;
    }

    const filteragent_base* other = dynamic_cast<const filteragent_base*>(rhs);

    if (!(filter_id == other->filter_id)) return false;
    if (!(filter_name == other->filter_name)) return false;
    if (!(disabled == other->disabled)) return false;
    if (!(speed == other->speed)) return false;
    if (!(traffic_limit_mb == other->traffic_limit_mb)) return false;
    if (!(traffic_limit_type == other->traffic_limit_type)) return false;
    if (!(bytes_sent == other->bytes_sent)) return false;
    if (!(speed_array_sent == other->speed_array_sent)) return false;
    if (!(bytes_recv == other->bytes_recv)) return false;
    if (!(speed_array_recv == other->speed_array_recv)) return false;

    return true;
}

void filteragent_base::clear()
{
    filter_id = 0;
    filter_name.clear();
    disabled = false;
    speed = 0;
    traffic_limit_mb = 0;
    traffic_limit_type = 0;
    bytes_sent = 0;
    speed_array_sent.clear();
    bytes_recv = 0;
    speed_array_recv.clear();
}

void filteragent_base::xml_create()
{
    filteragent_base orig;

    xml_append_root( FILTERAGENT_XMLTAG_FILTERAGENT);
    add_class_name();
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
    xml_append_node("N", filter_name, orig.filter_name);
    xml_append_node("D", disabled, orig.disabled);
    xml_append_node("SP", speed, orig.speed);
    xml_append_node("TLM", traffic_limit_mb, orig.traffic_limit_mb);
    xml_append_node("TLT", traffic_limit_type, orig.traffic_limit_type);
    xml_append_node("S", bytes_sent, orig.bytes_sent);
    xml_append_node("SA", speed_array_sent, orig.speed_array_sent);
    xml_append_node("R", bytes_recv, orig.bytes_recv);
    xml_append_node("RA", speed_array_recv, orig.speed_array_recv);
}

void filteragent_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "I", keyvalue, filter_id)) return;
    if (xml_check_value(keyname, "N", keyvalue, filter_name)) return;
    if (xml_check_value(keyname, "D", keyvalue, disabled)) return;
    if (xml_check_value(keyname, "SP", keyvalue, speed)) return;
    if (xml_check_value(keyname, "TLM", keyvalue, traffic_limit_mb)) return;
    if (xml_check_value(keyname, "TLT", keyvalue, traffic_limit_type)) return;
    if (xml_check_value(keyname, "S", keyvalue, bytes_sent)) return;
    if (xml_check_value(keyname, "SA", keyvalue, speed_array_sent)) return;
    if (xml_check_value(keyname, "R", keyvalue, bytes_recv)) return;
    if (xml_check_value(keyname, "RA", keyvalue, speed_array_recv)) return;
}



#ifdef UTM_WIN

LONG filteragent_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG filteragent_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
