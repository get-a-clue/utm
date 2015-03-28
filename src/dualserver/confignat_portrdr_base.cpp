// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "confignat_portrdr_base.h"

namespace utm {

const char confignat_portrdr_base::this_class_name[] = "confignat_portrdr_base";

confignat_portrdr_base::confignat_portrdr_base()
{
    clear();
}

confignat_portrdr_base::confignat_portrdr_base(const confignat_portrdr_base& rhs)
{
    operator=(rhs);
}

confignat_portrdr_base::~confignat_portrdr_base()
{
}

confignat_portrdr_base& confignat_portrdr_base::operator=(const confignat_portrdr_base& rhs)
{
    proto = rhs.proto;
    public_port = rhs.public_port;
    remote_port = rhs.remote_port;
    remote_ip = rhs.remote_ip;
    return *this;
}

bool confignat_portrdr_base::operator==(const confignat_portrdr_base& rhs) const
{
    if (!(proto == rhs.proto)) return false;
    if (!(public_port == rhs.public_port)) return false;
    if (!(remote_port == rhs.remote_port)) return false;
    if (!(remote_ip == rhs.remote_ip)) return false;

    return true;
}

void confignat_portrdr_base::clear()
{
    proto = 0;
    public_port = 0;
    remote_port = 0;
    remote_ip = 0;
}

}
