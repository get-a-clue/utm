// This is auto-generated file. Do not modify it!

#ifndef UTM_FILTERCONS_BASE_H
#define UTM_FILTERCONS_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <blob.h>
#include <string>

#define  FILTERCONS_XMLTAG_FILTERCONS "F"

namespace utm {



class filtercons_base : public ubase
{
public:
    static const char this_class_name[];

public:
    filtercons_base();
    filtercons_base(const filtercons_base& rhs);
    filtercons_base(const filtercons_base& rhs, bool make_threadsafe_copyobj);
    virtual ~filtercons_base();

    filtercons_base& operator=(const filtercons_base& rhs);
    virtual bool equals(const ubase* rhs) const;
    bool operator==(const filtercons_base& rhs) const;

    virtual const char *get_this_class_name() const { return "filtercons_base"; };



public:
    unsigned int filter_id;
    __int64 bytes_sent;
    __int64 bytes_recv;
    blob speed_array;


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

#endif // UTM_FILTERCONS_BASE_H
