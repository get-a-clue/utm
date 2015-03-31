// This is auto-generated file. Do not modify it!

#ifndef UTM_TRAFFICREPORT_DAYTICK_BASE_H
#define UTM_TRAFFICREPORT_DAYTICK_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <gstring.h>
#include "trafficreport_hourticklist.h"

#define  TR_DAYTICK_XMLTAG_ROOT "Day"

namespace utm {

typedef  std::list<utm::trafficreport_hourtick> trafficreport_hourtick_container;

class trafficreport_daytick_base : public ubase
{
public:
    static const char this_class_name[];

public:
    trafficreport_daytick_base();
    trafficreport_daytick_base(const trafficreport_daytick_base& rhs);
    trafficreport_daytick_base(const trafficreport_daytick_base& rhs, bool make_threadsafe_copyobj);
    virtual ~trafficreport_daytick_base();

    trafficreport_daytick_base& operator=(const trafficreport_daytick_base& rhs);
    bool  operator==(const trafficreport_daytick_base& rhs) const;

virtual  trafficreport_hourtick_container get_hourticks() { return hourticks.items; };
virtual  void parse_hourticks_string(const char *hourtick_string) { };

public:
    __int64 sent;
    __int64 recv;
    trafficreport_hourticklist hourticks;


public:
    void clear();
    void xml_create();
    void xml_catch_value(const char *keyname, const char *keyvalue);
    virtual ubase* xml_catch_subnode(const char *name) { return NULL; };



#ifdef UTM_WIN

    virtual LONG SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
    virtual LONG ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
#endif // UTM_WIN
};

}

#endif // UTM_TRAFFICREPORT_DAYTICK_BASE_H
