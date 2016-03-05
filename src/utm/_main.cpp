#include "stdafx.h"

#define BOOST_TEST_MAIN
#define BOOST_TEST_STATIC_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(SimpleTestInMainTestingModule)
{
	BOOST_REQUIRE(true);
}
