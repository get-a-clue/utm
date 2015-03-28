// This is auto-generated file. Do not modify it!

#ifndef UTM_MONITOR_DETAIL_RECORD_BASE_H
#define UTM_MONITOR_DETAIL_RECORD_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <gstring.h>
#include <addrip_v4.h>
#include <addrmac.h>

#define  MONDETREC_XMLTAG_ROOT "Entry"

namespace utm {



class monitor_detail_record_base : public ubase
{
public:
    static const char this_class_name[];

public:
    monitor_detail_record_base();
    monitor_detail_record_base(const monitor_detail_record_base& rhs);
    monitor_detail_record_base(const monitor_detail_record_base& rhs, bool make_threadsafe_copyobj);
    virtual ~monitor_detail_record_base();

    monitor_detail_record_base& operator=(const monitor_detail_record_base& rhs);
    bool  operator==(const monitor_detail_record_base& rhs) const;



public:
    unsigned int id;
    unsigned int flags;
    addrip_v4 ip;
    addrmac mac;
    gstring comment;


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

#endif // UTM_MONITOR_DETAIL_RECORD_BASE_H
