// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "filterset_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char filterset_base::this_class_name[] = "filterset_base";

filterset_base::filterset_base()
{
    clear();
}

filterset_base::filterset_base(const filterset_base& rhs)
{
    operator=(rhs);
}

filterset_base::filterset_base(const filterset_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

filterset_base::~filterset_base()
{
}

filterset_base& filterset_base::operator=(const filterset_base& rhs)
{
    revision = rhs.revision;
    filters = rhs.filters;
    table_lat = rhs.table_lat;
    table_mat = rhs.table_mat;
    users = rhs.users;
    urlfilters = rhs.urlfilters;
    procs = rhs.procs;
    ranges = rhs.ranges;
    m_nUserProfileAuthMode = rhs.m_nUserProfileAuthMode;
    m_nUserProfileStatus = rhs.m_nUserProfileStatus;
    m_nUserProfileManageFilters = rhs.m_nUserProfileManageFilters;
    m_nUserProfileType = rhs.m_nUserProfileType;
    m_nUserProfileDenyDoubleLogins = rhs.m_nUserProfileDenyDoubleLogins;
    m_bUseMiB = rhs.m_bUseMiB;
    filterset_file = rhs.filterset_file;
    descr = rhs.descr;
    adodb_conn_string = rhs.adodb_conn_string;
    m_bKeepCounters = rhs.m_bKeepCounters;
    m_nLogCntFlushPeriod = rhs.m_nLogCntFlushPeriod;
    m_szLogCntFolder1 = rhs.m_szLogCntFolder1;
    m_szLogCntFolder2 = rhs.m_szLogCntFolder2;
    m_szLogCntXSLFile1 = rhs.m_szLogCntXSLFile1;
    m_bLogCntDbEnable = rhs.m_bLogCntDbEnable;
    m_nLogCntDbFlushPeriod = rhs.m_nLogCntDbFlushPeriod;
    m_szLogCntDbTable = rhs.m_szLogCntDbTable;
    m_nLogCntDbSamples = rhs.m_nLogCntDbSamples;
    return *this;
}

bool filterset_base::operator==(const filterset_base& rhs) const
{
    if (!(revision == rhs.revision)) return false;
    if (!(filters == rhs.filters)) return false;
    if (!(table_lat == rhs.table_lat)) return false;
    if (!(table_mat == rhs.table_mat)) return false;
    if (!(users == rhs.users)) return false;
    if (!(urlfilters == rhs.urlfilters)) return false;
    if (!(procs == rhs.procs)) return false;
    if (!(ranges == rhs.ranges)) return false;
    if (!(m_nUserProfileAuthMode == rhs.m_nUserProfileAuthMode)) return false;
    if (!(m_nUserProfileStatus == rhs.m_nUserProfileStatus)) return false;
    if (!(m_nUserProfileManageFilters == rhs.m_nUserProfileManageFilters)) return false;
    if (!(m_nUserProfileType == rhs.m_nUserProfileType)) return false;
    if (!(m_nUserProfileDenyDoubleLogins == rhs.m_nUserProfileDenyDoubleLogins)) return false;
    if (!(m_bUseMiB == rhs.m_bUseMiB)) return false;
    if (!(filterset_file == rhs.filterset_file)) return false;
    if (!(descr == rhs.descr)) return false;
    if (!(adodb_conn_string == rhs.adodb_conn_string)) return false;
    if (!(m_bKeepCounters == rhs.m_bKeepCounters)) return false;
    if (!(m_nLogCntFlushPeriod == rhs.m_nLogCntFlushPeriod)) return false;
    if (!(m_szLogCntFolder1 == rhs.m_szLogCntFolder1)) return false;
    if (!(m_szLogCntFolder2 == rhs.m_szLogCntFolder2)) return false;
    if (!(m_szLogCntXSLFile1 == rhs.m_szLogCntXSLFile1)) return false;
    if (!(m_bLogCntDbEnable == rhs.m_bLogCntDbEnable)) return false;
    if (!(m_nLogCntDbFlushPeriod == rhs.m_nLogCntDbFlushPeriod)) return false;
    if (!(m_szLogCntDbTable == rhs.m_szLogCntDbTable)) return false;
    if (!(m_nLogCntDbSamples == rhs.m_nLogCntDbSamples)) return false;

    return true;
}

void filterset_base::clear()
{
    revision = 0;
    filters.clear();
    table_lat.clear();
    table_mat.clear();
    users.clear();
    urlfilters.clear();
    procs.clear();
    ranges.clear();
    m_nUserProfileAuthMode = 0;
    m_nUserProfileStatus = 0;
    m_nUserProfileManageFilters = 0;
    m_nUserProfileType = 0;
    m_nUserProfileDenyDoubleLogins = 0;
    m_bUseMiB = true;
    filterset_file.clear();
    descr.clear();
    adodb_conn_string.clear();
    m_bKeepCounters = true;
    m_nLogCntFlushPeriod = 10;
    m_szLogCntFolder1.clear();
    m_szLogCntFolder2.clear();
    m_szLogCntXSLFile1.clear();
    m_bLogCntDbEnable = false;
    m_nLogCntDbFlushPeriod = 20;
    m_szLogCntDbTable.clear();
    m_nLogCntDbSamples = 20;
}

void filterset_base::xml_create()
{
    filterset_base orig;

    xml_append_root( FILTERSET_XMLTAG_FILTER);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("FiltersetRevision", revision, orig.revision);
    xml_append_node("Filter", get_filters());
    xml_append_node("LAT", get_lat());
    xml_append_node("MAT", get_mat());
    xml_append_node("User", get_users());
    xml_append_node("UrlFilter", get_urlfilters());
    xml_append_node("Proc", get_procnicknames());
    xml_append_node("Range", get_monitorranges());
    xml_append_node("UserProfileAuthMode", m_nUserProfileAuthMode, orig.m_nUserProfileAuthMode);
    xml_append_node("UserProfileStatus", m_nUserProfileStatus, orig.m_nUserProfileStatus);
    xml_append_node("UserProfileManagerFilters", m_nUserProfileManageFilters, orig.m_nUserProfileManageFilters);
    xml_append_node("UserProfileType", m_nUserProfileType, orig.m_nUserProfileType);
    xml_append_node("UserProfileDenyDoubleLogins", m_nUserProfileDenyDoubleLogins, orig.m_nUserProfileDenyDoubleLogins);
    xml_append_node("UseMebiBytes", m_bUseMiB, orig.m_bUseMiB);
    xml_append_node("File", filterset_file, orig.filterset_file);
    xml_append_node("FiltersetDescription", descr, orig.descr);
    xml_append_node("DbConnString", adodb_conn_string, orig.adodb_conn_string);
    xml_append_node("RestoreCounters", m_bKeepCounters, orig.m_bKeepCounters);
    xml_append_node("LogCntXmlReportsFlushPeriod", m_nLogCntFlushPeriod, orig.m_nLogCntFlushPeriod);
    xml_append_node("LogCntXmlReportsFolder", m_szLogCntFolder1, orig.m_szLogCntFolder1);
    xml_append_node("LogCntBackupReportsFolder", m_szLogCntFolder2, orig.m_szLogCntFolder2);
    xml_append_node("LogCntXmlReportsXsl", m_szLogCntXSLFile1, orig.m_szLogCntXSLFile1);
    xml_append_node("LogCntDbEnable", m_bLogCntDbEnable, orig.m_bLogCntDbEnable);
    xml_append_node("LogCntDbFlushPeriod", m_nLogCntDbFlushPeriod, orig.m_nLogCntDbFlushPeriod);
    xml_append_node("LogCntDbTable", m_szLogCntDbTable, orig.m_szLogCntDbTable);
    xml_append_node("LogCntDbSamples", m_nLogCntDbSamples, orig.m_nLogCntDbSamples);
}

void filterset_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "FiltersetRevision", keyvalue, revision)) return;
    if (xml_check_value(keyname, "Filter", keyvalue, boost::bind(&filterset_base::parse_filter_string,this,_1))) return;
    if (xml_check_value(keyname, "LAT", keyvalue, boost::bind(&filterset_base::parse_lat_string,this,_1))) return;
    if (xml_check_value(keyname, "MAT", keyvalue, boost::bind(&filterset_base::parse_mat_string,this,_1))) return;
    if (xml_check_value(keyname, "User", keyvalue, boost::bind(&filterset_base::parse_user_string,this,_1))) return;
    if (xml_check_value(keyname, "UrlFilter", keyvalue, boost::bind(&filterset_base::parse_urlfilter_string,this,_1))) return;
    if (xml_check_value(keyname, "Proc", keyvalue, boost::bind(&filterset_base::parse_procnickname_string,this,_1))) return;
    if (xml_check_value(keyname, "Range", keyvalue, boost::bind(&filterset_base::parse_monitorrange_string,this,_1))) return;
    if (xml_check_value(keyname, "UserProfileAuthMode", keyvalue, m_nUserProfileAuthMode)) return;
    if (xml_check_value(keyname, "UserProfileStatus", keyvalue, m_nUserProfileStatus)) return;
    if (xml_check_value(keyname, "UserProfileManagerFilters", keyvalue, m_nUserProfileManageFilters)) return;
    if (xml_check_value(keyname, "UserProfileType", keyvalue, m_nUserProfileType)) return;
    if (xml_check_value(keyname, "UserProfileDenyDoubleLogins", keyvalue, m_nUserProfileDenyDoubleLogins)) return;
    if (xml_check_value(keyname, "UseMebiBytes", keyvalue, m_bUseMiB)) return;
    if (xml_check_value(keyname, "File", keyvalue, filterset_file)) return;
    if (xml_check_value(keyname, "FiltersetDescription", keyvalue, descr)) return;
    if (xml_check_value(keyname, "DbConnString", keyvalue, adodb_conn_string)) return;
    if (xml_check_value(keyname, "RestoreCounters", keyvalue, m_bKeepCounters)) return;
    if (xml_check_value(keyname, "LogCntXmlReportsFlushPeriod", keyvalue, m_nLogCntFlushPeriod)) return;
    if (xml_check_value(keyname, "LogCntXmlReportsFolder", keyvalue, m_szLogCntFolder1)) return;
    if (xml_check_value(keyname, "LogCntBackupReportsFolder", keyvalue, m_szLogCntFolder2)) return;
    if (xml_check_value(keyname, "LogCntXmlReportsXsl", keyvalue, m_szLogCntXSLFile1)) return;
    if (xml_check_value(keyname, "LogCntDbEnable", keyvalue, m_bLogCntDbEnable)) return;
    if (xml_check_value(keyname, "LogCntDbFlushPeriod", keyvalue, m_nLogCntDbFlushPeriod)) return;
    if (xml_check_value(keyname, "LogCntDbTable", keyvalue, m_szLogCntDbTable)) return;
    if (xml_check_value(keyname, "LogCntDbSamples", keyvalue, m_nLogCntDbSamples)) return;
}



#ifdef UTM_WIN

LONG filterset_base::CreateRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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

LONG filterset_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG filterset_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
