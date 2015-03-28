#include "stdafx.h"
#include "trafficreport_daytick.h"

#include <ubase_test.h>

namespace utm {

trafficreport_daytick::trafficreport_daytick()
{
}


trafficreport_daytick::~trafficreport_daytick()
{
}

bool trafficreport_daytick::xml_has_root_attr() const
{
	return true;
}

ubase* trafficreport_daytick::xml_catch_subnode(const char *keyname)
{ 
	ubase *u = NULL;

	if (strcmp(keyname, TR_HOURTICK_XMLTAG_ROOT) == 0)
	{
		u = (ubase *)hourticks.get_temp_item();
		return u;
	}

	return NULL;
};

void trafficreport_daytick::xml_catch_subnode_finished(const char *keyname)
{
	if (strcmp(keyname, TR_HOURTICK_XMLTAG_ROOT) == 0)
	{
		hourticks.commit_temp_item();
		return;
	}
}

void trafficreport_daytick::xml_catch_subnode_attribute(const char *attrname, const char* attrvalue)
{
	xml_catch_rootnode_attribute(attrname, attrvalue);
}

void trafficreport_daytick::xml_catch_rootnode_attribute(const char *attrname, const char* attrvalue)
{
	if (strcmp(attrname, "dt") == 0)
	{
		trafficreport_daytick::dayts2utime(attrvalue, ut);
	}
}

void trafficreport_daytick::xml_get_root_attr(xmlattr_container& attr)
{
	std::string s;
	trafficreport_daytick::utime2dayts(ut, s);
	attr.insert(std::make_pair<std::string, std::string>(std::string("dt"), std::string(s)));
};

void trafficreport_daytick::utime2dayts(const utime& ut, std::string& str)
{
	unsigned short y = ut.get_year() % 100;
	unsigned short m = ut.get_month();
	unsigned short d = ut.get_day();

	char s[64];
	sprintf_s(s, 64, "%02d%02d%02d", y, m, d);
	str.assign(s);
}

void trafficreport_daytick::set_id(unsigned int id)
{
	int y = id / 10000;
	int m = (id % 10000) / 100;
	int d = id % 100;

	ut.set(y + 2000, m, d, 0, 0, 0);
}

void trafficreport_daytick::dayts2utime(const char* str, utime& ut)
{
	int y = 0;
	int m = 0;
	int d = 0;

	sscanf_s(str, "%02d%02d%02d", &y, &m, &d);
	ut.set(y + 2000, m, d, 0, 0, 0);
}

unsigned int trafficreport_daytick::utime2id(const utime& utim)
{
	unsigned int y = static_cast<unsigned int>(utim.get_year() % 100);
	unsigned int m = static_cast<unsigned int>(utim.get_month());
	unsigned int d = static_cast<unsigned int>(utim.get_day());

	unsigned int id = d + (m * 100) + (y * 10000);

	return id;
}

void trafficreport_daytick::id2utime(unsigned int idts, utime& ut)
{
	int y = idts / 10000;
	int m = (idts % 10000) / 100;
	int d = idts % 100;

	ut.set(y + 2000, m, d, 0, 0, 0);
}

unsigned int trafficreport_daytick::get_id() const
{
	return utime2id(ut);
}

void trafficreport_daytick::update_counters(const utime& ctm, __int64 sent, __int64 recv)
{
	unsigned int cts_day = utime2id(ctm);
	unsigned int ts_day = utime2id(ut);

	if (cts_day != ts_day)
	{
		return;
	}

	this->sent += sent;
	this->recv += recv;

	unsigned int cts_hour = trafficreport_hourtick::utime2id(ctm);

	bool found = false;
	for (auto iter = hourticks.items.begin(); iter != hourticks.items.end(); ++iter)
	{
		unsigned int ts_hour = iter->get_id();
		if (cts_hour == ts_hour)
		{
			iter->sent += sent;
			iter->recv += recv;
			found = true;
			break;
		}
	}

	if (!found)
	{
		utime mtm;
		trafficreport_hourtick::id2utime(cts_hour, mtm);
		trafficreport_hourtick tick;
		tick.ut = mtm;
		tick.sent = sent;
		tick.recv = recv;
		hourticks.add_element(tick);
	}
}

#ifdef UTM_DEBUG
void trafficreport_daytick::test_all()
{
	trafficreport_daytick trd = trafficreport_daytick::test_get_daytick();

	std::string x;
	trd.xml_create();
	trd.xml_get_string(x);

	trafficreport_daytick trd2;
	trd2.xml_parse(x.c_str());

	{
		TEST_CASE_CHECK(trd2.sent, __int64(100));
		TEST_CASE_CHECK(trd2.recv, __int64(200));
		std::string ts2 = trd2.ut.to_string(utime::format_sql);
		TEST_CASE_CHECK(ts2, std::string("2014-01-31 00:00:00"));
		TEST_CASE_CHECK(trd2.hourticks.items.size(), size_t(3));

		TEST_CASE_CHECK(trd2.hourticks.size(), size_t(3));
		int i = 0;
		for (auto iter = trd2.hourticks.items.begin(); iter != trd2.hourticks.items.end(); ++iter)
		{
			if (i == 0)
			{
				TEST_CASE_CHECK(iter->sent, __int64(10));
				TEST_CASE_CHECK(iter->recv, __int64(20));
				TEST_CASE_CHECK(iter->ut.to_string(utime::format_sql), std::string("2014-01-31 00:00:00"));
			}

			if (i == 1)
			{
				TEST_CASE_CHECK(iter->sent, __int64(10));
				TEST_CASE_CHECK(iter->recv, __int64(20));
				TEST_CASE_CHECK(iter->ut.to_string(utime::format_sql), std::string("2014-01-31 02:00:00"));
			}

			if (i == 2)
			{
				TEST_CASE_CHECK(iter->sent, __int64(80));
				TEST_CASE_CHECK(iter->recv, __int64(160));
				TEST_CASE_CHECK(iter->ut.to_string(utime::format_sql), std::string("2014-01-31 04:00:00"));
			}

			i++;
		}
	}

	utime ut3;
	ut3.set(2014, 01, 31, 05, 32, 59);
	trd2.update_counters(ut3, 11, 22);

	{
		TEST_CASE_CHECK(trd2.sent, __int64(111));
		TEST_CASE_CHECK(trd2.recv, __int64(222));

		TEST_CASE_CHECK(trd2.hourticks.size(), size_t(3));
		int i = 0;
		for (auto iter = trd2.hourticks.items.begin(); iter != trd2.hourticks.items.end(); ++iter)
		{
			if (i == 0)
			{
				TEST_CASE_CHECK(iter->sent, __int64(10));
				TEST_CASE_CHECK(iter->recv, __int64(20));
				TEST_CASE_CHECK(iter->ut.to_string(utime::format_sql), std::string("2014-01-31 00:00:00"));
			}

			if (i == 1)
			{
				TEST_CASE_CHECK(iter->sent, __int64(10));
				TEST_CASE_CHECK(iter->recv, __int64(20));
				TEST_CASE_CHECK(iter->ut.to_string(utime::format_sql), std::string("2014-01-31 02:00:00"));
			}

			if (i == 2)
			{
				TEST_CASE_CHECK(iter->sent, __int64(91));
				TEST_CASE_CHECK(iter->recv, __int64(182));
				TEST_CASE_CHECK(iter->ut.to_string(utime::format_sql), std::string("2014-01-31 04:00:00"));
			}

			i++;
		}
	}

	utime ut4;
	ut4.set(2014, 01, 31, 06, 02, 12);
	trd2.update_counters(ut4, 33, 44);

	{
		TEST_CASE_CHECK(trd2.sent, __int64(144));
		TEST_CASE_CHECK(trd2.recv, __int64(266));

		TEST_CASE_CHECK(trd2.hourticks.size(), size_t(4));
		int i = 0;
		for (auto iter = trd2.hourticks.items.begin(); iter != trd2.hourticks.items.end(); ++iter)
		{
			if (i == 0)
			{
				TEST_CASE_CHECK(iter->sent, __int64(10));
				TEST_CASE_CHECK(iter->recv, __int64(20));
				TEST_CASE_CHECK(iter->ut.to_string(utime::format_sql), std::string("2014-01-31 00:00:00"));
			}

			if (i == 1)
			{
				TEST_CASE_CHECK(iter->sent, __int64(10));
				TEST_CASE_CHECK(iter->recv, __int64(20));
				TEST_CASE_CHECK(iter->ut.to_string(utime::format_sql), std::string("2014-01-31 02:00:00"));
			}

			if (i == 2)
			{
				TEST_CASE_CHECK(iter->sent, __int64(91));
				TEST_CASE_CHECK(iter->recv, __int64(182));
				TEST_CASE_CHECK(iter->ut.to_string(utime::format_sql), std::string("2014-01-31 04:00:00"));
			}

			if (i == 3)
			{
				TEST_CASE_CHECK(iter->sent, __int64(33));
				TEST_CASE_CHECK(iter->recv, __int64(44));
				TEST_CASE_CHECK(iter->ut.to_string(utime::format_sql), std::string("2014-01-31 06:00:00"));
			}

			i++;
		}
	}

	return;
}

trafficreport_daytick trafficreport_daytick::test_get_daytick()
{
	trafficreport_daytick trd;

	trd.sent = 100;
	trd.recv = 200;
	trd.ut.set(2014, 01, 31, 0, 0, 0);

	trafficreport_hourtick h1;
	h1.sent = 10;
	h1.recv = 20;
	h1.ut.set(2014, 01, 31, 00, 00, 00);
	trd.hourticks.add_element(h1);

	trafficreport_hourtick h2;
	h2.sent = 10;
	h2.recv = 20;
	h2.ut.set(2014, 01, 31, 02, 00, 00);
	trd.hourticks.add_element(h2);

	trafficreport_hourtick h3;
	h3.sent = 80;
	h3.recv = 160;
	h3.ut.set(2014, 01, 31, 04, 00, 00);
	trd.hourticks.add_element(h3);

	return trd;
}

#endif

}