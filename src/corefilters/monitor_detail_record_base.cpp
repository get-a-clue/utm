// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "monitor_detail_record_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char monitor_detail_record_base::this_class_name[] = "monitor_detail_record_base";

monitor_detail_record_base::monitor_detail_record_base()
{
    clear();
}

monitor_detail_record_base::monitor_detail_record_base(const monitor_detail_record_base& rhs)
{
    operator=(rhs);
}

monitor_detail_record_base::monitor_detail_record_base(const monitor_detail_record_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

monitor_detail_record_base::~monitor_detail_record_base()
{
}

monitor_detail_record_base& monitor_detail_record_base::operator=(const monitor_detail_record_base& rhs)
{
    id = rhs.id;
    flags = rhs.flags;
    ip = rhs.ip;
    mac = rhs.mac;
    comment = rhs.comment;
    return *this;
}

bool monitor_detail_record_base::operator==(const monitor_detail_record_base& rhs) const
{
    if (!(id == rhs.id)) return false;
    if (!(flags == rhs.flags)) return false;
    if (!(ip == rhs.ip)) return false;
    if (!(mac == rhs.mac)) return false;
    if (!(comment == rhs.comment)) return false;

    return true;
}

void monitor_detail_record_base::clear()
{
    id = 0;
    flags = 0;
    ip.clear();
    mac.clear();
    comment.clear();
}

void monitor_detail_record_base::xml_create()
{
    monitor_detail_record_base orig;

    xml_append_root( MONDETREC_XMLTAG_ROOT);
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
    xml_append_node("Flag", flags, orig.flags);
    xml_append_node("Ip", ip, orig.ip);
    xml_append_node("Mac", mac, orig.mac);
    xml_append_node("Comment", comment, orig.comment);
}

void monitor_detail_record_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "Id", keyvalue, id)) return;
    if (xml_check_value(keyname, "Flag", keyvalue, flags)) return;
    if (xml_check_value(keyname, "Ip", keyvalue, ip)) return;
    if (xml_check_value(keyname, "Mac", keyvalue, mac)) return;
    if (xml_check_value(keyname, "Comment", keyvalue, comment)) return;
}



#ifdef UTM_WIN

LONG monitor_detail_record_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG monitor_detail_record_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
