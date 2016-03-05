#include "stdafx.h"
#include "fastformat.h"

#include <boost/test/unit_test.hpp>

namespace utm {

fastformat::fastformat()
{
}


fastformat::~fastformat()
{
}

short fastformat::parse_short(const char *p)
{
	short retval = 0;
	parse_signed(p, retval);
	return retval;
}

unsigned short fastformat::parse_ushort(const char *p)
{
	unsigned short retval = 0;
	parse_unsigned(p, retval);
	return retval;
}

int fastformat::parse_int(const char *p)
{
	int retval = 0;
	parse_signed(p, retval);
	return retval;
}

unsigned int fastformat::parse_uint(const char *p)
{
	unsigned int retval = 0;
	parse_unsigned(p, retval);
	return retval;
}

std::int64_t fastformat::parse_int64(const char *p)
{
	std::int64_t retval = 0;
	parse_signed(p, retval);
	return retval;
}

std::uint64_t fastformat::parse_uint64(const char *p)
{
	std::uint64_t retval = 0;
	parse_unsigned(p, retval);
	return retval;
}

BOOST_AUTO_TEST_CASE(fastformat_test_all)
{
	BOOST_REQUIRE_EQUAL(1, utm::fastformat::parse_int("1"));
	BOOST_REQUIRE_EQUAL(1000, utm::fastformat::parse_int("1000"));
	BOOST_REQUIRE_EQUAL(2147483647, utm::fastformat::parse_int("2147483647"));
	BOOST_REQUIRE_EQUAL(-2147483647, utm::fastformat::parse_int("-2147483647"));
	BOOST_REQUIRE_EQUAL(-2147483647123, utm::fastformat::parse_int64("-2147483647123"));
	BOOST_REQUIRE_EQUAL(std::uint64_t(3147483647123), utm::fastformat::parse_uint64("3147483647123"));
}

}
