// This is auto-generated file. Do not modify it!

#ifndef UTM_SMS_BASE_H
#define UTM_SMS_BASE_H

#pragma once
#include <utm.h>

#include <time.h>
#include <ubase.h>
#include <gstring.h>
#include <idset.h>

#define  SMS_XMLTAG_ROOT "Sms"

namespace utm {



class sms_base : public ubase
{
public:
    static const char this_class_name[];

public:
    sms_base();
    sms_base(const sms_base& rhs);
    sms_base(const sms_base& rhs, bool make_threadsafe_copyobj);
    virtual ~sms_base();

    sms_base& operator=(const sms_base& rhs);
    bool  operator==(const sms_base& rhs) const;



public:
    unsigned int sender_uid;
    gstring sender_name;
    unsigned int recp_uid;
    gstring recp_name;
    time_t time_creation;
    time_t time_expire;
    gstring msg_text;
    std::string msg_id;


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

#endif // UTM_SMS_BASE_H
