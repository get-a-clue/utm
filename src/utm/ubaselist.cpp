#include "stdafx.h"
#include "ubaselist.h"
#include <boost/test/unit_test.hpp>

#ifdef UTM_DEBUG

int utm::testitem::destructor_calls = 0;

namespace utm {

void check_sequence(const ubaselist_test& ubt)
{
	for (auto iter = ubt.items.begin(); iter != ubt.items.end(); ++iter)
	{
		auto iter2 = iter;
		iter2++;
		if (iter2 == ubt.items.end())
			break;

		bool b = (iter->get()->get_id()) >= (iter2->get()->get_id());
		BOOST_REQUIRE_EQUAL(b, bool(false));
	}
}

BOOST_AUTO_TEST_CASE(ubaselist_test_all)
{
	BOOST_REQUIRE_EQUAL(0, testitem::destructor_calls);
	unsigned int elements_count = 0;
	unsigned int dcalls = 5;

	{
		ubaselist_test ubt;

		for (unsigned int i = 2; i < 22;)
		{
			testitem t(i);
			ubt.add_element(t);

			i += 4;
		}
		BOOST_REQUIRE_EQUAL(ubt.size(), size_t(5));
		BOOST_REQUIRE_EQUAL(dcalls, testitem::destructor_calls);

		int k = 0;
		for (unsigned int i = 2; i < 22;)
		{
			ubase* pu = ubt.findptr_by_index(k);
			BOOST_REQUIRE_EQUAL(true, pu != NULL);
			BOOST_REQUIRE_EQUAL(0, strcmp("testitem", pu->get_this_class_name()));
			BOOST_REQUIRE_EQUAL(i, pu->get_id());
			BOOST_REQUIRE_EQUAL(gstring("") << i << "A", pu->get_name());
			BOOST_REQUIRE_EQUAL(true, ubt.is_element_exist(i));
			BOOST_REQUIRE_EQUAL(i, ubt.findptr_by_index(k)->get_id());
			BOOST_REQUIRE_EQUAL(k, ubt.get_element_index(i));

			i += 4;
			k++;
		}

		{
			testitem t(8);
			ubt.insert_element_after(t, 0, 6);
			check_sequence(ubt);
			BOOST_REQUIRE_EQUAL(ubt.size(), size_t(6));
			BOOST_REQUIRE_EQUAL(8, ubt.findptr_by_id(8)->get_id());
			BOOST_REQUIRE_EQUAL(gstring("8A"), ubt.findptr_by_name(gstring("8A"))->get_name());
			BOOST_REQUIRE_EQUAL(10, ubt.findptr_next_by_id(8)->get_id());
		}
		BOOST_REQUIRE_EQUAL(++dcalls, testitem::destructor_calls);

		{
			testitem t(7);
			ubt.insert_element_after(t, 0, 6);
			check_sequence(ubt);
		}
		BOOST_REQUIRE_EQUAL(++dcalls, testitem::destructor_calls);

		{
			testitem t(9);
			ubt.insert_element_after(t, 0, 8);
			check_sequence(ubt);
		}
		BOOST_REQUIRE_EQUAL(++dcalls, testitem::destructor_calls);

		{
			testitem t(3);
			ubt.insert_element_after(t, 0, 2);
			check_sequence(ubt);
		}
		BOOST_REQUIRE_EQUAL(++dcalls, testitem::destructor_calls);

		{
			testitem t(19);
			ubt.insert_element_after(t, 0, 18);
			check_sequence(ubt);
		}
		BOOST_REQUIRE_EQUAL(++dcalls, testitem::destructor_calls);

		{
			testitem t(22);
			ubt.insert_element_after(t, 0, 19);
			check_sequence(ubt);
		}
		BOOST_REQUIRE_EQUAL(++dcalls, testitem::destructor_calls);

		{
			testitem t(30);
			ubt.insert_element_after(t, 0, 1);
			check_sequence(ubt);
		}
		BOOST_REQUIRE_EQUAL(++dcalls, testitem::destructor_calls);

		{
			testitem t(1);
			ubt.insert_element_before(t, 0, 2);
			check_sequence(ubt);
		}
		BOOST_REQUIRE_EQUAL(++dcalls, testitem::destructor_calls);

		{
			testitem t(29);
			ubt.insert_element_before(t, 0, 30);
			check_sequence(ubt);
		}
		BOOST_REQUIRE_EQUAL(++dcalls, testitem::destructor_calls);

		{
			testitem t(28);
			ubt.insert_element_before(t, 0, 29);
			check_sequence(ubt);
		}
		BOOST_REQUIRE_EQUAL(++dcalls, testitem::destructor_calls);

		{
			BOOST_REQUIRE_EQUAL(9, ubt.findptr_next_by_id(8)->get_id());
			ubt.move_up(8);
			BOOST_REQUIRE_EQUAL(7, ubt.findptr_next_by_id(8)->get_id());
			ubt.move_up(8);
			BOOST_REQUIRE_EQUAL(6, ubt.findptr_next_by_id(8)->get_id());
			BOOST_REQUIRE_EQUAL(7, ubt.findptr_next_by_id(6)->get_id());
			BOOST_REQUIRE_EQUAL(9, ubt.findptr_next_by_id(7)->get_id());
		}
		BOOST_REQUIRE_EQUAL(dcalls, testitem::destructor_calls);

		{
			ubt.move_down(8);
			ubt.move_down(8);
			BOOST_REQUIRE_EQUAL(7, ubt.findptr_next_by_id(6)->get_id());
			BOOST_REQUIRE_EQUAL(8, ubt.findptr_next_by_id(7)->get_id());
			BOOST_REQUIRE_EQUAL(9, ubt.findptr_next_by_id(8)->get_id());
		}
		BOOST_REQUIRE_EQUAL(dcalls, testitem::destructor_calls);

		{
			ubt.delete_element(8);
			BOOST_REQUIRE_EQUAL(7, ubt.findptr_next_by_id(6)->get_id());
			BOOST_REQUIRE_EQUAL(true, ubt.findptr_by_id(8) == NULL);
			BOOST_REQUIRE_EQUAL(9, ubt.findptr_next_by_id(7)->get_id());
		}
		BOOST_REQUIRE_EQUAL(++dcalls, testitem::destructor_calls);

		{
			int next_id = ubt.get_next_id();
			BOOST_REQUIRE_EQUAL(31, next_id);
		}

		{
			testitem* ti = new testitem(28);
			ti->set_name(gstring("28B"));
			ubt.modify_element(ti);

			ubase* ti2 = ubt.findptr_by_id(28);
			BOOST_REQUIRE_EQUAL(true, ti2 != NULL);
			BOOST_REQUIRE_EQUAL(28, ti2->get_id());
			BOOST_REQUIRE_EQUAL(gstring("28B"), ti2->get_name());
		}
		BOOST_REQUIRE_EQUAL(++dcalls, testitem::destructor_calls);

		elements_count = ubt.size();
	}
	BOOST_REQUIRE_EQUAL(14, elements_count);
	BOOST_REQUIRE_EQUAL(dcalls + elements_count, testitem::destructor_calls);

	{
		ubaselist_test ubt;

		for (unsigned int i = 2; i < 7; i++)
		{
			testitem t(i);
			ubt.add_element(t);
		}
		BOOST_REQUIRE_EQUAL(ubt.size(), size_t(5));

		ubaselist_test ubt2;
		ubt2 = ubt;
		BOOST_REQUIRE_EQUAL(ubt2.size(), size_t(5));
		BOOST_REQUIRE_EQUAL(true, ubt == ubt2);
		BOOST_REQUIRE_EQUAL(true, ubt.equals(&ubt2));

		ubaselist_test ubt3(ubt);
		BOOST_REQUIRE_EQUAL(ubt3.size(), size_t(5));
		BOOST_REQUIRE_EQUAL(true, ubt == ubt3);
		BOOST_REQUIRE_EQUAL(true, ubt.equals(&ubt3));

		testitem* ti2 = dynamic_cast<testitem *>(ubt2.findptr_by_index(0));
		ti2->set_name(utm::gstring("TEST_TEST"));

		testitem* ti2_2 = dynamic_cast<testitem *>(ubt2.findptr_by_index(0));
		BOOST_REQUIRE_EQUAL(0, strcmp("TEST_TEST", ti2_2->get_name().getmb().c_str()));

		testitem* ti0_2 = dynamic_cast<testitem *>(ubt.findptr_by_index(0));
		BOOST_REQUIRE_EQUAL(0, strcmp("2A", ti0_2->get_name().getmb().c_str()));

		BOOST_REQUIRE_EQUAL(false, ubt == ubt2);
		BOOST_REQUIRE_EQUAL(false, ubt.equals(&ubt2));
	}

	{
		ubaselist_main_test ubmain;
		BOOST_REQUIRE_EQUAL(0, ubmain.test_items.size());

		ubaselist_main_test ubmain2;
		BOOST_REQUIRE_EQUAL(0, ubmain2.test_items.size());

		ubmain = ubmain2;
		BOOST_REQUIRE_EQUAL(0, ubmain.test_items.size());

		BOOST_REQUIRE_EQUAL(true, ubmain == ubmain2);

	}

	return;
}

}

#endif
