// This is auto-generated file. Do not modify it!

#ifndef **guard**
#define **guard**

#pragma once
#include <utm.h>

**include**

**define**

namespace utm {

**typedef**

class $classname : public ubase
{
public:
    static const char this_class_name[];

public:
    $classname();
    $classname(const $classname& rhs);
    $classname(const $classname& rhs, bool make_threadsafe_copyobj);
    virtual ~$classname();

    $classname& operator=(const $classname& rhs);
    bool  operator==(const $classname& rhs) const;

    virtual const char *get_this_class_name() const { return "$classname"; };

**virtual**

public:
**members**
**member_threadsafe_rw**

public:
    void clear();
    void xml_create();
    void xml_catch_value(const char *keyname, const char *keyvalue);
    virtual ubase* xml_catch_subnode(const char *name) { return NULL; };

**method_threadsafe_rw**

#ifdef UTM_WIN
**has_registry_true**
    virtual LONG SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
    virtual LONG ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
#endif // UTM_WIN
};

}

#endif // **guard**