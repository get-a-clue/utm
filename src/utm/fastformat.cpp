#include "stdafx.h"
#include "fastformat.h"

#include "ubase_test.h"

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

void fastformat::test_all()
{
	TEST_CASE_CHECK(1, utm::fastformat::parse_int("1"));
	TEST_CASE_CHECK(1000, utm::fastformat::parse_int("1000"));
	TEST_CASE_CHECK(2147483647, utm::fastformat::parse_int("2147483647"));
	TEST_CASE_CHECK(-2147483647, utm::fastformat::parse_int("-2147483647"));
	TEST_CASE_CHECK(-2147483647123, utm::fastformat::parse_int64("-2147483647123"));
	TEST_CASE_CHECK(std::uint64_t(3147483647123), utm::fastformat::parse_uint64("3147483647123"));
}

}
