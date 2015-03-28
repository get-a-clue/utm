#include "stdafx.h"
#include "pktcollector_value.h"

namespace utm {

pktcollector_value::pktcollector_value()
{
	clear();
}

pktcollector_value::~pktcollector_value()
{
}

void pktcollector_value::clear()
{
	sent_fixed = 0;
	recv_fixed = 0;
	sent_flush = 0;
	recv_flush = 0;
	last_update = 0;
	flags = 0;
}

}