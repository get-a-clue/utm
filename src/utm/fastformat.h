#ifndef _UTM_FASTFORMAT_H
#define _UTM_FASTFORMAT_H

#pragma once
#include <utm.h>

#include <string>

namespace utm {

class fastformat
{
public:
	fastformat();
	~fastformat();

private:
	template<typename T> inline static void parse_signed(const char *p, T& res)
	{
		bool is_positive = true;
		while (*p == ' ') p++;

		if (*p == '-')
		{
			is_positive = false;
			p++;
		}

		while (*p >= '0' && *p <= '9') { res = res * 10 + (*p++ - '0'); }
		if (!is_positive)
		{
			res = -res;
		}
	}

	template<typename T> inline static void parse_unsigned(const char *p, T& res)
	{
		while (*p == ' ') p++;
		while (*p >= '0' && *p <= '9') { res = res * 10 + (*p++ - '0'); }
	}

public:
	static short parse_short(const char *p);
	static unsigned short parse_ushort(const char *p);
	static int parse_int(const char *p);
	static unsigned int parse_uint(const char *p);
	static std::int64_t parse_int64(const char *p);
	static std::uint64_t parse_uint64(const char *p);

	static void test_all();
};

}

#endif // _UTM_FASTFORMAT_H