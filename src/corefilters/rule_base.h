// This is auto-generated file. Do not modify it!

#ifndef UTM_RULE_BASE_H
#define UTM_RULE_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <addrip_v4.h>
#include <addrmac.h>
#include <gstring.h>
#include "hostname.h"
#include "rule_common.h"

#define  RULE_XMLTAG_RULE "RuleNT"

namespace utm {



class rule_base : public ubase
{
public:
    static const char this_class_name[];

public:
    rule_base();
    rule_base(const rule_base& rhs);
    rule_base(const rule_base& rhs, bool make_threadsafe_copyobj);
    virtual ~rule_base();

    rule_base& operator=(const rule_base& rhs);
    bool  operator==(const rule_base& rhs) const;

virtual  const char* get_src_host_str() const { return src_host.get_host(); };
virtual  const char* get_dst_host_str() const { return dst_host.get_host(); };
virtual  void parse_src_host_string(const char *p) { src_host.set_host(p); };
virtual  void parse_dst_host_string(const char *p) { dst_host.set_host(p); };

public:
    unsigned int id;
    unsigned short mirrored;
    gstring descr;
    int action;
    unsigned short rwr_fwd;
    int proto;
    unsigned int nicalias;
    unsigned int pkt_options;
    unsigned int condition_type;
    unsigned int condition_limit;
    unsigned int cond_mac_type;
    addrmac cond_mac_data;
    int prevfilter_type;
    int natuse;
    unsigned int wday;
    unsigned short time_from;
    unsigned short time_to;
    int src_type;
    addrip_v4 src_ip;
    addrip_v4 src_mask;
    unsigned short src_addr_type;
    unsigned short src_port_type;
    unsigned short src_port;
    unsigned short src_port_to;
    unsigned int src_atkey;
    unsigned int src_uid;
    unsigned int src_procnick;
    addrmac src_mac;
    hostname src_host;
    int dst_type;
    addrip_v4 dst_ip;
    addrip_v4 dst_mask;
    unsigned short dst_addr_type;
    unsigned short dst_port_type;
    unsigned short dst_port;
    unsigned short dst_port_to;
    unsigned int dst_atkey;
    unsigned int dst_uid;
    unsigned int dst_procnick;
    addrmac dst_mac;
    hostname dst_host;


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

#endif // UTM_RULE_BASE_H
