#ifndef _UTM_UTIME_H
#define _UTM_UTIME_H

#pragma once
#include <utm.h>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

#include <string>
#include <gstring.h>

#define MONTH_January _U("January")
#define MONTH_February _U("February")
#define MONTH_March _U("March")
#define MONTH_April _U("April")
#define MONTH_May _U("May")
#define MONTH_June _U("June")
#define MONTH_July _U("July")
#define MONTH_August _U("August")
#define MONTH_September _U("September")
#define MONTH_October _U("October")
#define MONTH_November _U("November")
#define MONTH_December _U("December")

#define MONTH_Jan _U("Jan")
#define MONTH_Feb _U("Feb")
#define MONTH_Mar _U("Mar")
#define MONTH_Apr _U("Apr")
#define MONTH_May _U("May")
#define MONTH_Jun _U("Jun")
#define MONTH_Jul _U("Jul")
#define MONTH_Aug _U("Aug")
#define MONTH_Sep _U("Sep")
#define MONTH_Oct _U("Oct")
#define MONTH_Nov _U("Nov")
#define MONTH_Dec _U("Dec")

#define UTM_DAYS_IN_WEEK 7
#define UTM_SECONDS_IN_DAY 86400

namespace utm {

class utime
{
public:
	static const char this_class_name[];
	enum utime_format { format_log = 0, format_iso = 1, format_yyyymmdd = 2, format_hhmmss = 3, format_sql = 4, format_yyyymmddhhmmss = 5 };

	utime(bool initnow = false);
	utime(int year, int month, int day, int hour, int minute, int second);
	~utime(void);

	bool operator==(const utime& rhs) const;
	bool operator!=(const utime& rhs) const;

	boost::posix_time::ptime ptime;

	void clear();
	void now();
	bool is_valid() const;

	tm to_tm() const;
	time_t to_time_t() const;
	void from_time_t(time_t t);

	std::string to_string(int format = 0, char delim = 0) const;
	void from_string(const char* str, int format = 0);
	static utime create_from_string(const char* str, int format = 0);

	inline unsigned short get_year() const;
	void get_year(gstring& year_str) const;
	inline unsigned short get_month() const;
	void get_month(gstring& month_str) const;
	void get_month_short(gstring& month_str) const;
	void get_month_long(gstring& month_str) const;
	inline unsigned short get_day() const;
	void get_day(gstring& day_str) const;

	unsigned short get_hours() const;
	unsigned short get_minutes() const;
	unsigned short get_seconds() const;

	void get(int& year, int& month, int& day, int& hour, int& minute, int& second) const;
	void set(int year, int month, int day, int hour, int minute, int second);

	static utime start_of_hour(const utime& t);
	static utime start_of_day(const utime& t);
	static utime start_of_month(const utime& t);
	static utime start_of_lastdayweek(const utime& t, int weekday);

	friend std::ostream&  operator<<(std::ostream& s, const utime& rhs) { s << rhs.to_string(); return s; };
	friend std::istream& operator>>(std::istream& s, utime& obj);

private:
	inline void format_twodigit_ushort(unsigned short us, gstring& str) const;

public:
#ifdef UTM_DEBUG
	static void test_all();
#endif
};

struct standard_timeset
{
	standard_timeset(const utime& t)
	{
		time_point = t.to_time_t();
		start_of_hour = utime::start_of_hour(t).to_time_t();
		start_of_day = utime::start_of_day(t).to_time_t();
		start_of_month = utime::start_of_month(t).to_time_t();
		for (int i = 0; i < UTM_DAYS_IN_WEEK; i++)
		{
			start_of_lastdayweek[i] = utime::start_of_lastdayweek(t, i).to_time_t();
		}
	};

	time_t time_point;
	time_t start_of_hour;
	time_t start_of_day;
	time_t start_of_month;
	time_t start_of_lastdayweek[UTM_DAYS_IN_WEEK];
};

}

#endif // _UTM_UTIME_H