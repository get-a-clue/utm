#include "stdafx.h"
#include "uhex.h"

#include <stdio.h>
#include <time.h>
#include <exception>
#include <string>

#include <boost/test/unit_test.hpp>

namespace utm {

const char uhex::this_class_name[] = "uhex";
const char uhex::nibble_values[] = "0123456789abcdef";

BOOST_AUTO_TEST_CASE(uhex_test_all)
{
	

	char tmp[32];

	unsigned int k = 65537;
	memset(tmp, 0, sizeof(tmp));
	uhex::as_hex(k, tmp);
	BOOST_REQUIRE_EQUAL(tmp, "00010001");

	unsigned char q2[4];
	uhex::parse_hex(tmp, 8, q2);
	unsigned char ethalon[4] = { 0, 1, 0, 1 };
	int res = memcmp((const void *)q2, (const void *)ethalon, 4);
	BOOST_REQUIRE_EQUAL(res, 0);

	for (unsigned char c = 0; ; c++)
	{
		sprintf_s(tmp, 32, "%02x", c);
		BOOST_REQUIRE_EQUAL(tmp[0], uhex::get_first_nibble(c));
		BOOST_REQUIRE_EQUAL(tmp[1], uhex::get_second_nibble(c));

		if (c == 255)
			break;
	}

	// Performance test. sprint sucks.

	int i, max = 1/* 000000 */;

	time_t t1, t2, t3;
	time(&t1);

	for (i = 0; i < max; i++)
	{
		for (unsigned char c = 0; ; c++)
		{
			sprintf_s(tmp, 8, "%02x", c);

			if (c == 255)
				break;
		}
	}

	time(&t2);

	for (i = 0; i < max; i++)
	{
		for (unsigned char c = 0; ; c++)
		{
			tmp[0] = uhex::get_first_nibble(c);
			tmp[1] = uhex::get_second_nibble(c);
			tmp[2] = 0;

			if (c == 255)
				break;
		}
	}

	time(&t3);

	time_t diff1 = t2 - t1;
	time_t diff2 = t3 - t2;

	return;
}

}