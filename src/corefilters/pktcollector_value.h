#ifndef _PKTCOLLECTOR_VALUE_H
#define _PKTCOLLECTOR_VALUE_H

#pragma once

#include <cstdint>
#include <ip_header.h>

namespace utm {

class pktcollector_value
{
public:
	pktcollector_value();
	~pktcollector_value();

	std::uint64_t sent_fixed;
	std::uint64_t recv_fixed;
	std::uint64_t sent_flush;
	std::uint64_t recv_flush;
	time_t last_update;
	unsigned int flags;

private:
	inline void clear();
};

}

#endif // _PKTCOLLECTOR_VALUE_H