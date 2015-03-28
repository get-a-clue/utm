#include "StdAfx.h"
#include "squidrecord.h"

namespace utm {

squidrecord::squidrecord(void)
{
	action = "TCP_MISS";
	code = 0;
	ident = "-";
	hierarchy = "DIRECT";
}


squidrecord::~squidrecord(void)
{
}

std::string squidrecord::to_string(bool make_localization)
{
	std::ostringstream os;

	boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
	boost::posix_time::time_duration::sec_type sec = (timestamp - epoch).total_seconds();
	boost::posix_time::ptime now = boost::posix_time::from_time_t(sec);
	boost::posix_time::time_duration msec = timestamp - now;

	os << sec << "." << std::setfill('0') << std::setw(3) << msec.total_milliseconds() << " ";
	os << std::setw(0) << elapsed.total_milliseconds() << " " << client << " ";
	os << action << "/" << std::setfill('0') << std::setw(3) << code << " " << std::setw(0) << size << " ";
	os << method << " " << uri << " " << ident << " " << hierarchy << "/" << from << " " << content;

	return os.str();
}

}
