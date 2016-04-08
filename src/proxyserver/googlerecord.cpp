#include "stdafx.h"
#include "googlerecord.h"

#include <iostream>
#include <sstream>

namespace utm {

const char googlerecord::this_class_name[] = "googlerecord";

googlerecord::googlerecord(void)
{
}


googlerecord::~googlerecord(void)
{
}

bool googlerecord::equals(const ubase* rhs) const
{
	const googlerecord *p = dynamic_cast<const googlerecord*>(rhs);
	if (p == NULL)
	{
		return false;
	}

	return operator==(*p);
}

bool googlerecord::operator==(const googlerecord& rhs) const
{
	if (msg != rhs.msg) return false;
	if (site != rhs.site) return false;
	if (result_code != rhs.result_code) return false;

	return true;
}

std::string googlerecord::to_string(bool make_localization)
{
	std::ostringstream oss;
	oss << time.to_string(utime::format_log) << " " << event_type::c_str(type) << " " << result_code << " " << site << " " << msg;

	return oss.str();
}

}