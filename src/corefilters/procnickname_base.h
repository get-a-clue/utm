// This is auto-generated file. Do not modify it!

#ifndef UTM_PROCNICKNAME_BASE_H
#define UTM_PROCNICKNAME_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <gstring.h>

#define  PROCNICKNAME_XMLTAG_ROOT "Proc"

namespace utm {



class procnickname_base : public ubase
{
public:
    static const char this_class_name[];

public:
    procnickname_base();
    procnickname_base(const procnickname_base& rhs);
    procnickname_base(const procnickname_base& rhs, bool make_threadsafe_copyobj);
    virtual ~procnickname_base();

    procnickname_base& operator=(const procnickname_base& rhs);
    bool  operator==(const procnickname_base& rhs) const;



public:
    unsigned int nid;
    gstring nickname;
    gstring pathexe;


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

#endif // UTM_PROCNICKNAME_BASE_H
