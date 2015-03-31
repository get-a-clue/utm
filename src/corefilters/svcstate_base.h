// This is auto-generated file. Do not modify it!

#ifndef UTM_SVCSTATE_BASE_H
#define UTM_SVCSTATE_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <gstring.h>
#include <capture_status.h>
#include <atomic>

#define  SVCSTATE_XMLTAG_ROOT "SvcState"

namespace utm {



class svcstate_base : public ubase
{
public:
    static const char this_class_name[];

public:
    svcstate_base();
    svcstate_base(const svcstate_base& rhs);
    svcstate_base(const svcstate_base& rhs, bool make_threadsafe_copyobj);
    virtual ~svcstate_base();

    svcstate_base& operator=(const svcstate_base& rhs);
    bool  operator==(const svcstate_base& rhs) const;

virtual  std::string get_currenttm_str() const { return std::string(""); };
virtual  void parse_currenttm_string(const char *currenttm_string) { };

public:
    unsigned int max_demo_filters;
    unsigned int max_filters;
    unsigned int ver_major;
    unsigned int ver_minor;
    unsigned int ver_build;
    unsigned int adapters_selected;
    time_t startup_time;
    std::atomic_uint_fast32_t uptime_seconds;
    time_t current_time;
    tm current_tm;
    std::atomic_uint_fast64_t total_bytes_captured;
    std::atomic_uint_fast64_t total_bytes_captured_prev;
    std::atomic_uint_fast32_t total_bytes_speed;
    std::atomic_uint_fast64_t total_packets_captured;
    std::atomic_uint_fast64_t total_packets_captured_prev;
    std::atomic_uint_fast32_t total_packets_speed;
    std::atomic_uint_fast32_t total_netflow_packets_captured;
    utm::capture_status CaptureStatus;
    utm::gstring license_type;
    utm::gstring reg_name;
    utm::gstring hardware_id;
    int expiration_year;
    int expiration_month;
    int expiration_day;
    int trial_days_total;
    int trial_days_left;
    unsigned int times_after_flush_db;
    unsigned int times_after_flush_xml;
    unsigned int times_after_flush_pktlog;
    int revision;
    utm::gstring fsname;


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

#endif // UTM_SVCSTATE_BASE_H
