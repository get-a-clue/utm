// This is auto-generated file. Do not modify it!

#ifndef UTM_TRAFFICREPORT_FILTER_BASE_H
#define UTM_TRAFFICREPORT_FILTER_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <gstring.h>
#include "trafficreport_dayticklist.h"

#define  TR_FILTER_XMLTAG_ROOT "Filter"

namespace utm {

typedef  std::list<utm::trafficreport_daytick> trafficreport_daytick_container;

class trafficreport_filter_base : public ubase
{
public:
    static const char this_class_name[];

public:
    trafficreport_filter_base();
    trafficreport_filter_base(const trafficreport_filter_base& rhs);
    trafficreport_filter_base(const trafficreport_filter_base& rhs, bool make_threadsafe_copyobj);
    virtual ~trafficreport_filter_base();

    trafficreport_filter_base& operator=(const trafficreport_filter_base& rhs);
    bool  operator==(const trafficreport_filter_base& rhs) const;

virtual  trafficreport_daytick_container get_dayticks() { return dayticks.items; };
virtual  void parse_dayticks_string(const char *daytick_string) { };

public:
    __int64 sent;
    __int64 recv;
    gstring name;
    unsigned int color;
    trafficreport_dayticklist dayticks;


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

#endif // UTM_TRAFFICREPORT_FILTER_BASE_H
