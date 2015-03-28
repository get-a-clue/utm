#include "stdafx.h"
#include "googlerecord.h"

#include <iostream>
#include <sstream>

namespace utm {

googlerecord::googlerecord(void)
{
}


googlerecord::~googlerecord(void)
{
}

std::string googlerecord::to_string(bool make_localization)
{
	std::ostringstream oss;
	oss << time.to_string(utime::format_log) << " " << event_type::c_str(type) << " " << result_code << " " << site << " " << msg;

	return oss.str();
}

}