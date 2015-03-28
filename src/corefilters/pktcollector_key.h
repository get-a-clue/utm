#ifndef _PKTCOLLECTOR_KEY_H
#define _PKTCOLLECTOR_KEY_H

#pragma once

namespace utm {

class pktcollector_key
{
public:
	pktcollector_key();
	pktcollector_key(unsigned short proto, unsigned short src_port, unsigned short dst_port, unsigned int src_addr, unsigned int dst_addr);
	~pktcollector_key();

	unsigned short proto;
	unsigned short src_port;
	unsigned short dst_port;
	unsigned int src_addr;
	unsigned int dst_addr;

	bool operator<(const pktcollector_key& rhs) const;
	bool operator==(const pktcollector_key& rhs) const;
};

}

#endif _PKTCOLLECTOR_KEY_H
