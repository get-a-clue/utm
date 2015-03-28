// This is auto-generated file. Do not modify it!

#ifndef **guard**
#define **guard**

#pragma once
#include <utm.h>

#ifdef UTM_WIN
#include <Windows.h>
#include <tchar.h>
#endif

**include**

**define**

namespace utm {

**typedef**

class $classname
{
public:
    static const char this_class_name[];

public:
    $classname();
    $classname(const $classname& rhs);
    virtual ~$classname();

    $classname& operator=(const $classname& rhs);
    bool  operator==(const $classname& rhs) const;

**virtual**

public:
**members**

    void clear();
};

}

#endif // **guard**
