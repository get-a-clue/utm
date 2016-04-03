#include "stdafx.h"
#include "trafficreport_hourtick.h"

#include <ubase_test.h>

namespace utm {

trafficreport_hourtick::trafficreport_hourtick()
{
}


trafficreport_hourtick::~trafficreport_hourtick()
{
}

bool trafficreport_hourtick::xml_has_root_attr() const
{ 
	return true; 
}

void trafficreport_hourtick::xml_catch_subnode_attribute(const char *attrname, const char* attrvalue)
{
	xml_catch_rootnode_attribute(attrname, attrvalue);
}

void trafficreport_hourtick::xml_catch_rootnode_attribute(const char *attrname, const char* attrvalue)
{
	if (strcmp(attrname, "dt") == 0)
	{
		trafficreport_hourtick::hourts2utime(attrvalue, ut);
	}
}

void trafficreport_hourtick::xml_get_root_attr(xmlattr_container& attr)
{
	std::string s;
	trafficreport_hourtick::utime2hourts(ut, s);
	attr.insert(std::make_pair<std::string, std::string>(std::string("dt"), std::string(s)));
};

void trafficreport_hourtick::utime2hourts(const utime& ut, std::string& str)
{
	unsigned short y = ut.get_year() % 100;
	unsigned short m = ut.get_month();
	unsigned short d = ut.get_day();
	unsigned short ho = (ut.get_hours() / 2) * 2;
	unsigned short mi = /*ut.get_minutes()*/ 0;

	char s[64];
	sprintf_s(s, 64, "%02d%02d%02d %02d:%02d", y, m, d, ho, mi);
	str.assign(s);
}

unsigned int trafficreport_hourtick::get_id() const
{
	return utime2id(ut);
}

unsigned int trafficreport_hourtick::utime2id(const utime& uti)
{
	unsigned int y = static_cast<unsigned int>(uti.get_year() % 100);
	unsigned int m = static_cast<unsigned int>(uti.get_month());
	unsigned int d = static_cast<unsigned int>(uti.get_day());
	unsigned int ho = static_cast<unsigned int>((uti.get_hours() / 2) * 2);

	return (ho * 100) + (d * 10000) + (m * 1000000) + (y * 100000000);
}

void trafficreport_hourtick::id2utime(unsigned int idts, utime& ut)
{
	unsigned int y = idts / 100000000;
	unsigned int m = (idts / 1000000) % 100;
	unsigned int d = (idts / 10000) % 100;
	unsigned int ho = (idts % 10000) / 100;

	ut.set(y + 2000, m, d, ho, 0, 0);
}


void trafficreport_hourtick::set_id(unsigned int id)
{
	return;
}

void trafficreport_hourtick::hourts2utime(const char* str, utime& ut)
{
	int y = 0;
	int m = 0;
	int d = 0;
	int ho = 0;
	int mi = 0;

	sscanf_s(str, "%02d%02d%02d %02d:%02d", &y, &m, &d, &ho, &mi);
	ut.set(y + 2000, m, d, ho, mi, 0);
}

#ifdef UTM_DEBUG
void trafficreport_hourtick::test_all()
{
	utime ut;
	ut.set(2019, 10, 23, 21, 11, 0);

	unsigned int id;
	id = utime2id(ut);
	TEST_CASE_CHECK(id, unsigned int(1910232000));

	utime ut0;
	id2utime(id, ut0);
	TEST_CASE_CHECK(ut0.to_string(utime::format_sql), std::string("2019-10-23 20:00:00"));

	std::string s;
	trafficreport_hourtick::utime2hourts(ut, s);
	TEST_CASE_CHECK(s, std::string("191023 20:00"));

	utime ut2;
	trafficreport_hourtick::hourts2utime(s.c_str(), ut2);
	std::string s2 = ut2.to_string(utime::format_sql);
	TEST_CASE_CHECK(s2, std::string("2019-10-23 20:00:00"));

	trafficreport_hourtick tht;
	tht.sent = 123;
	tht.recv = 432;
	tht.ut.set(2019, 10, 23, 20, 11, 0);

	std::string x;
	tht.xml_create();
	tht.xml_get_string(x);

	trafficreport_hourtick crt;
	crt.xml_parse(x.c_str());

	TEST_CASE_CHECK(crt.sent, __int64(123));
	TEST_CASE_CHECK(crt.recv, __int64(432));
	TEST_CASE_CHECK(crt.ut.to_string(utime::format_sql), std::string("2019-10-23 20:00:00"));
}
#endif

}