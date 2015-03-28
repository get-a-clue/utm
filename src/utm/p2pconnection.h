#ifndef _UTM_P2PCONNECTION_H
#define _UTM_P2PCONNECTION_H

#pragma once

#include <addrip_v4.h>

namespace utm {

template<class T>
class p2pconnection
{
public:
	p2pconnection(unsigned short _port1, unsigned short _port2, T _addr1, T _addr2) 
		: port1(_port1), port2(_port2), addr1(_addr1), addr2(_addr2) {};

	~p2pconnection() {};

	unsigned short port1;
	unsigned short port2;
	T addr1;
	T addr2;

	bool operator<(const p2pconnection<T>& rhs) const
	{
		if (port1 < rhs.port1)
			return true;
		else if (rhs.port1 < port1)
			return false;

		if (port2 < rhs.port2)
			return true;
		else if (rhs.port2 < port2)
			return false;

		if (addr1 < rhs.addr1)
			return true;
		else if (rhs.addr1 < addr1)
			return false;

		if (addr2 < rhs.addr2)
			return true;

		return false;
	}
};

typedef p2pconnection<addrip_v4> p2pconnection_v4;

}

#endif // _UTM_P2PCONNECTION_H

