// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "adapterinfo_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char adapterinfo_base::this_class_name[] = "adapterinfo_base";

adapterinfo_base::adapterinfo_base()
{
    clear();
}

adapterinfo_base::adapterinfo_base(const adapterinfo_base& rhs)
{
    operator=(rhs);
}

adapterinfo_base::adapterinfo_base(const adapterinfo_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

adapterinfo_base::~adapterinfo_base()
{
}

adapterinfo_base& adapterinfo_base::operator=(const adapterinfo_base& rhs)
{
    name = rhs.name;
    descr = rhs.descr;
    mac_permanent = rhs.mac_permanent;
    mac_current = rhs.mac_current;
    medium = rhs.medium;
    ipaddr_v4 = rhs.ipaddr_v4;
    mask_v4 = rhs.mask_v4;
    alias = rhs.alias;
    original_index = rhs.original_index;
    is_promiscuous = rhs.is_promiscuous;
    is_selected = rhs.is_selected;
    return *this;
}

bool adapterinfo_base::operator==(const adapterinfo_base& rhs) const
{
    return equals(&rhs);
}

bool adapterinfo_base::equals(const ubase* rhs) const
{
    if (rhs == NULL)
    {
       return false;
    }

    const adapterinfo_base* other = dynamic_cast<const adapterinfo_base*>(rhs);

    if (!(name == other->name)) return false;
    if (!(descr == other->descr)) return false;
    if (!(mac_permanent == other->mac_permanent)) return false;
    if (!(mac_current == other->mac_current)) return false;
    if (!(medium == other->medium)) return false;
    if (!(ipaddr_v4 == other->ipaddr_v4)) return false;
    if (!(mask_v4 == other->mask_v4)) return false;
    if (!(alias == other->alias)) return false;
    if (!(original_index == other->original_index)) return false;
    if (!(is_promiscuous == other->is_promiscuous)) return false;
    if (!(is_selected == other->is_selected)) return false;

    return true;
}

void adapterinfo_base::clear()
{
    name.clear();
    descr.clear();
    mac_permanent.clear();
    mac_current.clear();
    medium = 0;
    ipaddr_v4 = 0;
    mask_v4 = 0;
    alias = 0;
    original_index = 0;
    is_promiscuous = false;
    is_selected = false;
}

void adapterinfo_base::xml_create()
{
    adapterinfo_base orig;

    xml_append_root( ADAPTERINFO_XMLTAG_NIC);
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

    xml_append_node("Name", name, orig.name);
    xml_append_node("Descr", descr, orig.descr);
    xml_append_node("MacPerm", mac_permanent, orig.mac_permanent);
    xml_append_node("MacCurrent", mac_current, orig.mac_current);
    xml_append_node("Medium", medium, orig.medium);
    xml_append_node("Ip", ipaddr_v4, orig.ipaddr_v4);
    xml_append_node("Mask", mask_v4, orig.mask_v4);
    xml_append_node("Alias", alias, orig.alias);
    xml_append_node("Orig", original_index, orig.original_index);
    xml_append_node("Promiscuous", is_promiscuous, orig.is_promiscuous);
    xml_append_node("Selected", is_selected, orig.is_selected);
}

void adapterinfo_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "Name", keyvalue, name)) return;
    if (xml_check_value(keyname, "Descr", keyvalue, descr)) return;
    if (xml_check_value(keyname, "MacPerm", keyvalue, mac_permanent)) return;
    if (xml_check_value(keyname, "MacCurrent", keyvalue, mac_current)) return;
    if (xml_check_value(keyname, "Medium", keyvalue, medium)) return;
    if (xml_check_value(keyname, "Ip", keyvalue, ipaddr_v4)) return;
    if (xml_check_value(keyname, "Mask", keyvalue, mask_v4)) return;
    if (xml_check_value(keyname, "Alias", keyvalue, alias)) return;
    if (xml_check_value(keyname, "Orig", keyvalue, original_index)) return;
    if (xml_check_value(keyname, "Promiscuous", keyvalue, is_promiscuous)) return;
    if (xml_check_value(keyname, "Selected", keyvalue, is_selected)) return;
}



#ifdef UTM_WIN

LONG adapterinfo_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG adapterinfo_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
