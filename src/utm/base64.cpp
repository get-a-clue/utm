#include "stdafx.h"
#include "base64.h"

#include <cstdint>

#include <boost/test/unit_test.hpp>

namespace utm {

const char base64::this_class_name[] = "base64";

const char base64::base64enc[] = {
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
	'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
	'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
	'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/' };

const char base64::base64dec[] = {
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,62, -1,-1,-1,63,
	52,53,54,55, 56,57,58,59, 60,61,-1,-1, -1,-1,-1,-1,
	-1, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
	15,16,17,18, 19,20,21,22, 23,24,25,-1, -1,-1,-1,-1,
	-1,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
	41,42,43,44, 45,46,47,48, 49,50,51,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1 };

const int base64::mod_table[] = {0, 2, 1};

base64::base64(void)
{
}


base64::~base64(void)
{
}

void base64::encode(const unsigned char *srcdata, size_t srclen, std::string& result)
{
    size_t result_len = 4 * ((srclen + 2) / 3);
	result.reserve(result_len);

	uint32_t c[3], triple;
	unsigned char b;

    for (size_t i = 0; i < srclen;) 
	{
		c[0] = i < srclen ? srcdata[i++] : 0;
        c[1] = i < srclen ? srcdata[i++] : 0;
        c[2] = i < srclen ? srcdata[i++] : 0;
        triple = (c[0] << 0x10) + (c[1] << 0x08) + c[2];

		b = base64enc[(triple >> 3 * 6) & 0x3F]; result.push_back(b);
        b = base64enc[(triple >> 2 * 6) & 0x3F]; result.push_back(b);
        b = base64enc[(triple >> 1 * 6) & 0x3F]; result.push_back(b);
        b = base64enc[(triple >> 0 * 6) & 0x3F]; result.push_back(b);
    }

	for (int k = 0; k < mod_table[srclen % 3]; k++)
        result[result_len - 1 - k] = '=';
}

BOOST_AUTO_TEST_CASE(base64_test_all)
{
	

	{
		std::string a1("Hello World");
		std::string b1;
		base64::encode(reinterpret_cast<const unsigned char*>(a1.c_str()), a1.size(), b1);
		BOOST_REQUIRE_EQUAL(b1, std::string("SGVsbG8gV29ybGQ="));
		std::vector<unsigned char> c1;
		base64::decode(b1.c_str(), c1);
		BOOST_REQUIRE_EQUAL(strncmp((a1.c_str()), reinterpret_cast<const char*>(c1.data()), c1.size()), 0);
	}

	{
		std::string a2("Hello World!");
		std::string b2;
		base64::encode(reinterpret_cast<const unsigned char*>(a2.c_str()), a2.size(), b2);
		BOOST_REQUIRE_EQUAL(b2, std::string("SGVsbG8gV29ybGQh"));
		std::vector<unsigned char> c2;
		base64::decode(b2.c_str(), c2);
		BOOST_REQUIRE_EQUAL(strncmp((a2.c_str()), reinterpret_cast<const char*>(c2.data()), c2.size()), 0);
	}

	{
		std::string a3("Hello World!?");
		std::string b3;
		base64::encode(reinterpret_cast<const unsigned char*>(a3.c_str()), a3.size(), b3);
		BOOST_REQUIRE_EQUAL(b3, std::string("SGVsbG8gV29ybGQhPw=="));
		std::vector<unsigned char> c3;
		base64::decode(b3.c_str(), c3);
		BOOST_REQUIRE_EQUAL(strncmp((a3.c_str()), reinterpret_cast<const char*>(c3.data()), c3.size()), 0);
	}
}

}