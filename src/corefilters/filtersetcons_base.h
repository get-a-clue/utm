// This is auto-generated file. Do not modify it!

#ifndef UTM_FILTERSETCONS_BASE_H
#define UTM_FILTERSETCONS_BASE_H

#pragma once
#include <utm.h>

#include <string>
#include <list>
#include <utime.h>
#include <blob.h>
#include <filtercons.h>
#include <filterconslist.h>

#define  FILTERSETCONS_XMLTAG_TRAFFICCONS "TrafficConsole"

namespace utm {

typedef  std::list<std::unique_ptr<utm::ubase>> filtercons_container;

class filtersetcons_base : public ubase
{
public:
    static const char this_class_name[];

public:
    filtersetcons_base();
    filtersetcons_base(const filtersetcons_base& rhs);
    filtersetcons_base(const filtersetcons_base& rhs, bool make_threadsafe_copyobj);
    virtual ~filtersetcons_base();

    filtersetcons_base& operator=(const filtersetcons_base& rhs);
    virtual bool equals(const ubase* rhs) const;
    bool operator==(const filtersetcons_base& rhs) const;

    virtual const char *get_this_class_name() const { return "filtersetcons_base"; };

virtual  filtercons_container& get_filtercons() { return filtercons_s.items; };;
virtual  void parse_filtercons_string(const char *filtercons_string) { };

public:
    __int64 total_bytes_captured;
    unsigned int total_bytes_speed;
    __int64 total_packets_captured;
    unsigned int total_packets_speed;
    unsigned int total_netflow_packets_captured;
    unsigned int total_points;
    unsigned int next_seqnum;
    unsigned int diff_size;
    unsigned int start_diffindex;
    blob diff_seqnum;
    blob diff_time;
    filterconslist filtercons_s;


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

#endif // UTM_FILTERSETCONS_BASE_H
