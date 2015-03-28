#ifndef _PKTCOLLECTOR_E_H
#define _PKTCOLLECTOR_E_H

#pragma once

#include "pktcollector.h"
#include "pktcollector_value.h"

namespace utm {

typedef std::pair<pktcollector_key, pktcollector_value> flush_record_e;
typedef std::list<flush_record_e> flush_container_e;

class pktcollector_e : public pktcollector<pktcollector_value>
{
public:
	static const char this_class_name[];

public:
	pktcollector_e();
	~pktcollector_e();

#ifdef UTM_DEBUG
public:
	static void test_all();
#endif
};

}

#endif
