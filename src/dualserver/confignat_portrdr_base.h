// This is auto-generated file. Do not modify it!

#ifndef UTM_CONFIGNAT_PORTRDR_BASE_H
#define UTM_CONFIGNAT_PORTRDR_BASE_H

#pragma once
#include <utm.h>

#ifdef UTM_WIN
#include <Windows.h>
#include <tchar.h>
#endif



#define  CONFIGNAT_XMLTAG_PORTRDR "PortRdr"

namespace utm {



class confignat_portrdr_base
{
public:
    static const char this_class_name[];

public:
    confignat_portrdr_base();
    confignat_portrdr_base(const confignat_portrdr_base& rhs);
    virtual ~confignat_portrdr_base();

    confignat_portrdr_base& operator=(const confignat_portrdr_base& rhs);
    bool  operator==(const confignat_portrdr_base& rhs) const;



public:
    int proto;
    unsigned short public_port;
    unsigned short remote_port;
    unsigned long remote_ip;

    void clear();
};

}

#endif // UTM_CONFIGNAT_PORTRDR_BASE_H
