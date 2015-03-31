// This is auto-generated file. Do not modify it!

#ifndef UTM_CONFIGDNS_BASE_H
#define UTM_CONFIGDNS_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <addrip_v4.h>
#include <addrpair.h>
#include <addrpair_v4.h>
#include <boost/thread/mutex.hpp>

#define  CONFIGDNS_XMLTAG_DNS "DnsSettings"

namespace utm {



class configdns_base : public ubase
{
public:
    static const char this_class_name[];

public:
    configdns_base();
    configdns_base(const configdns_base& rhs);
    configdns_base(const configdns_base& rhs, bool make_threadsafe_copyobj);
    virtual ~configdns_base();

    configdns_base& operator=(const configdns_base& rhs);
    bool  operator==(const configdns_base& rhs) const;

virtual  std::string create_allowedhosts_string() const { return std::string(); };
virtual  void parse_allowedhosts_string(const char *hosts) { };

public:
    bool enabled;
    addrip_v4 fwd_server1;
    addrip_v4 fwd_server2;
    std::vector<addrpair_v4> allowedhosts;
    mutable boost::mutex guard_threadsafe_rw;

public:
    void clear();
    void xml_create();
    void xml_catch_value(const char *keyname, const char *keyvalue);
    virtual ubase* xml_catch_subnode(const char *name) { return NULL; };

    static void threadsafe_copyobj(bool lock_src, const configdns_base& src, configdns_base& dst);

#ifdef UTM_WIN

    virtual LONG SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
    virtual LONG ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
#endif // UTM_WIN
};

}

#endif // UTM_CONFIGDNS_BASE_H
