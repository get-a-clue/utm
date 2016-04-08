#include "stdafx.h"
#include "accessrecord.h"

namespace utm {

const char accessrecord::this_class_name[] = "accessrecord";

accessrecord::accessrecord(void)
{
}

accessrecord::~accessrecord(void)
{
}

bool accessrecord::equals(const ubase* rhs) const
{
	const accessrecord* p = dynamic_cast<const accessrecord*>(rhs);
	if (p == NULL)
	{
		return false;
	}

	return operator==(*p);
}

bool accessrecord::operator==(const accessrecord& rhs) const
{
	return false;
}

std::string accessrecord::to_string(bool make_localization)
{
	std::ostringstream os;
	os << time.to_string(utime::format_log) << " " << client_ip.to_string() << " " << request_phase << " " << resume << " " << request_url << " " << referer;

	return os.str();
}

}