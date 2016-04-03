// This is auto-generated file. Do not modify it!

#ifndef UTM_TRAFFICREPORT_HOURTICK_BASE_H
#define UTM_TRAFFICREPORT_HOURTICK_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <gstring.h>

#define  TR_HOURTICK_XMLTAG_ROOT "Day2h"

namespace utm {



class trafficreport_hourtick_base : public ubase
{
public:
    static const char this_class_name[];

public:
    trafficreport_hourtick_base();
    trafficreport_hourtick_base(const trafficreport_hourtick_base& rhs);
    trafficreport_hourtick_base(const trafficreport_hourtick_base& rhs, bool make_threadsafe_copyobj);
    virtual ~trafficreport_hourtick_base();

    trafficreport_hourtick_base& operator=(const trafficreport_hourtick_base& rhs);
    virtual bool equals(const ubase* rhs) const;
    bool operator==(const trafficreport_hourtick_base& rhs) const;

    virtual const char *get_this_class_name() const { return "trafficreport_hourtick_base"; };



public:
    __int64 sent;
    __int64 recv;


public:
    void clear();
    void xml_create();
    void xml_catch_value(const char *keyname, const char *keyvalue);
    virtual ubase* xml_catch_subnode(const char *tag_name, const char *class_name) { return NULL; };



#ifdef UTM_WIN

    virtual LONG SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
    virtual LONG ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
#endif // UTM_WIN
};

}

#endif // UTM_TRAFFICREPORT_HOURTICK_BASE_H
