#include "stdafx.h"

#include "utime.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <time.h>

#include "ubase_test.h"

namespace utm {

const char utime::this_class_name[] = "utime";

utime::utime(bool setnow)
{
	if (setnow)
	{
		now();
	}
	else
	{
		clear();
	}
}

utime::utime(int year, int month, int day, int hour, int minute, int second)
{
	set(year, month, day, hour, minute, second);
}

utime::~utime(void)
{
}

bool utime::operator==(const utime& rhs) const
{
	return (ptime == rhs.ptime);
}

bool utime::operator!=(const utime& rhs) const
{
	return (ptime != rhs.ptime);
}

void utime::clear()
{
	set(2000, 1, 1, 0, 0, 0);
}

void utime::now()
{
	ptime = boost::posix_time::second_clock::local_time();
}

bool utime::is_valid() const
{
	return !ptime.is_not_a_date_time();
}

tm utime::to_tm() const
{
	return boost::posix_time::to_tm(ptime);
}

time_t utime::to_time_t() const
{
	tm lt = boost::posix_time::to_tm(ptime);
	lt.tm_isdst = 1;
	return mktime(&lt);
}

void utime::from_time_t(time_t t)
{
	tm lt;
	localtime_s(&lt, &t);
	set((lt.tm_year) + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
}

std::string utime::to_string(int format, char delim) const
{
	std::ostringstream oss;

	if ((format == utime::format_log) || (format == utime::format_sql))
	{
		oss << std::setfill('0') << std::setw(4) << static_cast<unsigned short>(ptime.date().year()) << "-" << std::setfill('0') << std::setw(2) << ptime.date().month().as_number() << "-" << std::setfill('0') << std::setw(2) << ptime.date().day().as_number();
		oss << ' ' << std::setfill('0') << std::setw(2) << static_cast<unsigned short>(ptime.time_of_day().hours()) << ":" << std::setfill('0') << std::setw(2) << static_cast<unsigned short>(ptime.time_of_day().minutes()) << ":" << std::setfill('0') << std::setw(2) << static_cast<unsigned short>(ptime.time_of_day().seconds());
	}

	if (format == utime::format_iso)
	{
		oss << std::setfill('0') << std::setw(4) << static_cast<unsigned short>(ptime.date().year()) << std::setfill('0') << std::setw(2) << ptime.date().month().as_number() << std::setfill('0') << std::setw(2) << ptime.date().day().as_number();
		oss << 'T' << std::setfill('0') << std::setw(2) << static_cast<unsigned short>(ptime.time_of_day().hours()) << std::setfill('0') << std::setw(2) << static_cast<unsigned short>(ptime.time_of_day().minutes()) << std::setfill('0') << std::setw(2) << static_cast<unsigned short>(ptime.time_of_day().seconds());
	}

	if (format == utime::format_yyyymmdd)
	{
		if (delim == 0)
			delim = '-';

		oss << std::setfill('0') << std::setw(4) << static_cast<unsigned short>(ptime.date().year()) << delim << std::setfill('0') << std::setw(2) << ptime.date().month().as_number() << delim << std::setfill('0') << std::setw(2) << ptime.date().day().as_number();
	}

	if (format == utime::format_hhmmss)
	{
		if (delim == 0)
			delim = ':';

		oss << std::setfill('0') << std::setw(2) << static_cast<unsigned short>(ptime.time_of_day().hours()) << delim << std::setfill('0') << std::setw(2) << static_cast<unsigned short>(ptime.time_of_day().minutes()) << delim << std::setfill('0') << std::setw(2) << static_cast<unsigned short>(ptime.time_of_day().seconds());
	}

	if (format == utime::format_yyyymmddhhmmss)
	{
		oss << std::setfill('0') << std::setw(4) << static_cast<unsigned short>(ptime.date().year()) << std::setfill('0') << std::setw(2) << ptime.date().month().as_number() << std::setfill('0') << std::setw(2) << ptime.date().day().as_number();
		oss << std::setfill('0') << std::setw(2) << static_cast<unsigned short>(ptime.time_of_day().hours()) << std::setfill('0') << std::setw(2) << static_cast<unsigned short>(ptime.time_of_day().minutes()) << std::setfill('0') << std::setw(2) << static_cast<unsigned short>(ptime.time_of_day().seconds());
	}

	return oss.str();
}

void utime::from_string(const char* str, int format)
{
	if (format == utime::format_iso)
	{
		ptime = boost::posix_time::from_iso_string(str);
	}

	if ((format == utime::format_log) || (format == utime::format_sql))
	{
		// Example string: "2000-01-02 23:59:12"
		ptime = boost::posix_time::time_from_string(str);
	}
}

utime utime::create_from_string(const char* str, int format)
{
	utime retval;
	retval.from_string(str, format);
	return retval;
}

unsigned short utime::get_year() const
{
	return static_cast<unsigned short>(ptime.date().year());
}

void utime::get_year(gstring& year_str) const
{
	unsigned short year_num = get_year();

#ifdef UNICODE
	std::wstring str = boost::lexical_cast<std::wstring>(year_num);
#else
	std::string str = boost::lexical_cast<std::string>(year_num);
#endif

	year_str.assign(str.c_str());
}

unsigned short utime::get_month() const
{
	return static_cast<unsigned short>(ptime.date().month().as_number());
}

void utime::get_month(gstring& month_str) const
{
	unsigned short num = get_month();
	format_twodigit_ushort(num, month_str);
}

void utime::get_month_short(gstring& month_str) const
{
	unsigned short num = get_month();

	switch(num)
	{
		case 1: month_str.assign(MONTH_Jan); break;
		case 2: month_str.assign(MONTH_Feb); break;
		case 3: month_str.assign(MONTH_Mar); break;
		case 4: month_str.assign(MONTH_Apr); break;
		case 5: month_str.assign(MONTH_May); break;
		case 6: month_str.assign(MONTH_Jun); break;
		case 7: month_str.assign(MONTH_Jul); break;
		case 8: month_str.assign(MONTH_Aug); break;
		case 9: month_str.assign(MONTH_Sep); break;
		case 10: month_str.assign(MONTH_Oct); break;
		case 11: month_str.assign(MONTH_Nov); break;
		case 12: month_str.assign(MONTH_Dec); break;
	}
}

void utime::get_month_long(gstring& month_str) const
{
	unsigned short num = get_month();

	switch(num)
	{
		case 1: month_str.assign(MONTH_January); break;
		case 2: month_str.assign(MONTH_February); break;
		case 3: month_str.assign(MONTH_March); break;
		case 4: month_str.assign(MONTH_April); break;
		case 5: month_str.assign(MONTH_May); break;
		case 6: month_str.assign(MONTH_June); break;
		case 7: month_str.assign(MONTH_July); break;
		case 8: month_str.assign(MONTH_August); break;
		case 9: month_str.assign(MONTH_September); break;
		case 10: month_str.assign(MONTH_October); break;
		case 11: month_str.assign(MONTH_November); break;
		case 12: month_str.assign(MONTH_December); break;
	}
}

unsigned short utime::get_day() const
{
	return ptime.date().day().as_number();
}

void utime::get_day(gstring& day_str) const
{
	unsigned short num = get_day();
	format_twodigit_ushort(num, day_str);
}

unsigned short utime::get_hours() const
{
	return static_cast<unsigned short>(ptime.time_of_day().hours());
}

unsigned short utime::get_minutes() const
{
	return static_cast<unsigned short>(ptime.time_of_day().minutes());
}

unsigned short utime::get_seconds() const
{
	return static_cast<unsigned short>(ptime.time_of_day().seconds());
}

void utime::get(int& year, int& month, int& day, int& hour, int& minute, int& second) const
{
	tm lt = boost::posix_time::to_tm(ptime);
	year = lt.tm_year + 1900;
	month = lt.tm_mon + 1;
	day = lt.tm_mday;
	hour = lt.tm_hour;
	minute = lt.tm_min;
	second = lt.tm_sec;
}

void utime::set(int year, int month, int day, int hour, int minute, int second)
{
	boost::gregorian::date dt(year, month, day);
	boost::posix_time::ptime pts = boost::posix_time::from_time_t((3600*hour) + (minute*60) + second);
	boost::posix_time::time_duration td = pts - boost::posix_time::from_time_t(0);
	boost::posix_time::ptime p1(dt, td);
	ptime = p1;
}

utime utime::start_of_hour(const utime& t)
{
	tm lt = boost::posix_time::to_tm(t.ptime);
	return utime(lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, 0, 0);
}

utime utime::start_of_day(const utime& t)
{
	tm lt = boost::posix_time::to_tm(t.ptime);
	return utime(lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, 0, 0, 0);
}

utime utime::start_of_month(const utime& t)
{
	tm lt = boost::posix_time::to_tm(t.ptime);
	return utime(lt.tm_year + 1900, lt.tm_mon + 1, 1, 0, 0, 0);
}

utime utime::start_of_lastdayweek(const utime& t, int weekday)
{
	tm lt = boost::posix_time::to_tm(t.ptime);
	int diff = lt.tm_wday - weekday;

	if (diff > 0) 
		diff = -diff;
	else if (diff < 0)
		diff = -(UTM_DAYS_IN_WEEK + diff);

	lt.tm_hour = 0;
	lt.tm_min = 0;
	lt.tm_sec = 0;
	lt.tm_isdst = 1;
	time_t tt = mktime(&lt) + (diff * UTM_SECONDS_IN_DAY);

	utime retval;
	retval.from_time_t(tt);

	return retval;
}

void utime::format_twodigit_ushort(unsigned short us, gstring& str) const
{
	if (us < 10)
	{
#ifdef UNICODE
		str.assign(L"0");
		str.append(boost::lexical_cast<std::wstring>(us).c_str());
#else
		str.assign("0");
		str.append(boost::lexical_cast<std::string>(us).c_str());
#endif
	}
	else
	{
#ifdef UNICODE
		str.assign(boost::lexical_cast<std::wstring>(us).c_str());
#else
		str.assign(boost::lexical_cast<std::string>(us).c_str());
#endif
	}
}

std::istream& operator>>(std::istream& s, utime& obj)
{
	char ch;
	std::string tmp;
	tmp.reserve(40);

	while (!s.eof() && !s.fail())
	{
		s.get(ch);
		if (s.rdstate() & (std::ios::eofbit|std::ios::badbit|std::ios::failbit))
			break;

		tmp.push_back(ch);
	}

	obj.from_string(tmp.c_str());

	return s;
}

#ifdef UTM_DEBUG
void utime::test_all()
{
	test_report tr(this_class_name);

	utime ut1;
	ut1.set(2010, 12, 31, 10, 19, 59);
	std::string s1 = ut1.to_string();
	TEST_CASE_CHECK(s1, std::string("2010-12-31 10:19:59"));

	utime ut2;
	ut2.set(2012, 2, 3, 4, 5, 6);
	std::string s2 = ut2.to_string();
	TEST_CASE_CHECK(s2, std::string("2012-02-03 04:05:06"));

	utime ut3;
	ut3.from_string(s1.c_str(), utime::format_log);
	TEST_CASE_CHECK(ut1, ut3);

	utime ut4;
	ut4.from_string(s1.c_str(), utime::format_sql);
	TEST_CASE_CHECK(ut1, ut4);

	time_t t1 = ut1.to_time_t();
	utime ut5;
	ut5.from_time_t(t1);
	TEST_CASE_CHECK(ut1, ut5);

	std::ostringstream oss1;
	oss1 << ut1;
	std::string soss1 = oss1.str();
	TEST_CASE_CHECK(s1, std::string("2010-12-31 10:19:59"));

	utime ut6;
	std::istringstream iss1("2010-12-31 10:19:59");
	iss1 >> ut6;
	std::string s6 = ut6.to_string();
	TEST_CASE_CHECK(s6, std::string("2010-12-31 10:19:59"));

	int year, month, day, hour, minute, second;
	ut6.get(year, month, day, hour, minute, second);
	TEST_CASE_CHECK(year, 2010);
	TEST_CASE_CHECK(month, 12);
	TEST_CASE_CHECK(day, 31);
	TEST_CASE_CHECK(hour, 10);
	TEST_CASE_CHECK(minute, 19);
	TEST_CASE_CHECK(second, 59);

	utime ut7 = utime::start_of_hour(ut6);
	std::string s7 = ut7.to_string();
	TEST_CASE_CHECK(s7, std::string("2010-12-31 10:00:00"));

	utime ut8 = utime::start_of_day(ut6);
	std::string s8 = ut8.to_string();
	TEST_CASE_CHECK(s8, std::string("2010-12-31 00:00:00"));

	utime ut9 = utime::start_of_month(ut6);
	std::string s9 = ut9.to_string();
	TEST_CASE_CHECK(s9, std::string("2010-12-01 00:00:00"));

	int days[UTM_DAYS_IN_WEEK] = { 26, 27, 28, 29, 30, 31, 25 };
	for (int i = 0; i < UTM_DAYS_IN_WEEK; i++)
	{
		utime ut10 = utime::start_of_lastdayweek(ut6, i);
		std::string s10 = ut10.to_string();

		int year, month, day, hour, minute, second;
		ut10.get(year, month, day, hour, minute, second);
		TEST_CASE_CHECK(year, 2010);
		TEST_CASE_CHECK(month, 12);
		TEST_CASE_CHECK(day, days[i]);
		TEST_CASE_CHECK(hour, 0);
		TEST_CASE_CHECK(minute, 0);
		TEST_CASE_CHECK(second, 0);
	}

	size_t st = sizeof(time_t);

	return;
}
#endif

}