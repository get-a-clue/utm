// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "procnickname_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char procnickname_base::this_class_name[] = "procnickname_base";

procnickname_base::procnickname_base()
{
    clear();
}

procnickname_base::procnickname_base(const procnickname_base& rhs)
{
    operator=(rhs);
}

procnickname_base::procnickname_base(const procnickname_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

procnickname_base::~procnickname_base()
{
}

procnickname_base& procnickname_base::operator=(const procnickname_base& rhs)
{
    nid = rhs.nid;
    nickname = rhs.nickname;
    pathexe = rhs.pathexe;
    return *this;
}

bool procnickname_base::operator==(const procnickname_base& rhs) const
{
    if (!(nid == rhs.nid)) return false;
    if (!(nickname == rhs.nickname)) return false;
    if (!(pathexe == rhs.pathexe)) return false;

    return true;
}

void procnickname_base::clear()
{
    nid = 0;
    nickname.clear();
    pathexe.clear();
}

void procnickname_base::xml_create()
{
    procnickname_base orig;

    xml_append_root( PROCNICKNAME_XMLTAG_ROOT);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("nid", nid, orig.nid);
    xml_append_node("Nick", nickname, orig.nickname);
    xml_append_node("Exe", pathexe, orig.pathexe);
}

void procnickname_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "nid", keyvalue, nid)) return;
    if (xml_check_value(keyname, "Nick", keyvalue, nickname)) return;
    if (xml_check_value(keyname, "Exe", keyvalue, pathexe)) return;
}



#ifdef UTM_WIN

LONG procnickname_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG procnickname_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
