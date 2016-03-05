#ifndef _UTM_BASE64_H
#define _UTM_BASE64_H

#pragma once
#include <utm.h>

#include <cstdint>
#include <string>
#include <vector>

namespace utm {

class base64
{
public:
	static const char this_class_name[];
	static const char base64enc[];
	static const char base64dec[];
	static const int mod_table[];

private:
	base64(void);
	~base64(void);

public:
	static void encode(const unsigned char *srcdata, size_t srclen, std::string& result);

	template<typename T>
	static bool decode(const char *srcdata, T& result, bool append_terminating_zero = false)
	{
		size_t srclen = strlen(srcdata);
		if (srclen % 4 != 0) return false;

		size_t outlen = (srclen / 4) * 3;
		result.reserve(outlen);

		if (srcdata[srclen - 1] == '=') outlen--;
		if (srcdata[srclen - 2] == '=') outlen--;

		uint32_t s[4], triple;
		char c;

		for (size_t i = 0, j = 0; i < srclen;)
		{
			c = srcdata[i]; s[0] = c == '=' ? 0 : base64dec[c]; i++;
			c = srcdata[i]; s[1] = c == '=' ? 0 : base64dec[c]; i++;
			c = srcdata[i]; s[2] = c == '=' ? 0 : base64dec[c]; i++;
			c = srcdata[i]; s[3] = c == '=' ? 0 : base64dec[c]; i++;

			triple = (s[0] << 3 * 6) + (s[1] << 2 * 6) + (s[2] << 1 * 6) + (s[3] << 0 * 6);

			if (j < outlen) result.push_back((triple >> 2 * 8) & 0xFF); j++;
			if (j < outlen) result.push_back((triple >> 1 * 8) & 0xFF); j++;
			if (j < outlen) result.push_back((triple >> 0 * 8) & 0xFF); j++;
		}

		if (append_terminating_zero)
			result.push_back(0);

		return true;
	};

#ifdef UTM_DEBUG
	
#endif
};

}

#endif // _UTM_BASE64_H
