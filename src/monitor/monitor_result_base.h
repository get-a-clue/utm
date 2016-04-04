// This is auto-generated file. Do not modify it!

#ifndef UTM_MONITOR_RESULT_BASE_H
#define UTM_MONITOR_RESULT_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <addrip_v4.h>
#include <addrmac.h>
#include <gstring.h>
#include <utimestamp.h>

#define  MONITORRESULT_XMLTAG_MONITORRESULT "MonitorResults"

namespace utm {



class monitor_result_base : public ubase
{
public:
    static const char this_class_name[];

public:
    monitor_result_base();
    monitor_result_base(const monitor_result_base& rhs);
    monitor_result_base(const monitor_result_base& rhs, bool make_threadsafe_copyobj);
    virtual ~monitor_result_base();

    monitor_result_base& operator=(const monitor_result_base& rhs);
    virtual bool equals(const ubase* rhs) const;
    bool operator==(const monitor_result_base& rhs) const;

    virtual const char *get_this_class_name() const { return "monitor_result_base"; };



public:
    unsigned int id;
    int range_id;
    int action_type;
    int status;
    utimestamp last_status_change;
    utimestamp last_ping_reply;
    unsigned int curr_serial_request_count;
    unsigned int curr_serial_reply_count;
    unsigned int last_serial_request_count;
    unsigned int last_serial_reply_count;
    unsigned int total_ping_request_count;
    unsigned int total_ping_reply_count;
    unsigned int serial;
    unsigned int serial_attempts;
    std::int64_t curr_tick_request;
    std::int64_t curr_tick_reply;
    addrip_v4 addr;
    addrmac mac;
    unsigned int flags;
    gstring hostname;
    gstring descr;


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

#endif // UTM_MONITOR_RESULT_BASE_H
