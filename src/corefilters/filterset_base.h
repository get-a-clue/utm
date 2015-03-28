// This is auto-generated file. Do not modify it!

#ifndef UTM_FILTERSET_BASE_H
#define UTM_FILTERSET_BASE_H

#pragma once
#include <utm.h>

#include <string>
#include <list>
#include <addrtable_v4.h>
#include <addrtablemap.h>
#include <addrtablemap_v4.h>
#include "filter2.h"
#include "filterlist.h"
#include "userlist.h"
#include "urlfilterlist.h"
#include "procnicknamelist.h"
#include "monitor_range_list.h"

#define  FILTERSET_XMLTAG_FILTER "FilterSet"

namespace utm {

typedef  std::list<utm::filter2> filter_container;
typedef  std::list<std::string> lat_as_string_container;
typedef  std::list<utm::addrtablemaprec<utm::addrtable_v4>> mat_container;
typedef  std::list<utm::fsuser> user_container;
typedef  std::list<utm::urlfilter> urlfilter_container;
typedef  std::list<utm::procnickname> procnickname_container;
typedef  std::list<utm::monitor_range> monitor_range_container;

class filterset_base : public ubase
{
public:
    static const char this_class_name[];

public:
    filterset_base();
    filterset_base(const filterset_base& rhs);
    filterset_base(const filterset_base& rhs, bool make_threadsafe_copyobj);
    virtual ~filterset_base();

    filterset_base& operator=(const filterset_base& rhs);
    bool  operator==(const filterset_base& rhs) const;

virtual  filter_container get_filters() { return filters.items; };;
virtual  void parse_filter_string(const char *filter_string) { };
virtual  lat_as_string_container get_lat() const { lat_as_string_container lm; return lm; };
virtual  void parse_lat_string(const char *lat_string) { };
virtual  mat_container get_mat() const { return table_mat.items; };
virtual  void parse_mat_string(const char *mat_string) { };
virtual  user_container get_users() { return users.items; };;
virtual  void parse_user_string(const char *user_string) { };
virtual  urlfilter_container get_urlfilters() { return urlfilters.items; };;
virtual  void parse_urlfilter_string(const char *urlfilter_string) { };
virtual  procnickname_container get_procnicknames() { return procs.items; };;
virtual  void parse_procnickname_string(const char *procnickname_string) { };
virtual  monitor_range_container get_monitorranges() { return ranges.items; };
virtual  void parse_monitorrange_string(const char *monitorrange_string) { };

public:
    int revision;
    filterlist filters;
    addrtable_v4 table_lat;
    addrtablemap_v4 table_mat;
    userlist users;
    urlfilterlist urlfilters;
    procnicknamelist procs;
    monitor_range_list ranges;
    int m_nUserProfileAuthMode;
    int m_nUserProfileStatus;
    int m_nUserProfileManageFilters;
    int m_nUserProfileType;
    int m_nUserProfileDenyDoubleLogins;
    bool m_bUseMiB;
    gstring filterset_file;
    gstring descr;
    gstring adodb_conn_string;
    bool m_bKeepCounters;
    unsigned int m_nLogCntFlushPeriod;
    gstring m_szLogCntFolder1;
    gstring m_szLogCntFolder2;
    gstring m_szLogCntXSLFile1;
    bool m_bLogCntDbEnable;
    unsigned int m_nLogCntDbFlushPeriod;
    gstring m_szLogCntDbTable;
    unsigned int m_nLogCntDbSamples;


public:
    void clear();
    void xml_create();
    void xml_catch_value(const char *keyname, const char *keyvalue);
    virtual ubase* xml_catch_subnode(const char *name) { return NULL; };



#ifdef UTM_WIN

    virtual LONG CreateRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
    virtual LONG SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
    virtual LONG ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
#endif // UTM_WIN
};

}

#endif // UTM_FILTERSET_BASE_H
