// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "monitor_range_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char monitor_range_base::this_class_name[] = "monitor_range_base";

monitor_range_base::monitor_range_base()
{
    clear();
}

monitor_range_base::monitor_range_base(const monitor_range_base& rhs)
{
    operator=(rhs);
}

monitor_range_base::monitor_range_base(const monitor_range_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

monitor_range_base::~monitor_range_base()
{
}

monitor_range_base& monitor_range_base::operator=(const monitor_range_base& rhs)
{
    id = rhs.id;
    attempts = rhs.attempts;
    poll_interval = rhs.poll_interval;
    active = rhs.active;
    location = rhs.location;
    rangetype = rhs.rangetype;
    no_macaddr = rhs.no_macaddr;
    no_macvend = rhs.no_macvend;
    reverse_lookup = rhs.reverse_lookup;
    dnsserver = rhs.dnsserver;
    comment = rhs.comment;
    table = rhs.table;
    start_addr = rhs.start_addr;
    end_addr = rhs.end_addr;
    color0 = rhs.color0;
    color1 = rhs.color1;
    color2 = rhs.color2;
    color3 = rhs.color3;
    details = rhs.details;
    return *this;
}

bool monitor_range_base::operator==(const monitor_range_base& rhs) const
{
    if (!(id == rhs.id)) return false;
    if (!(attempts == rhs.attempts)) return false;
    if (!(poll_interval == rhs.poll_interval)) return false;
    if (!(active == rhs.active)) return false;
    if (!(location == rhs.location)) return false;
    if (!(rangetype == rhs.rangetype)) return false;
    if (!(no_macaddr == rhs.no_macaddr)) return false;
    if (!(no_macvend == rhs.no_macvend)) return false;
    if (!(reverse_lookup == rhs.reverse_lookup)) return false;
    if (!(dnsserver == rhs.dnsserver)) return false;
    if (!(comment == rhs.comment)) return false;
    if (!(table == rhs.table)) return false;
    if (!(start_addr == rhs.start_addr)) return false;
    if (!(end_addr == rhs.end_addr)) return false;
    if (!(color0 == rhs.color0)) return false;
    if (!(color1 == rhs.color1)) return false;
    if (!(color2 == rhs.color2)) return false;
    if (!(color3 == rhs.color3)) return false;
    if (!(details == rhs.details)) return false;

    return true;
}

void monitor_range_base::clear()
{
    id = 0;
    attempts = 0;
    poll_interval = 0;
    active = true;
    location = 0;
    rangetype = 0;
    no_macaddr = false;
    no_macvend = false;
    reverse_lookup = false;
    dnsserver.clear();
    comment.clear();
    table.clear();
    start_addr.clear();
    end_addr.clear();
    color0 = PING_STATUS_UNKNOWN_COLOR;
    color1 = PING_STATUS_OK_COLOR;
    color2 = PING_STATUS_OFFLINE_COLOR;
    color3 = PING_STATUS_QUALITYDROP_COLOR;
    details.clear();
}

void monitor_range_base::xml_create()
{
    monitor_range_base orig;

    xml_append_root( MONRANGE_XMLTAG_ROOT);
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
    xml_append_node("Att", attempts, orig.attempts);
    xml_append_node("Poll", poll_interval, orig.poll_interval);
    xml_append_node("Enabled", active, orig.active);
    xml_append_node("Location", location, orig.location);
    xml_append_node("Type", rangetype, orig.rangetype);
    xml_append_node("NoMacAddr", no_macaddr, orig.no_macaddr);
    xml_append_node("NoMacVendor", no_macvend, orig.no_macvend);
    xml_append_node("ReverseLookup", reverse_lookup, orig.reverse_lookup);
    xml_append_node("DnsServer", dnsserver, orig.dnsserver);
    xml_append_node("Comment", comment, orig.comment);
    xml_append_node("Table", table, orig.table);
    xml_append_node("Start", start_addr, orig.start_addr);
    xml_append_node("End", end_addr, orig.end_addr);
    xml_append_node("Color0", color0, orig.color0);
    xml_append_node("Color1", color1, orig.color1);
    xml_append_node("Color2", color2, orig.color2);
    xml_append_node("Color3", color3, orig.color3);
    xml_append_node("Entry", get_details());
}

void monitor_range_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "Id", keyvalue, id)) return;
    if (xml_check_value(keyname, "Att", keyvalue, attempts)) return;
    if (xml_check_value(keyname, "Poll", keyvalue, poll_interval)) return;
    if (xml_check_value(keyname, "Enabled", keyvalue, active)) return;
    if (xml_check_value(keyname, "Location", keyvalue, location)) return;
    if (xml_check_value(keyname, "Type", keyvalue, rangetype)) return;
    if (xml_check_value(keyname, "NoMacAddr", keyvalue, no_macaddr)) return;
    if (xml_check_value(keyname, "NoMacVendor", keyvalue, no_macvend)) return;
    if (xml_check_value(keyname, "ReverseLookup", keyvalue, reverse_lookup)) return;
    if (xml_check_value(keyname, "DnsServer", keyvalue, dnsserver)) return;
    if (xml_check_value(keyname, "Comment", keyvalue, comment)) return;
    if (xml_check_value(keyname, "Table", keyvalue, table)) return;
    if (xml_check_value(keyname, "Start", keyvalue, start_addr)) return;
    if (xml_check_value(keyname, "End", keyvalue, end_addr)) return;
    if (xml_check_value(keyname, "Color0", keyvalue, color0)) return;
    if (xml_check_value(keyname, "Color1", keyvalue, color1)) return;
    if (xml_check_value(keyname, "Color2", keyvalue, color2)) return;
    if (xml_check_value(keyname, "Color3", keyvalue, color3)) return;
    if (xml_check_value(keyname, "Entry", keyvalue, boost::bind(&monitor_range_base::parse_details_string,this,_1))) return;
}



#ifdef UTM_WIN

LONG monitor_range_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG monitor_range_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
