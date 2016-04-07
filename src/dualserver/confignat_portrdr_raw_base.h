// This is auto-generated file. Do not modify it!

#ifndef UTM_CONFIGNAT_PORTRDR_RAW_BASE_H
#define UTM_CONFIGNAT_PORTRDR_RAW_BASE_H

#pragma once
#include <utm.h>

#ifdef UTM_WIN
#include <Windows.h>
#include <tchar.h>
#endif



#define  CONFIGNAT_XMLTAG_PORTRDR "PortRdr"

namespace utm {



class confignat_portrdr_raw_base
{
public:
    static const char this_class_name[];

public:
    confignat_portrdr_raw_base();
    confignat_portrdr_raw_base(const confignat_portrdr_raw_base& rhs);
    virtual ~confignat_portrdr_raw_base();

    confignat_portrdr_raw_base& operator=(const confignat_portrdr_raw_base& rhs);
    virtual bool equals(const confignat_portrdr_raw_base* rhs) const;
    bool operator==(const confignat_portrdr_raw_base& rhs) const;

    virtual const char *get_this_class_name() const { return "confignat_portrdr_raw_base"; };



public:
    int proto;
    unsigned short public_port;
    unsigned short remote_port;
    unsigned long remote_ip;

    void clear();
};

}

#endif // UTM_CONFIGNAT_PORTRDR_RAW_BASE_H
