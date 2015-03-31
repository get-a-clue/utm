// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "configfwd_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char configfwd_base::this_class_name[] = "configfwd_base";

configfwd_base::configfwd_base()
{
    clear();
}

configfwd_base::configfwd_base(const configfwd_base& rhs)
{
    operator=(rhs);
}

configfwd_base::configfwd_base(const configfwd_base& rhs, bool make_threadsafe_copyobj)
{
    if (make_threadsafe_copyobj) { threadsafe_copyobj(true, rhs, *this); return; };
    operator=(rhs);
}

configfwd_base::~configfwd_base()
{
}

configfwd_base& configfwd_base::operator=(const configfwd_base& rhs)
{
    fwd_port = rhs.fwd_port;
    fwd_ip = rhs.fwd_ip;
    return *this;
}

bool configfwd_base::operator==(const configfwd_base& rhs) const
{
    if (!(fwd_port == rhs.fwd_port)) return false;
    if (!(fwd_ip == rhs.fwd_ip)) return false;

    return true;
}

void configfwd_base::clear()
{
    fwd_port = 0;
    fwd_ip.clear();
}

void configfwd_base::xml_create()
{
    configfwd_base orig;

    xml_append_root( CONFIGFWD_XMLTAG_FWD);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("FwdPort", fwd_port, orig.fwd_port);
    xml_append_node("FwdIp", fwd_ip, orig.fwd_ip);
}

void configfwd_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "FwdPort", keyvalue, fwd_port)) return;
    if (xml_check_value(keyname, "FwdIp", keyvalue, fwd_ip)) return;
}

void configfwd_base::threadsafe_copyobj(bool lock_src, const configfwd_base& src, configfwd_base& dst)
{
    if (lock_src)
        boost::mutex::scoped_lock lock(src.guard_threadsafe_rw);
    else
        boost::mutex::scoped_lock lock(dst.guard_threadsafe_rw);
    
    dst = src;
}

#ifdef UTM_WIN

LONG configfwd_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG configfwd_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
