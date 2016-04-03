// This is auto-generated file. Do not modify it!

#ifndef UTM_FILTERSTATE_BASE_H
#define UTM_FILTERSTATE_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <utime.h>
#include <gstring.h>

#define  FILTERSTATE_XMLTAG_FILTER "Filter"

namespace utm {



class filterstate_base : public ubase
{
public:
    static const char this_class_name[];

public:
    filterstate_base();
    filterstate_base(const filterstate_base& rhs);
    filterstate_base(const filterstate_base& rhs, bool make_threadsafe_copyobj);
    virtual ~filterstate_base();

    filterstate_base& operator=(const filterstate_base& rhs);
    virtual bool equals(const ubase* rhs) const;
    bool operator==(const filterstate_base& rhs) const;

    virtual const char *get_this_class_name() const { return "filterstate_base"; };



public:
    unsigned int filter_id;
    gstring filter_name;
    __int64 bytes_sent;
    __int64 bytes_recv;
    unsigned short limflags;
    utime lastreset;


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

#endif // UTM_FILTERSTATE_BASE_H
