// This is auto-generated file. Do not modify it!

#ifndef UTM_URLFILTER_BASE_H
#define UTM_URLFILTER_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <gstring.h>
#include <idset.h>

#define  URLFILTER_XMLTAG_ROOT "UrlFilter"
#define  URLFILTER_ADDRPART_ANY 3
#define  URLFILTER_ADDRPART_HOST 1
#define  URLFILTER_ADDRPART_URI 2

namespace utm {

typedef  std::list<std::string> urlrules_container;

class urlfilter_base : public ubase
{
public:
    static const char this_class_name[];

public:
    urlfilter_base();
    urlfilter_base(const urlfilter_base& rhs);
    urlfilter_base(const urlfilter_base& rhs, bool make_threadsafe_copyobj);
    virtual ~urlfilter_base();

    urlfilter_base& operator=(const urlfilter_base& rhs);
    bool  operator==(const urlfilter_base& rhs) const;

virtual  std::string get_filter_ids_str() const { return fids.to_string(); };
virtual  void parse_filter_ids(const char *p) { fids.from_string(p); };
virtual  std::string get_urlrules_str() const { return std::string(); };
virtual  void parse_urlrules_str(const char *p) { };;

public:
    unsigned int ufid;
    gstring name;
    gstring file;
    bool is_disabled;
    bool is_any_filter;
    int addr_part;
    int ext_file;
    idset fids;
    urlrules_container urlrules;


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

#endif // UTM_URLFILTER_BASE_H
