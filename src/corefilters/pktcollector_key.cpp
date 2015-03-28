#include "stdafx.h"
#include "pktcollector_key.h"

#include <tuple>

namespace utm {

pktcollector_key::pktcollector_key()
{
	proto = 0;
	src_port = 0;
	dst_port = 0;
	src_addr = 0;
	dst_addr = 0;
}

pktcollector_key::pktcollector_key(unsigned short proto, unsigned short src_port, unsigned short dst_port, unsigned int src_addr, unsigned int dst_addr)
{
	this->proto = proto;
	this->src_port = src_port;
	this->dst_port = dst_port;
	this->src_addr = src_addr;
	this->dst_addr = dst_addr;
}

pktcollector_key::~pktcollector_key()
{
}

bool pktcollector_key::operator<(const pktcollector_key& rhs) const
{
	return std::tie(proto, src_addr, dst_addr, src_port, dst_port) < std::tie(rhs.proto, rhs.src_addr, rhs.dst_addr, rhs.src_port, rhs.dst_port);
//	if (proto < rhs.proto) return true;
//	if (src_addr < rhs.src_addr) return true;
//	if (dst_addr < rhs.dst_addr) return true;
//	if (src_port < rhs.src_port) return true;
//	if (dst_port < rhs.dst_port) return true;

//	return false;
}

bool pktcollector_key::operator==(const pktcollector_key& rhs) const
{
	if (proto != rhs.proto) return false;
	if (src_addr != rhs.src_addr) return false;
	if (dst_addr != rhs.dst_addr) return false;
	if (src_port != rhs.src_port) return false;
	if (dst_port != rhs.dst_port) return false;

	return true;
}

}