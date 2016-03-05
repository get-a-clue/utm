#include "stdafx.h"

#include "addrmac.h"
#include "uhex.h"
#include <boost/test/unit_test.hpp>

#include <iostream>

namespace utm {

const char addrmac::this_class_name[] = "addrmac";

addrmac::addrmac()
{
	clear();
}

addrmac::~addrmac()
{
}

addrmac::addrmac(const char *p)
{
	from_string(p);
}

addrmac::addrmac(const std::string& str)
{
	from_string(str.c_str());
}

bool addrmac::operator <(const addrmac& rhs) const
{
	for (int i = 0; i < MAX_ETHER_LENGTH; i++)
	{
		if (content[i] < rhs.content[i])
			return true;

		if (content[i] > rhs.content[i])
			return false;
	}

	return false;
}

bool addrmac::operator==(const addrmac& rhs) const
{
	return (memcmp(content, rhs.content, MAX_ETHER_LENGTH) == 0);
}

bool addrmac::operator!=(const addrmac& rhs) const
{
	return (memcmp(content, rhs.content, MAX_ETHER_LENGTH) != 0);
}

bool addrmac::is_equal(const unsigned char *p) const
{
	return (memcmp(content, p, MAX_ETHER_LENGTH) == 0);
}

void addrmac::clear()
{
	memset(&content, 0, MAX_ETHER_LENGTH);
}

bool addrmac::is_zero() const
{
	// the brutest and the fastest algorithm
	if (content[0] != 0) return false;
	if (content[1] != 0) return false;
	if (content[2] != 0) return false;
	if (content[3] != 0) return false;
	if (content[4] != 0) return false;
	if (content[5] != 0) return false;
	return true;
}

void addrmac::set(const unsigned char *p)
{
	if (p == NULL)
	{
		clear();
		return;
	}

	memcpy(&content, p, MAX_ETHER_LENGTH);
}

const std::string addrmac::to_string() const
{
	char buffer[24];
	to_string(buffer);
	return std::string(buffer);
}

void addrmac::to_string(char* buffer) const
{
	// the brutest and the fastest algorithm
	*buffer = uhex::get_first_nibble(content[0]); buffer++;	*buffer = uhex::get_second_nibble(content[0]); buffer++; *buffer=':'; buffer++;
	*buffer = uhex::get_first_nibble(content[1]); buffer++;	*buffer = uhex::get_second_nibble(content[1]); buffer++; *buffer=':'; buffer++;
	*buffer = uhex::get_first_nibble(content[2]); buffer++;	*buffer = uhex::get_second_nibble(content[2]); buffer++; *buffer=':'; buffer++;
	*buffer = uhex::get_first_nibble(content[3]); buffer++;	*buffer = uhex::get_second_nibble(content[3]); buffer++; *buffer=':'; buffer++;
	*buffer = uhex::get_first_nibble(content[4]); buffer++;	*buffer = uhex::get_second_nibble(content[4]); buffer++; *buffer=':'; buffer++;
	*buffer = uhex::get_first_nibble(content[5]); buffer++;	*buffer = uhex::get_second_nibble(content[5]); buffer++; 
	*buffer = 0;
}

void addrmac::as_hex(char* buffer) const
{
	// the brutest and the fastest algorithm
	*buffer = uhex::get_first_nibble(content[0]); buffer++;	*buffer = uhex::get_second_nibble(content[0]); buffer++;
	*buffer = uhex::get_first_nibble(content[1]); buffer++;	*buffer = uhex::get_second_nibble(content[1]); buffer++;
	*buffer = uhex::get_first_nibble(content[2]); buffer++;	*buffer = uhex::get_second_nibble(content[2]); buffer++;
	*buffer = uhex::get_first_nibble(content[3]); buffer++;	*buffer = uhex::get_second_nibble(content[3]); buffer++;
	*buffer = uhex::get_first_nibble(content[4]); buffer++;	*buffer = uhex::get_second_nibble(content[4]); buffer++;
	*buffer = uhex::get_first_nibble(content[5]); buffer++;	*buffer = uhex::get_second_nibble(content[5]); buffer++;
	*buffer = 0;
}

bool addrmac::from_string(const std::string& str)
{
	return from_string(str.c_str());
}

bool addrmac::from_string(const char *p)
{
	clear();
	if (p == NULL)
	{
		return false;
	}

	std::string filteredchars;
	filteredchars.reserve(strlen(p));

	char c;
	while((c = *p) != 0)
	{
		if ((isdigit(c)) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')))
		{
			filteredchars.push_back(c);
		}
		p++;
	}

	if (filteredchars.size() != MAX_ETHER_LENGTH*2)
		return false;

	uhex::parse_hex(filteredchars.c_str(), MAX_ETHER_LENGTH*2, content);

	return true;
}

std::ostream& operator<<(std::ostream& s, const addrmac& rhs)
{
	s << rhs.to_string();
	return s;
}

std::istream& operator>>(std::istream& s, addrmac& obj)
{
	char ch;
	std::string mmac;
	mmac.reserve(40);

	while (!s.eof() && !s.fail())
	{
		// read characters from the stream
		s.get(ch);

		// EOF of stream?
		if (s.rdstate() & (std::ios::eofbit|std::ios::badbit|std::ios::failbit))
			break;

		mmac.push_back(ch);
	}

	obj.from_string(mmac.c_str());
	return s;
}

BOOST_AUTO_TEST_CASE(addrmac_test_all)
{
	

	std::string mac1("81:92:a3:b4:c5:e6");
	std::string mac2("81-92-A3-B4-c5-e6");

	{
		addrmac m1(mac1);
		BOOST_REQUIRE_EQUAL(m1.to_string(), mac1);
	}

	{
		addrmac m1(mac2);
		BOOST_REQUIRE_EQUAL(m1.to_string(), mac1);
	}

	{
		addrmac m1(mac1);
		std::ostringstream oss;
		oss << m1;
		BOOST_REQUIRE_EQUAL(oss.str(), mac1);
	}

	{
		addrmac m1(mac1);
		addrmac m2(mac2);
		BOOST_REQUIRE_EQUAL(m1 == m2, bool(true));
	}

	{
		addrmac m1(mac1);
		char sz[20];
		m1.as_hex(sz);
		addrmac m2(sz);
		BOOST_REQUIRE_EQUAL(m1 == m2, bool(true));
	}

	{
		addrmac m1(mac1);
		unsigned char macbinary[] = { 0x81, 0x92, 0xa3, 0xb4, 0xc5, 0xe6 };
		bool eq = m1.is_equal(macbinary);
		BOOST_REQUIRE_EQUAL(eq, bool(true));
	}
}

}