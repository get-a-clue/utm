// This is auto-generated file. Do not modify it!

#ifndef UTM_CONFIGFWD_BASE_H
#define UTM_CONFIGFWD_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <addrip_v4.h>
#include <boost/thread/mutex.hpp>

#define  CONFIGFWD_XMLTAG_FWD "FwdSettings"

namespace utm {



class configfwd_base : public ubase
{
public:
    static const char this_class_name[];

public:
    configfwd_base();
    configfwd_base(const configfwd_base& rhs);
    configfwd_base(const configfwd_base& rhs, bool make_threadsafe_copyobj);
    virtual ~configfwd_base();

    configfwd_base& operator=(const configfwd_base& rhs);
    bool  operator==(const configfwd_base& rhs) const;



public:
    unsigned short fwd_port;
    addrip_v4 fwd_ip;
    mutable boost::mutex guard_threadsafe_rw;

public:
    void clear();
    void xml_create();
    void xml_catch_value(const char *keyname, const char *keyvalue);
    virtual ubase* xml_catch_subnode(const char *name) { return NULL; };

    static void threadsafe_copyobj(bool lock_src, const configfwd_base& src, configfwd_base& dst);

#ifdef UTM_WIN

    virtual LONG SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
    virtual LONG ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
#endif // UTM_WIN
};

}

#endif // UTM_CONFIGFWD_BASE_H
