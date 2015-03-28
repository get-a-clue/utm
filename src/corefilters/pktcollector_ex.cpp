#include "stdafx.h"
#include "pktcollector_ex.h"

#include <ubase_test.h>

namespace utm {

const char pktcollector_ex::this_class_name[] = "pktcollector_ex";

pktcollector_ex::pktcollector_ex()
{
}


pktcollector_ex::~pktcollector_ex()
{
}

#ifdef UTM_DEBUG
void pktcollector_ex::test_all()
{
	test_report tr(this_class_name);

}
#endif

}