// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "confignat_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char confignat_base::this_class_name[] = "confignat_base";

confignat_base::confignat_base()
{
    clear();
}

confignat_base::confignat_base(const confignat_base& rhs)
{
    operator=(rhs);
}

confignat_base::confignat_base(const confignat_base& rhs, bool make_threadsafe_copyobj)
{
    if (make_threadsafe_copyobj) { threadsafe_copyobj(true, rhs, *this); return; };
    operator=(rhs);
}

confignat_base::~confignat_base()
{
}

confignat_base& confignat_base::operator=(const confignat_base& rhs)
{
    enabled = rhs.enabled;
    exclude_first_filter = rhs.exclude_first_filter;
    auto_public = rhs.auto_public;
    public_ip = rhs.public_ip;
    portrdr = rhs.portrdr;
    return *this;
}

bool confignat_base::operator==(const confignat_base& rhs) const
{
    if (!(enabled == rhs.enabled)) return false;
    if (!(exclude_first_filter == rhs.exclude_first_filter)) return false;
    if (!(auto_public == rhs.auto_public)) return false;
    if (!(public_ip == rhs.public_ip)) return false;
    if (!(portrdr == rhs.portrdr)) return false;

    return true;
}

void confignat_base::clear()
{
    enabled = false;
    exclude_first_filter = false;
    auto_public = false;
    public_ip.clear();
    portrdr.clear();
}

void confignat_base::xml_create()
{
    confignat_base orig;

    xml_append_root( CONFIGDNS_XMLTAG_NAT);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("NatEnabled", enabled, orig.enabled);
    xml_append_node("ExcludeFirst", exclude_first_filter, orig.exclude_first_filter);
    xml_append_node("AutoPublic", auto_public, orig.auto_public);
    xml_append_node("PublicIp", public_ip, orig.public_ip);
    xml_append_node("PortRdr", create_portrdr_string());
}

void confignat_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "NatEnabled", keyvalue, enabled)) return;
    if (xml_check_value(keyname, "ExcludeFirst", keyvalue, exclude_first_filter)) return;
    if (xml_check_value(keyname, "AutoPublic", keyvalue, auto_public)) return;
    if (xml_check_value(keyname, "PublicIp", keyvalue, public_ip)) return;
    if (xml_check_value(keyname, "PortRdr", keyvalue, boost::bind(&confignat_base::parse_portrdr_string,this,_1))) return;
}

void confignat_base::threadsafe_copyobj(bool lock_src, const confignat_base& src, confignat_base& dst)
{
    if (lock_src)
        boost::mutex::scoped_lock lock(src.guard_threadsafe_rw);
    else
        boost::mutex::scoped_lock lock(dst.guard_threadsafe_rw);
    
    dst = src;
}

#ifdef UTM_WIN

LONG confignat_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG confignat_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
