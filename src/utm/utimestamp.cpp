#include "stdafx.h"
#include "utimestamp.h"

#include "ubase_test.h"

namespace utm {

const char utimestamp::this_class_name[] = "utimestamp";

utimestamp::utimestamp(void)
{
	clear();
}

utimestamp::utimestamp(time_t _ts)
{
	ts = _ts;
}

utimestamp::utimestamp(unsigned int _ts)
{
	from_uint(_ts);
}

utimestamp::utimestamp(const utime& ut)
{
	ts = ut.to_time_t();
}

utimestamp::~utimestamp(void)
{
}

std::string utimestamp::to_string() const
{
	char buf[16];

#if defined(_MSC_VER)
	sprintf_s(buf, 16, "%u", ts);
#else
	sprintf(buf, "%u", ts);
#endif

	return std::string(buf);
}


void utimestamp::from_string(const char *t)
{
	unsigned int temp = 0;

#if defined(_MSC_VER)
	sscanf_s(t, "%u", &temp);
#else
	sscanf(t, "%u", &temp);
#endif

	ts = (time_t)temp;
}

void utimestamp::from_uint(unsigned int _ts)
{
	ts = _ts;
}

std::istream& operator>>(std::istream& s, utimestamp& obj)
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

#ifdef UTM_WIN
void utimestamp::from_filetime(const FILETIME& ft)
{
   ULARGE_INTEGER ull;   
   ull.LowPart = ft.dwLowDateTime;   
   ull.HighPart = ft.dwHighDateTime;
   ts = (time_t)(ull.QuadPart / 10000000ULL - 11644473600ULL);
}
#endif

#ifdef UTM_DEBUG
void utimestamp::test_all()
{
	test_report tr(this_class_name);

	utimestamp u(unsigned int(777));

	std::ostringstream oss;
	oss << u;
	std::string us = oss.str();
	TEST_CASE_CHECK(us, std::string("777"));

	utimestamp t;
	std::istringstream iss(us.c_str());
	iss >> t;
	TEST_CASE_CHECK(t.to_string(), std::string("777"));

	return;
}
#endif

}