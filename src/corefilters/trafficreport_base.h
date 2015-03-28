// This is auto-generated file. Do not modify it!

#ifndef UTM_TRAFFICREPORT_BASE_H
#define UTM_TRAFFICREPORT_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <gstring.h>
#include "trafficreport_filterlist.h"

#define  TR_TRAFFICREPORT_XMLTAG_ROOT "TrafficReport"

namespace utm {

typedef  std::list<utm::trafficreport_filter> trafficreport_filter_container;

class trafficreport_base : public ubase
{
public:
    static const char this_class_name[];

public:
    trafficreport_base();
    trafficreport_base(const trafficreport_base& rhs);
    trafficreport_base(const trafficreport_base& rhs, bool make_threadsafe_copyobj);
    virtual ~trafficreport_base();

    trafficreport_base& operator=(const trafficreport_base& rhs);
    bool  operator==(const trafficreport_base& rhs) const;

virtual  trafficreport_filter_container get_filters() { return filters.items; };
virtual  void parse_filter_string(const char *filter_string) { };

public:
    trafficreport_filterlist filters;


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

#endif // UTM_TRAFFICREPORT_BASE_H
