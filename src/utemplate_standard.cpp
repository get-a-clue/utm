// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "$classname.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char $classname::this_class_name[] = "$classname";

$classname::$classname()
{
    clear();
}

$classname::$classname(const $classname& rhs)
{
    operator=(rhs);
}

$classname::$classname(const $classname& rhs, bool make_threadsafe_copyobj)
{
**ctor_threadsafe_copyobj**
    operator=(rhs);
}

$classname::~$classname()
{
}

$classname& $classname::operator=(const $classname& rhs)
{
**assignment_operator**
    return *this;
}

bool $classname::operator==(const $classname& rhs) const
{
**equal_operator**

    return true;
}

void $classname::clear()
{
**clear_function**
}

void $classname::xml_create()
{
    $classname orig;

**xml_root_operator**
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }    
    }

**xml_create_function**
}

void $classname::xml_catch_value(const char *keyname, const char *keyvalue)
{
**xml_catch_value_function**
}

**implementation_threadsafe_rw_function**

#ifdef UTM_WIN

LONG $classname::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {

**registry_save_function**

    RegCloseKey(h);
    };

    return result;
}

LONG $classname::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_READ, &h);
    if (result == ERROR_SUCCESS) {

**registry_load_function**

    RegCloseKey(h);
    };

    return result;
}
#endif // UTM_WIN

}
