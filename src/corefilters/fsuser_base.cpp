// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "fsuser_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char fsuser_base::this_class_name[] = "fsuser_base";

fsuser_base::fsuser_base()
{
    clear();
}

fsuser_base::fsuser_base(const fsuser_base& rhs)
{
    operator=(rhs);
}

fsuser_base::fsuser_base(const fsuser_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

fsuser_base::~fsuser_base()
{
}

fsuser_base& fsuser_base::operator=(const fsuser_base& rhs)
{
    m_uid = rhs.m_uid;
    m_szName = rhs.m_szName;
    m_szDescr = rhs.m_szDescr;
    m_szPasswd = rhs.m_szPasswd;
    m_nAuthMode = rhs.m_nAuthMode;
    m_nStatus = rhs.m_nStatus;
    m_nExpireDt = rhs.m_nExpireDt;
    m_nManageFilters = rhs.m_nManageFilters;
    m_nType = rhs.m_nType;
    m_nDenyDoubleLogins = rhs.m_nDenyDoubleLogins;
    fids = rhs.fids;
    return *this;
}

bool fsuser_base::operator==(const fsuser_base& rhs) const
{
    if (!(m_uid == rhs.m_uid)) return false;
    if (!(m_szName == rhs.m_szName)) return false;
    if (!(m_szDescr == rhs.m_szDescr)) return false;
    if (!(m_szPasswd == rhs.m_szPasswd)) return false;
    if (!(m_nAuthMode == rhs.m_nAuthMode)) return false;
    if (!(m_nStatus == rhs.m_nStatus)) return false;
    if (!(m_nExpireDt == rhs.m_nExpireDt)) return false;
    if (!(m_nManageFilters == rhs.m_nManageFilters)) return false;
    if (!(m_nType == rhs.m_nType)) return false;
    if (!(m_nDenyDoubleLogins == rhs.m_nDenyDoubleLogins)) return false;
    if (!(fids == rhs.fids)) return false;

    return true;
}

void fsuser_base::clear()
{
    m_uid = 0;
    m_szName.clear();
    m_szDescr.clear();
    m_szPasswd.clear();
    m_nAuthMode = 0;
    m_nStatus = 0;
    m_nExpireDt = 0;
    m_nManageFilters = 0;
    m_nType = 0;
    m_nDenyDoubleLogins = 0;
    fids.clear();
}

void fsuser_base::xml_create()
{
    fsuser_base orig;

    xml_append_root( FSUSER_XMLTAG_ROOT);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("uid", m_uid, orig.m_uid);
    xml_append_node("Name", m_szName, orig.m_szName);
    xml_append_node("Descr", m_szDescr, orig.m_szDescr);
    xml_append_node("Passwd", m_szPasswd, orig.m_szPasswd);
    xml_append_node("Auth", m_nAuthMode, orig.m_nAuthMode);
    xml_append_node("Status", m_nStatus, orig.m_nStatus);
    xml_append_node("Expire", m_nExpireDt, orig.m_nExpireDt);
    xml_append_node("Mng", m_nManageFilters, orig.m_nManageFilters);
    xml_append_node("Type", m_nType, orig.m_nType);
    xml_append_node("Double", m_nDenyDoubleLogins, orig.m_nDenyDoubleLogins);
    xml_append_node("Fids", get_filter_ids_str());
}

void fsuser_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "uid", keyvalue, m_uid)) return;
    if (xml_check_value(keyname, "Name", keyvalue, m_szName)) return;
    if (xml_check_value(keyname, "Descr", keyvalue, m_szDescr)) return;
    if (xml_check_value(keyname, "Passwd", keyvalue, m_szPasswd)) return;
    if (xml_check_value(keyname, "Auth", keyvalue, m_nAuthMode)) return;
    if (xml_check_value(keyname, "Status", keyvalue, m_nStatus)) return;
    if (xml_check_value(keyname, "Expire", keyvalue, m_nExpireDt)) return;
    if (xml_check_value(keyname, "Mng", keyvalue, m_nManageFilters)) return;
    if (xml_check_value(keyname, "Type", keyvalue, m_nType)) return;
    if (xml_check_value(keyname, "Double", keyvalue, m_nDenyDoubleLogins)) return;
    if (xml_check_value(keyname, "Fids", keyvalue, boost::bind(&fsuser_base::parse_filter_ids,this,_1))) return;
}



#ifdef UTM_WIN

LONG fsuser_base::CreateRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_READ, &h);
    if (result == ERROR_FILE_NOT_FOUND)
    {
       result = RegCreateKeyEx(hk, pRegistryPath, NULL, NULL, NULL, KEY_ALL_ACCESS, NULL, &h, NULL);
    }

    if (result == ERROR_SUCCESS)
    {
       RegCloseKey(h);
    };

    return result;
}

LONG fsuser_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG fsuser_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
