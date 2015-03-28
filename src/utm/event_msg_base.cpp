#include "stdafx.h"
#include "event_msg_base.h"

namespace utm {

event_msg_base::event_msg_base(void)
{
}


event_msg_base::~event_msg_base(void)
{
}

void event_msg_base::clear()
{
	id = 0;
	type = 0;
	time.clear();
}

}