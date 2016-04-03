#include "stdafx.h"

#include "addrtablemap_v4.h"
#include "addrpair_v4.h"
#include "addrtable_v4.h"
#include <boost/test/unit_test.hpp>

namespace utm {

#ifdef UTM_DEBUG

BOOST_AUTO_TEST_CASE(addrtablemap_v4_test_all)
{
	addrtablemap_v4 map_v4_0;
	addrtablemaprec_v4* rec_v4_0 = new addrtablemaprec_v4();
	rec_v4_0->set_id(1);
	map_v4_0.add_element(rec_v4_0);
	map_v4_0.AddAddrPair(1, "192.168.0.1", "192.168.0.255");
	map_v4_0.AddAddrPair(1, "192.168.1.1", "192.168.1.255");

	addrtablemap_v4 map_v4;
	BOOST_REQUIRE_EQUAL(0, map_v4.size());

	addrtablemaprec_v4* rec_v4 = new addrtablemaprec_v4();
	rec_v4->set_id(1);
	rec_v4->addrtable.AddAddrPair("192.168.0.1", "192.168.0.255", true);
	rec_v4->addrtable.AddAddrPair("192.168.1.1", "192.168.1.255", true);
	map_v4.add_element(rec_v4);

	BOOST_REQUIRE_EQUAL(true, map_v4 == map_v4_0);

	BOOST_REQUIRE_EQUAL(1, map_v4.size());
	addrtablemaprec_v4* rec2_v4 = dynamic_cast<addrtablemaprec_v4 *>(map_v4.findptr_by_id(1));
	BOOST_REQUIRE_EQUAL(true, rec2_v4 != NULL);
	BOOST_REQUIRE_EQUAL(2, rec2_v4->addrtable.itemsref().size());

//	utm::ubaselist<utm::addrtablemaprec<utm::addrtable_v4>>& items = map_v4.items;
	std::list<std::unique_ptr<utm::ubase>>& items = map_v4.items;
	BOOST_REQUIRE_EQUAL(1, items.size());

	addrip_v4 a1, a2;
	rec2_v4->addrtable.GetAddrPair(0, a1, a2);
	BOOST_REQUIRE_EQUAL(0, strcmp(a1.to_string().c_str(), "192.168.0.1"));
	BOOST_REQUIRE_EQUAL(0, strcmp(a2.to_string().c_str(), "192.168.0.255"));
	rec2_v4->addrtable.GetAddrPair(1, a1, a2);
	BOOST_REQUIRE_EQUAL(0, strcmp(a1.to_string().c_str(), "192.168.1.1"));
	BOOST_REQUIRE_EQUAL(0, strcmp(a2.to_string().c_str(), "192.168.1.255"));

	std::string xml;
	map_v4.xml_create();
	map_v4.xml_get_string(xml, true);

	addrtablemap_v4 map_v4_2;
	map_v4_2.xml_parse(xml.c_str());
	BOOST_REQUIRE_EQUAL(1, map_v4_2.size());

	BOOST_REQUIRE_EQUAL(true, map_v4 == map_v4_2);


	return;
}

#endif

}