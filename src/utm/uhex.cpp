#include "stdafx.h"
#include "uhex.h"

#include <stdio.h>
#include <time.h>
#include <exception>
#include <string>

#include <ubase_test.h>

namespace utm {

const char uhex::this_class_name[] = "uhex";
const char uhex::nibble_values[] = "0123456789abcdef";

#ifdef UTM_DEBUG
void uhex::test_all()
{
	test_report tr(this_class_name);

	char tmp[32];

	unsigned int k = 65537;
	memset(tmp, 0, sizeof(tmp));
	uhex::as_hex(k, tmp);
	TEST_CASE_CHECK(tmp, "00010001");

	unsigned char q2[4];
	uhex::parse_hex(tmp, 8, q2);
	unsigned char ethalon[4] = { 0, 1, 0, 1 };
	int res = memcmp((const void *)q2, (const void *)ethalon, 4);
	TEST_CASE_CHECK(res, 0);

	for (unsigned char c = 0; ; c++)
	{
		sprintf_s(tmp, 32, "%02x", c);
		TEST_CASE_CHECK(tmp[0], get_first_nibble(c));
		TEST_CASE_CHECK(tmp[1], get_second_nibble(c));

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
			tmp[0] = get_first_nibble(c);
			tmp[1] = get_second_nibble(c);
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
#endif

}