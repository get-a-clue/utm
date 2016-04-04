// This is auto-generated file. Do not modify it!

#ifndef UTM_PROC_ROW_BASE_H
#define UTM_PROC_ROW_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <gstring.h>
#include <utimestamp.h>

#define  PROCROW_XMLTAG_PROCROW "Pr"

namespace utm {



class proc_row_base : public ubase
{
public:
    static const char this_class_name[];

public:
    proc_row_base();
    proc_row_base(const proc_row_base& rhs);
    proc_row_base(const proc_row_base& rhs, bool make_threadsafe_copyobj);
    virtual ~proc_row_base();

    proc_row_base& operator=(const proc_row_base& rhs);
    virtual bool equals(const ubase* rhs) const;
    bool operator==(const proc_row_base& rhs) const;

    virtual const char *get_this_class_name() const { return "proc_row_base"; };



public:
    unsigned int id;
    unsigned int pid;
    unsigned short nick_id;
    std::uint64_t bytes_sent;
    std::uint64_t bytes_recv;
    unsigned short speed_sent;
    unsigned short speed_recv;
    utimestamp last_activity;
    gstring exename;
    gstring username;


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

#endif // UTM_PROC_ROW_BASE_H
