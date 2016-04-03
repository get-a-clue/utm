#include "StdAfx.h"
#include "fsuser.h"

namespace utm {

const char fsuser::this_class_name[] = "fsuser";

fsuser::fsuser(void)
{
}

fsuser::fsuser(const gstring& _username)
{
	m_szName = _username;
}

fsuser::~fsuser(void)
{
}

bool fsuser::is_user_expired(const utime& now) const
{
	int year, month, day, hour, minute, second;
	now.get(year, month, day, hour, minute, second);

	// Current timestamp as int (yMMddHHmm)
	int nowts = minute + (hour * 100) + (day * 10000) + (month * 1000000) + ((year - 2000) * 100000000);

	return (m_nExpireDt < nowts) ? true : false;
}

bool fsuser::is_user_enabled(const utime& now) const
{
	if (m_nStatus == USER_ACCOUNT_DISABLED)
		return false;

	if ((m_nStatus == USER_ACCOUNT_WILLEXPIRE) && (is_user_expired(now)))
		return false;

	return true;
}

}
