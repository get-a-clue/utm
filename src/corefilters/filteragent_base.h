// This is auto-generated file. Do not modify it!

#ifndef UTM_FILTERAGENT_BASE_H
#define UTM_FILTERAGENT_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <gstring.h>
#include <string>

#define  FILTERAGENT_XMLTAG_FILTERAGENT "Filter"

namespace utm {



class filteragent_base : public ubase
{
public:
    static const char this_class_name[];

public:
    filteragent_base();
    filteragent_base(const filteragent_base& rhs);
    filteragent_base(const filteragent_base& rhs, bool make_threadsafe_copyobj);
    virtual ~filteragent_base();

    filteragent_base& operator=(const filteragent_base& rhs);
    virtual bool equals(const ubase* rhs) const;
    bool operator==(const filteragent_base& rhs) const;

    virtual const char *get_this_class_name() const { return "filteragent_base"; };



public:
    unsigned int filter_id;
    gstring filter_name;
    bool disabled;
    int speed;
    __int64 traffic_limit_mb;
    int traffic_limit_type;
    __int64 bytes_sent;
    std::string speed_array_sent;
    __int64 bytes_recv;
    std::string speed_array_recv;


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

#endif // UTM_FILTERAGENT_BASE_H
