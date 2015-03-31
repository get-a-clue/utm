// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "configdns_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char configdns_base::this_class_name[] = "configdns_base";

configdns_base::configdns_base()
{
    clear();
}

configdns_base::configdns_base(const configdns_base& rhs)
{
    operator=(rhs);
}

configdns_base::configdns_base(const configdns_base& rhs, bool make_threadsafe_copyobj)
{
    if (make_threadsafe_copyobj) { threadsafe_copyobj(true, rhs, *this); return; };
    operator=(rhs);
}

configdns_base::~configdns_base()
{
}

configdns_base& configdns_base::operator=(const configdns_base& rhs)
{
    enabled = rhs.enabled;
    fwd_server1 = rhs.fwd_server1;
    fwd_server2 = rhs.fwd_server2;
    allowedhosts = rhs.allowedhosts;
    return *this;
}

bool configdns_base::operator==(const configdns_base& rhs) const
{
    if (!(enabled == rhs.enabled)) return false;
    if (!(fwd_server1 == rhs.fwd_server1)) return false;
    if (!(fwd_server2 == rhs.fwd_server2)) return false;
    if (!(allowedhosts == rhs.allowedhosts)) return false;

    return true;
}

void configdns_base::clear()
{
    enabled = false;
    fwd_server1.clear();
    fwd_server2.clear();
    allowedhosts.clear();
}

void configdns_base::xml_create()
{
    configdns_base orig;

    xml_append_root( CONFIGDNS_XMLTAG_DNS);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("DnsEnabled", enabled, orig.enabled);
    xml_append_node("ForwardingServer", fwd_server1, orig.fwd_server1);
    xml_append_node("ForwardingServer2", fwd_server2, orig.fwd_server2);
    xml_append_node("ExtraAllowedHosts", create_allowedhosts_string());
}

void configdns_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "DnsEnabled", keyvalue, enabled)) return;
    if (xml_check_value(keyname, "ForwardingServer", keyvalue, fwd_server1)) return;
    if (xml_check_value(keyname, "ForwardingServer2", keyvalue, fwd_server2)) return;
    if (xml_check_value(keyname, "ExtraAllowedHosts", keyvalue, boost::bind(&configdns_base::parse_allowedhosts_string,this,_1))) return;
}

void configdns_base::threadsafe_copyobj(bool lock_src, const configdns_base& src, configdns_base& dst)
{
    if (lock_src)
        boost::mutex::scoped_lock lock(src.guard_threadsafe_rw);
    else
        boost::mutex::scoped_lock lock(dst.guard_threadsafe_rw);
    
    dst = src;
}

#ifdef UTM_WIN

LONG configdns_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG configdns_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
