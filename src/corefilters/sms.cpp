#include "StdAfx.h"
#include "sms.h"

#include <md5.h>

namespace utm {

const char sms::this_class_name[] = "sms";


sms::sms(void)
{
}


sms::~sms(void)
{
}

void sms::generate_msg_id()
{
	time(&time_creation);
	time_expire = time_creation + (4 * 86400);

	int rnd = utm::get_random(1, 0x7FFFFFFF);

	char tmp[128];
	sprintf_s(tmp, 128, "%d_%u_%u_%u", rnd, time_creation, sender_uid, recp_uid);

	md5::md5_string(tmp, msg_id);
}

}