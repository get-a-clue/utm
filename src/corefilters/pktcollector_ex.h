#ifndef _PKTCOLLECTOR_EX_H
#define _PKTCOLLECTOR_EX_H

#pragma once

#include "pktcollector.h"
#include "pktcollector_value_ex.h"

namespace utm {

typedef std::pair<pktcollector_key, pktcollector_value_ex> flush_record_ex;
typedef std::list<flush_record_ex> flush_container_ex;

class pktcollector_ex : public pktcollector<pktcollector_value_ex>
{
public:
	static const char this_class_name[];

public:
	pktcollector_ex();
	~pktcollector_ex();

#ifdef UTM_DEBUG
public:
	static void test_all();
#endif
};

}

#endif
