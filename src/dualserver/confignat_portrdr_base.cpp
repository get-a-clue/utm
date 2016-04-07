// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "confignat_portrdr_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char confignat_portrdr_base::this_class_name[] = "confignat_portrdr_base";

confignat_portrdr_base::confignat_portrdr_base()
{
    clear();
}

confignat_portrdr_base::confignat_portrdr_base(const confignat_portrdr_base& rhs)
{
    operator=(rhs);
}

confignat_portrdr_base::confignat_portrdr_base(const confignat_portrdr_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

confignat_portrdr_base::~confignat_portrdr_base()
{
}

confignat_portrdr_base& confignat_portrdr_base::operator=(const confignat_portrdr_base& rhs)
{
    proto = rhs.proto;
    public_port = rhs.public_port;
    remote_port = rhs.remote_port;
    remote_ip = rhs.remote_ip;
    return *this;
}

bool confignat_portrdr_base::operator==(const confignat_portrdr_base& rhs) const
{
    return equals(&rhs);
}

bool confignat_portrdr_base::equals(const ubase* rhs) const
{
    if (rhs == NULL)
    {
       return false;
    }

    const confignat_portrdr_base* other = dynamic_cast<const confignat_portrdr_base*>(rhs);

    if (!(proto == other->proto)) return false;
    if (!(public_port == other->public_port)) return false;
    if (!(remote_port == other->remote_port)) return false;
    if (!(remote_ip == other->remote_ip)) return false;

    return true;
}

void confignat_portrdr_base::clear()
{
    proto = 0;
    public_port = 0;
    remote_port = 0;
    remote_ip = 0;
}

void confignat_portrdr_base::xml_create()
{
    confignat_portrdr_base orig;

    xml_append_root( CONFIGNAT_XMLTAG_PORTRDR);
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

    xml_append_node("Proto", proto, orig.proto);
    xml_append_node("PublicPort", public_port, orig.public_port);
    xml_append_node("RemotePort", remote_port, orig.remote_port);
    xml_append_node("RemoteIp", remote_ip, orig.remote_ip);
}

void confignat_portrdr_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "Proto", keyvalue, proto)) return;
    if (xml_check_value(keyname, "PublicPort", keyvalue, public_port)) return;
    if (xml_check_value(keyname, "RemotePort", keyvalue, remote_port)) return;
    if (xml_check_value(keyname, "RemoteIp", keyvalue, remote_ip)) return;
}



#ifdef UTM_WIN

LONG confignat_portrdr_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG confignat_portrdr_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
