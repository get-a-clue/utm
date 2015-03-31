// This is auto-generated file. Do not modify it!

#ifndef UTM_SINGLEDHCPLEASE_BASE_H
#define UTM_SINGLEDHCPLEASE_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <gstring.h>
#include <addrip_v4.h>
#include <utimestamp.h>

#define  SINGLEDHCPLEASE_XMLTAG_ROOT "SDL"

namespace utm {



class singledhcplease_base : public ubase
{
public:
    static const char this_class_name[];

public:
    singledhcplease_base();
    singledhcplease_base(const singledhcplease_base& rhs);
    singledhcplease_base(const singledhcplease_base& rhs, bool make_threadsafe_copyobj);
    virtual ~singledhcplease_base();

    singledhcplease_base& operator=(const singledhcplease_base& rhs);
    bool  operator==(const singledhcplease_base& rhs) const;



public:
    unsigned int id;
    gstring mac;
    addrip_v4 ipaddr;
    utimestamp leasetime;
    gstring hostname;


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

#endif // UTM_SINGLEDHCPLEASE_BASE_H
