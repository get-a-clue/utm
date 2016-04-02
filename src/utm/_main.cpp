#include "stdafx.h"

#ifdef UTM_DEBUG

#include "ubase_test.h"
#include "adapterinfo.h"
#include "adapterlist.h"

#define BOOST_TEST_MAIN
#define BOOST_TEST_STATIC_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(SimpleTestInMainTestingModule)
{
	BOOST_REQUIRE(true);

	utm::ubase_test<utm::adapterinfo> test_adapterinfo;
	test_adapterinfo.test_all();

	utm::ubase_test<utm::adapterlist> test_adapterlist;
	test_adapterlist.test_all();

}

#endif
