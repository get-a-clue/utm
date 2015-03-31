// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "urlfilter_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char urlfilter_base::this_class_name[] = "urlfilter_base";

urlfilter_base::urlfilter_base()
{
    clear();
}

urlfilter_base::urlfilter_base(const urlfilter_base& rhs)
{
    operator=(rhs);
}

urlfilter_base::urlfilter_base(const urlfilter_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

urlfilter_base::~urlfilter_base()
{
}

urlfilter_base& urlfilter_base::operator=(const urlfilter_base& rhs)
{
    ufid = rhs.ufid;
    name = rhs.name;
    file = rhs.file;
    is_disabled = rhs.is_disabled;
    is_any_filter = rhs.is_any_filter;
    addr_part = rhs.addr_part;
    ext_file = rhs.ext_file;
    fids = rhs.fids;
    urlrules = rhs.urlrules;
    return *this;
}

bool urlfilter_base::operator==(const urlfilter_base& rhs) const
{
    if (!(ufid == rhs.ufid)) return false;
    if (!(name == rhs.name)) return false;
    if (!(file == rhs.file)) return false;
    if (!(is_disabled == rhs.is_disabled)) return false;
    if (!(is_any_filter == rhs.is_any_filter)) return false;
    if (!(addr_part == rhs.addr_part)) return false;
    if (!(ext_file == rhs.ext_file)) return false;
    if (!(fids == rhs.fids)) return false;
    if (!(urlrules == rhs.urlrules)) return false;

    return true;
}

void urlfilter_base::clear()
{
    ufid = 0;
    name.clear();
    file.clear();
    is_disabled = false;
    is_any_filter = false;
    addr_part = URLFILTER_ADDRPART_ANY;
    ext_file = 0;
    fids.clear();
    urlrules.clear();
}

void urlfilter_base::xml_create()
{
    urlfilter_base orig;

    xml_append_root( URLFILTER_XMLTAG_ROOT);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("ufid", ufid, orig.ufid);
    xml_append_node("Name", name, orig.name);
    xml_append_node("File", file, orig.file);
    xml_append_node("Disabled", is_disabled, orig.is_disabled);
    xml_append_node("AnyFilter", is_any_filter, orig.is_any_filter);
    xml_append_node("AddrPart", addr_part, orig.addr_part);
    xml_append_node("ExtFile", ext_file, orig.ext_file);
    xml_append_node("Fids", get_filter_ids_str());
    xml_append_node("Rules", get_urlrules_str());
}

void urlfilter_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "ufid", keyvalue, ufid)) return;
    if (xml_check_value(keyname, "Name", keyvalue, name)) return;
    if (xml_check_value(keyname, "File", keyvalue, file)) return;
    if (xml_check_value(keyname, "Disabled", keyvalue, is_disabled)) return;
    if (xml_check_value(keyname, "AnyFilter", keyvalue, is_any_filter)) return;
    if (xml_check_value(keyname, "AddrPart", keyvalue, addr_part)) return;
    if (xml_check_value(keyname, "ExtFile", keyvalue, ext_file)) return;
    if (xml_check_value(keyname, "Fids", keyvalue, boost::bind(&urlfilter_base::parse_filter_ids,this,_1))) return;
    if (xml_check_value(keyname, "Rules", keyvalue, boost::bind(&urlfilter_base::parse_urlrules_str,this,_1))) return;
}



#ifdef UTM_WIN

LONG urlfilter_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG urlfilter_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
