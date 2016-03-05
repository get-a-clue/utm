#include "stdafx.h"
#include "ubaselist.h"
#include <boost/test/unit_test.hpp>

#ifdef UTM_DEBUG

namespace utm {

void check_sequence(const ubaselist_test& ubt)
{
	for (auto iter = ubt.items.begin(); iter != ubt.items.end(); ++iter)
	{
		auto iter2 = iter;
		iter2++;
		if (iter2 == ubt.items.end())
			break;

		bool b = ((*iter).get_id() >= (*iter2).get_id());
		BOOST_REQUIRE_EQUAL(b, bool(false));
	}
}

BOOST_AUTO_TEST_CASE(ubaselist_test_all)
{
	ubaselist_test ubt;

	for (unsigned int i = 2; i < 22;)
	{
		testitem t(i);
		ubt.add_element(t);

		i += 4;
	}

	BOOST_REQUIRE_EQUAL(ubt.size(), size_t(5));

	{
		testitem t(8);
		ubt.insert_element_after(t, 0, 6);
		check_sequence(ubt);
	}

	{
		testitem t(7);
		ubt.insert_element_after(t, 0, 6);
		check_sequence(ubt);
	}

	{
		testitem t(9);
		ubt.insert_element_after(t, 0, 8);
		check_sequence(ubt);
	}

	{
		testitem t(3);
		ubt.insert_element_after(t, 0, 2);
		check_sequence(ubt);
	}

	{
		testitem t(19);
		ubt.insert_element_after(t, 0, 18);
		check_sequence(ubt);
	}

	{
		testitem t(22);
		ubt.insert_element_after(t, 0, 19);
		check_sequence(ubt);
	}

	{
		testitem t(30);
		ubt.insert_element_after(t, 0, 1);
		check_sequence(ubt);
	}

	{
		testitem t(1);
		ubt.insert_element_before(t, 0, 2);
		check_sequence(ubt);
	}

	{
		testitem t(29);
		ubt.insert_element_before(t, 0, 30);
		check_sequence(ubt);
	}

	{
		testitem t(28);
		ubt.insert_element_before(t, 0, 29);
		check_sequence(ubt);
	}


	return;
}

}

#endif
