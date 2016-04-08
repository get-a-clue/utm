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

bool squidrecord::equals(const ubase* rhs) const
{
	const squidrecord* p = dynamic_cast<const squidrecord*>(rhs);
	if (p == NULL)
	{
		return false;
	}

	return operator==(*p);
}

bool squidrecord::operator==(const squidrecord& rhs) const
{
	if (!(timestamp == rhs.timestamp)) return false;
	if (!(elapsed == rhs.elapsed)) return false;
	if (client != rhs.client) return false;
	if (action != rhs.action) return false;
	if (code != rhs.code) return false;
	if (size != rhs.size) return false;
	if (method != rhs.method) return false;
	if (uri != rhs.uri) return false;
	if (ident != rhs.ident) return false;
	if (hierarchy != rhs.hierarchy) return false;
	if (from != rhs.from) return false;
	if (content != rhs.content) return false;

	return true;
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
