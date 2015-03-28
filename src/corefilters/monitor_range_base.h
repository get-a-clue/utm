// This is auto-generated file. Do not modify it!

#ifndef UTM_MONITOR_RANGE_BASE_H
#define UTM_MONITOR_RANGE_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <gstring.h>
#include <addrip_v4.h>
#include <addrmac.h>
#include <ubaselist.h>
#include "monitor_detail_list.h"

#define  MONRANGE_XMLTAG_ROOT "Range"
#define  PING_STATUS_MAXITEMS 4
#define  PING_STATUS_UNKNOWN 0
#define  PING_STATUS_OK 1
#define  PING_STATUS_OFFLINE 2
#define  PING_STATUS_QUALITYDROP 3
#define  PING_STATUS_OKQUALITYDROP 1000
#define  PING_STATUS_UNKNOWN_COLOR RGB(255, 255, 255)
#define  PING_STATUS_OK_COLOR RGB(222, 255, 222)
#define  PING_STATUS_OFFLINE_COLOR RGB(255, 222, 222)
#define  PING_STATUS_QUALITYDROP_COLOR RGB(255, 255, 128)
#define  MAX_PINGATTEMPTS 8
#define  MIN_PINGATTEMPTS 2
#define  MIN_POLLINTERVAL 2
#define  MAX_POLLINTERVAL 15

namespace utm {

typedef  std::list<utm::monitor_detail_record> mdr_container;

class monitor_range_base : public ubase
{
public:
    static const char this_class_name[];

public:
    monitor_range_base();
    monitor_range_base(const monitor_range_base& rhs);
    monitor_range_base(const monitor_range_base& rhs, bool make_threadsafe_copyobj);
    virtual ~monitor_range_base();

    monitor_range_base& operator=(const monitor_range_base& rhs);
    bool  operator==(const monitor_range_base& rhs) const;

virtual  const mdr_container& get_details() const { return details.items; };
virtual  void parse_details_string(const char* p) { };

public:
    unsigned int id;
    unsigned int attempts;
    unsigned int poll_interval;
    bool active;
    unsigned int location;
    unsigned int rangetype;
    bool no_macaddr;
    bool no_macvend;
    bool reverse_lookup;
    addrip_v4 dnsserver;
    gstring comment;
    gstring table;
    addrip_v4 start_addr;
    addrip_v4 end_addr;
    unsigned int color0;
    unsigned int color1;
    unsigned int color2;
    unsigned int color3;
    monitor_detail_list details;


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

#endif // UTM_MONITOR_RANGE_BASE_H
