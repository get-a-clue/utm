// This is auto-generated file. Do not modify it!

#ifndef UTM_FILTERSETSTATE_BASE_H
#define UTM_FILTERSETSTATE_BASE_H

#pragma once
#include <utm.h>

#include <string>
#include <list>
#include <utime.h>
#include <filterstate.h>
#include <filterstatelist.h>

#define  FILTERSETSTATE_XMLTAG_TRAFFICCOUNTERS "TrafficCounters"

namespace utm {

typedef  std::list<utm::filterstate> filterstate_container;

class filtersetstate_base : public ubase
{
public:
    static const char this_class_name[];

public:
    filtersetstate_base();
    filtersetstate_base(const filtersetstate_base& rhs);
    filtersetstate_base(const filtersetstate_base& rhs, bool make_threadsafe_copyobj);
    virtual ~filtersetstate_base();

    filtersetstate_base& operator=(const filtersetstate_base& rhs);
    bool  operator==(const filtersetstate_base& rhs) const;

virtual  filterstate_container get_filterstates() { return filterstates.items; };;
virtual  void parse_filterstate_string(const char *filterstate_string) { };

public:
    gstring comment;
    gstring filtersetfile;
    filterstatelist filterstates;
    utime lastmodified;


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

#endif // UTM_FILTERSETSTATE_BASE_H
