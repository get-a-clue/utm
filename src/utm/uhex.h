#ifndef _UTM_UHEX_H
#define _UTM_UHEX_H

#pragma once
#include <utm.h>

#include <exception>
#include <cstdint>

namespace utm {

class uhex
{
public:
	static const char this_class_name[];
	static const char nibble_values[];

	uhex(void) { };
	~uhex(void) { };

	inline static char get_first_nibble(unsigned char b)    { return nibble_values[b >> 4]; };
	inline static char get_second_nibble(unsigned char b)   { return nibble_values[b & 0x0F]; }
	inline static char get_first_nibble(unsigned short n)   { return nibble_values[(n >> 4) & 0x0F]; };
	inline static char get_second_nibble(unsigned short n)  { return nibble_values[n & 0x0F]; }
	inline static char get_first_nibble(unsigned int a)     { return nibble_values[(a >> 4) & 0x0F]; };
	inline static char get_second_nibble(unsigned int a)    { return nibble_values[a & 0x0F]; }
	inline static unsigned char parse_nibble(const char n)
	{
		if (n >= '0' && n <= '9')
			return (n - '0');

		if (n >= 'a' && n <= 'f')
			return (n - 'a' + 10);

		if (n >= 'A' && n <= 'F')
			return (n - 'A' + 10);

		throw std::exception("Unable parse symbol as hex");
	}

	static void as_hex(unsigned int m, char* buffer)
	{
		unsigned int a;
	
		a = m >> 24; *buffer = get_first_nibble(a); buffer++; *buffer = get_second_nibble(a); buffer++;
		a = m >> 16; *buffer = get_first_nibble(a); buffer++; *buffer = get_second_nibble(a); buffer++;
		a = m >> 8;  *buffer = get_first_nibble(a); buffer++; *buffer = get_second_nibble(a); buffer++;
		             *buffer = get_first_nibble(m); buffer++; *buffer = get_second_nibble(m); buffer++;

		*buffer = 0;
	}

	static void as_hex(unsigned short m, char* buffer)
	{
		unsigned short a;
	
		a = m >> 8;  *buffer = get_first_nibble(a); buffer++; *buffer = get_second_nibble(a); buffer++;
		             *buffer = get_first_nibble(m); buffer++; *buffer = get_second_nibble(m); buffer++;

		*buffer = 0;
	}

	static void parse_hex(const char* hex, unsigned int len, unsigned char* buffer)
	{
		const char *p = hex;
		for (unsigned int i = 0; i < len; i++, p++)
		{
			if (*p == 0)
			{
				break;
			}

			if ((i & 0x01) == 0)
			{
				*(buffer + (i >> 1)) = ((parse_nibble(*p)) << 4);
			}
			else
			{
				*(buffer + (i >> 1)) += parse_nibble(*p);
			}
		}
	}

#ifdef UTM_DEBUG
	
#endif
};

}

#endif // _UTM_UHEX_H