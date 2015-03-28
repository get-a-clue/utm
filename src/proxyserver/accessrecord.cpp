#include "stdafx.h"
#include "accessrecord.h"

namespace utm {

accessrecord::accessrecord(void)
{
}


accessrecord::~accessrecord(void)
{
}

std::string accessrecord::to_string(bool make_localization)
{
	std::ostringstream os;
	os << time.to_string(utime::format_log) << " " << client_ip.to_string() << " " << request_phase << " " << resume << " " << request_url << " " << referer;

	return os.str();
}

}