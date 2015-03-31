// This is auto-generated file. Do not modify it!

#ifndef UTM_ADAPTERINFO_BASE_H
#define UTM_ADAPTERINFO_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <gstring.h>
#include <addrmac.h>

#define  ADAPTERINFO_XMLTAG_NIC "Nic"

namespace utm {



class adapterinfo_base : public ubase
{
public:
    static const char this_class_name[];

public:
    adapterinfo_base();
    adapterinfo_base(const adapterinfo_base& rhs);
    adapterinfo_base(const adapterinfo_base& rhs, bool make_threadsafe_copyobj);
    virtual ~adapterinfo_base();

    adapterinfo_base& operator=(const adapterinfo_base& rhs);
    bool  operator==(const adapterinfo_base& rhs) const;



public:
    gstring name;
    gstring descr;
    addrmac mac_permanent;
    addrmac mac_current;
    unsigned int medium;
    unsigned int ipaddr_v4;
    unsigned int mask_v4;
    unsigned int alias;
    unsigned int original_index;
    bool is_promiscuous;
    bool is_selected;


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

#endif // UTM_ADAPTERINFO_BASE_H
