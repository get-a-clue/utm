// This is auto-generated file. Do not modify it!

#ifndef UTM_CONFIGNAT_BASE_H
#define UTM_CONFIGNAT_BASE_H

#pragma once
#include <utm.h>

#include <vector>
#include <ubase.h>
#include "confignat_portrdr.h"
#include <boost/thread/mutex.hpp>

#define  CONFIGDNS_XMLTAG_NAT "NatSettings"

namespace utm {

typedef  std::vector<utm::confignat_portrdr> confignat_portrdr_container;

class confignat_base : public ubase
{
public:
    static const char this_class_name[];

public:
    confignat_base();
    confignat_base(const confignat_base& rhs);
    confignat_base(const confignat_base& rhs, bool make_threadsafe_copyobj);
    virtual ~confignat_base();

    confignat_base& operator=(const confignat_base& rhs);
    virtual bool equals(const ubase* rhs) const;
    bool operator==(const confignat_base& rhs) const;

    virtual const char *get_this_class_name() const { return "confignat_base"; };

virtual  std::vector<std::string> create_portrdr_string() const { return std::vector<std::string>(); };
virtual  void parse_portrdr_string(const char *portrdr_string) { };

public:
    bool enabled;
    bool exclude_first_filter;
    bool auto_public;
    addrip_v4 public_ip;
    confignat_portrdr_container portrdr;
    mutable boost::mutex guard_threadsafe_rw;

public:
    void clear();
    void xml_create();
    void xml_catch_value(const char *keyname, const char *keyvalue);
    virtual ubase* xml_catch_subnode(const char *tag_name, const char *class_name) { return NULL; };

    static void threadsafe_copyobj(bool lock_src, const confignat_base& src, confignat_base& dst);

#ifdef UTM_WIN

    virtual LONG SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
    virtual LONG ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
#endif // UTM_WIN
};

}

#endif // UTM_CONFIGNAT_BASE_H
