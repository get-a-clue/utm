// This is auto-generated file. Do not modify it!

#ifndef UTM_FSUSER_BASE_H
#define UTM_FSUSER_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <gstring.h>
#include <idset.h>

#define  FSUSER_XMLTAG_ROOT "User"
#define  USER_ACCESS_SPECIFIED_FILTERS 0
#define  USER_ACCESS_ALL_FILTERS 1
#define  USER_AUTHMODE_NATIVE 0
#define  USER_AUTHMODE_DOMAIN 1
#define  USER_ACCOUNT_ENABLED 0
#define  USER_ACCOUNT_DISABLED 1
#define  USER_ACCOUNT_WILLEXPIRE 2

namespace utm {



class fsuser_base : public ubase
{
public:
    static const char this_class_name[];

public:
    fsuser_base();
    fsuser_base(const fsuser_base& rhs);
    fsuser_base(const fsuser_base& rhs, bool make_threadsafe_copyobj);
    virtual ~fsuser_base();

    fsuser_base& operator=(const fsuser_base& rhs);
    virtual bool equals(const ubase* rhs) const;
    bool operator==(const fsuser_base& rhs) const;

    virtual const char *get_this_class_name() const { return "fsuser_base"; };

virtual  std::string get_filter_ids_str() const { return fids.to_string(); };
virtual  void parse_filter_ids(const char *p) { fids.from_string(p); };

public:
    unsigned int m_uid;
    gstring m_szName;
    gstring m_szDescr;
    std::string m_szPasswd;
    int m_nAuthMode;
    int m_nStatus;
    int m_nExpireDt;
    int m_nManageFilters;
    int m_nType;
    int m_nDenyDoubleLogins;
    idset fids;


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

#endif // UTM_FSUSER_BASE_H
