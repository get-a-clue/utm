#include "stdafx.h"
#include "trafficreport.h"

namespace utm {

trafficreport::trafficreport()
{
}


trafficreport::~trafficreport()
{
}

ubase* trafficreport::xml_catch_subnode(const char *keyname)
{
	ubase *u = NULL;

	if (strcmp(keyname, TR_FILTER_XMLTAG_ROOT) == 0)
	{
		u = (ubase *)filters.get_temp_item();
		return u;
	}

	return NULL;
};

void trafficreport::xml_catch_subnode_finished(const char *keyname)
{
	if (strcmp(keyname, TR_FILTER_XMLTAG_ROOT) == 0)
	{
		filters.commit_temp_item();
		return;
	}
}

void trafficreport::update_counters(const utime& ctm, const counterdata& cdata)
{
	bool found = false;
	for (auto iter = filters.items.begin(); iter != filters.items.end(); ++iter)
	{
		if (iter->get_id() == cdata.filterid)
		{
			iter->update_counters(ctm, cdata.sent, cdata.recv, cdata.filtername);
			found = true;
			break;
		}
	}

	if (!found)
	{
		trafficreport_filter trf;
		trf.set_id(cdata.filterid);
		trf.update_counters(ctm, cdata.sent, cdata.recv, cdata.filtername);
		filters.add_element(trf);
	}
}

void trafficreport::update_mass_counters(const utime& ctm, const std::list<counterdata>& cdatas)
{
	for (auto iter = cdatas.begin(); iter != cdatas.end(); ++iter)
	{
		update_counters(ctm, *iter);
	}
}

bool trafficreport::do_refresh(const char *xmlfile, const utime& ctm, const std::list<counterdata>& cdatas)
{
	trafficreport tr;

	try
	{
		tr.xml_load(xmlfile);
	}
	catch (std::exception&)
	{
		// unable to open file for reading
	}

	tr.update_mass_counters(ctm, cdatas);
	
	try
	{
		tr.xml_save(xmlfile);
	}
	catch (std::exception&)
	{
		// unable to save file
		return false;
	}

	return true;
}

#ifdef UTM_DEBUG
void trafficreport::test_all()
{
	trafficreport tr;

	trafficreport_filter f;
	f.set_id(25);
	f.name.assign("Test filter name");
	f.sent = 123456;
	f.recv = 654321;

	trafficreport_daytick dtick = trafficreport_daytick::test_get_daytick();
	f.dayticks.items.push_back(dtick);

	tr.filters.add_element(f);

	std::string x;
	tr.xml_create();
	tr.xml_get_string(x);

	trafficreport tr2;
	tr2.xml_parse(x.c_str());

	bool b = (tr == tr2);
	TEST_CASE_CHECK(b, true);

	utime ctm;
	ctm.set(2014, 01, 31, 10, 33, 00);

	counterdata cd1(29, 777, 888, gstring("Test Filter Name"));
	tr.update_counters(ctm, cd1);
	std::string x2;
	tr.xml_create();
	tr.xml_get_string(x2);

	counterdata cd2(29, 0, 12, gstring("Test Filter Name"));
	tr.update_counters(ctm, cd2);
	std::string x3;
	tr.xml_create();
	tr.xml_get_string(x3);

	counterdata cd3(30, 0, 12, gstring("Test Filter Name - 2"));

	std::list<counterdata> datas;
	datas.push_back(cd1);
	datas.push_back(cd2);
	datas.push_back(cd3);

	trafficreport tr3;
	tr3.update_mass_counters(ctm, datas);
	std::string x5;
	tr3.xml_create();
	tr3.xml_get_string(x5);

	trafficreport tr4;
	tr4.xml_parse(x5.c_str());
	std::string x6;
	tr4.xml_create();
	tr4.xml_get_string(x6);

	TEST_CASE_CHECK(x5, x6);

	tr4.update_mass_counters(ctm, datas);
	std::string x7;
	tr4.xml_create();
	tr4.xml_get_string(x7);

	return;
}
#endif

}