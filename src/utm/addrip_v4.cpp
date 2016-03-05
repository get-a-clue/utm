#include "stdafx.h"

#include "addrip_v4.h"
#include "uhex.h"
#include <boost/test/unit_test.hpp>

namespace utm {

const char addrip_v4::this_class_name[] = "addrip_v4";

addrip_v4::addrip_v4(void)
{
	m_addr = 0;

#ifdef _DEBUG
	m_addr2[0] = 0;
#endif
}

addrip_v4::~addrip_v4(void)
{
}

addrip_v4::addrip_v4(unsigned long addr)
{
	m_addr = addr;

#ifdef _DEBUG
	to_strbuffer(m_addr, m_addr2);
#endif

}

addrip_v4::addrip_v4(const char *str)
{
	from_string(str);
}

addrip_v4::addrip_v4(const addrip_v4 &r)
{
	m_addr = r.m_addr;

#ifdef _DEBUG
	to_strbuffer(m_addr, m_addr2);
#endif
}

addrip_v4& addrip_v4::operator=(const addrip_v4& rhs)
{
	m_addr = rhs.m_addr;

#ifdef _DEBUG
	to_strbuffer(m_addr, m_addr2);
#endif

	return *this;
}

addrip_v4& addrip_v4::operator=(unsigned int rhs)
{
	m_addr = rhs;

#ifdef _DEBUG
	to_strbuffer(m_addr, m_addr2);
#endif

	return *this;
}

addrip_v4 addrip_v4::AND(const addrip_v4& lhs, const addrip_v4& rhs)
{
	addrip_v4 retval(lhs.m_addr & rhs.m_addr);
	return retval;
}

bool addrip_v4::operator ==(const addrip_v4& rhs) const
{
	return (m_addr == rhs.m_addr);
}

bool addrip_v4::operator !=(const addrip_v4& rhs) const
{
	return (m_addr != rhs.m_addr);
}

bool addrip_v4::operator <(const addrip_v4& rhs) const
{
	return (m_addr < rhs.m_addr);
}

bool addrip_v4::operator >(const addrip_v4& rhs) const
{
	return (m_addr > rhs.m_addr);
}

bool addrip_v4::operator <=(const addrip_v4& rhs) const
{
	return (m_addr <= rhs.m_addr);
}

bool addrip_v4::operator >=(const addrip_v4& rhs) const
{
	return (m_addr >= rhs.m_addr);
}

std::ostream& operator<<(std::ostream& s, const addrip_v4& rhs)
{
	char tmp[MAX_ADDRIPV4_STR_LEN];
	addrip_v4::to_strbuffer(rhs.m_addr, tmp);
	s << tmp;

	return s;
}

// Extract a string with IPv4 address in dotted-decimial notation into addrip_v4 class.
std::istream& operator>>(std::istream& s, addrip_v4& obj)
{
	char ch;
	std::string ipaddr;
	ipaddr.reserve(40);

	while (!s.eof() && !s.fail())
	{
		// read characters from the stream
		s.get(ch);

		// EOF of stream?
		if (s.rdstate() & (std::ios::eofbit|std::ios::badbit|std::ios::failbit))
			break;

		ipaddr.push_back(ch);
	}

	obj.from_string(ipaddr.c_str());

	return s;
}

void addrip_v4::set(const unsigned char* buffer)
{
	m_addr = (static_cast<unsigned long>(*buffer)) << 24;
	m_addr += (static_cast<unsigned long>(*(buffer + 1))) << 16;
	m_addr += (static_cast<unsigned long>(*(buffer + 2))) << 8;
	m_addr += (static_cast<unsigned long>(*(buffer + 3)));
}

bool addrip_v4::from_string(const std::string& str, bool check)
{
	return from_string(str.c_str(), check);
}

bool addrip_v4::from_string(const char *str, bool check)
{
	m_addr = 0;

	if (str == NULL)
	{
		return true;
	}

	// sscanf replacement... :)

	char octet[4];							// string representation of current octet
	unsigned long a[4] = { 0, 0, 0, 0 };	// array for octets

	int octet_symbols = 0;
	int octet_counter = 0;

	char ch;
	while ((ch = *str) != 0)
	{
		str++;

		if (!isdigit(ch) && ch != '.')
			break;

		if (octet_symbols == 0)
			memset(&octet[0], 0, sizeof(octet));

		if (ch == '.')
		{
			// do not allow to parse more than 4 octets.
			if (octet_counter < 4)
			{
				// convert string representation of the current octet
				// into "unsigned long" type.

				a[octet_counter] = atoi(octet);
				if (a[octet_counter] > 255)
				{
					// the octet must not be greater than 0xFF
					a[octet_counter] = 255;
				};

				octet_counter++;		// increase the number of parsed octets

				// prepare the buffer for next octet
				octet_symbols = 0;
			};
		}
		else
		{
			// we have a digit
			if (octet_symbols < 3)
			{
				octet[octet_symbols] = ch;
				octet_symbols++;
			};
		};
	};

	if ((octet_counter < 4) && (octet_symbols > 0))
	{
		// Convert the latest octet into "unsigned long" type.
		a[octet_counter] = atoi(octet);
		if (a[octet_counter] > 255)
		{
			a[octet_counter] = 255;
		};
	};

	// In the end, all octets will be assembled in IPv4 address.

	m_addr = (a[0] << 24) + (a[1] << 16) + (a[2] << 8) + a[3];
	
#ifdef _DEBUG
	to_strbuffer(m_addr, m_addr2);
#endif

	if (check)
	{
		std::string c = to_string();
		if (strcmp(str, c.c_str()) != 0)
			return false;
	}

    return true;
}

std::string addrip_v4::to_string() const
{
	char tmp[MAX_ADDRIPV4_STR_LEN];
	addrip_v4::to_strbuffer(m_addr, tmp);
	return std::string(tmp);
}

void addrip_v4::as_hex(char* buffer) const
{
	uhex::as_hex((unsigned int)m_addr, buffer);
}

void addrip_v4::revert_octets()
{
	unsigned int a1 = (m_addr >> 24) & 0x000000FF;
	unsigned int a2 = (m_addr >> 16) & 0x000000FF;
	unsigned int a3 = (m_addr >> 8) & 0x000000FF;
	unsigned int a4 = m_addr & 0x000000FF;

	m_addr = (a4 << 24) + (a3 << 16) + (a2 << 8) + a1;
}

void addrip_v4::convert_ipmask_to_range(const addrip_v4& ip, const addrip_v4& mask, addrip_v4& startip, addrip_v4& endip)
{
    unsigned int bitmask = 1;
    unsigned int netmask = 0xFFFFFFFF;

    while ((mask.m_addr & bitmask) == 0)
    {
        bitmask = bitmask << 1;
        netmask = netmask << 1;
        netmask = netmask & 0xFFFFFFFE;
    }

	startip.m_addr = ip.m_addr & netmask;
	endip.m_addr = startip.m_addr + bitmask - 1;
}

bool addrip_v4::is_netmask() const
{
    unsigned int bitmask = 1;
    while ((m_addr & bitmask) == 0)
    {
        bitmask = bitmask << 1;
    }	

	unsigned int r = m_addr + bitmask;

	return (r == 0);
}

BOOST_AUTO_TEST_CASE(addrip_v4_test_all)
{
	

	{
		addrip_v4 a1("127.0.0.1");
		BOOST_REQUIRE_EQUAL(a1.m_addr, unsigned long(2130706433));
	}

	{
		addrip_v4 a2("255.255.255.255");
		BOOST_REQUIRE_EQUAL(a2.m_addr, unsigned long(0xFFFFFFFF));
	}

	{
		addrip_v4 a3("0.0.0.0");
		BOOST_REQUIRE_EQUAL(a3.m_addr, unsigned long(0));
	}

	{
		std::ostringstream oss;
		oss << addrip_v4("0.0.0.255");
		BOOST_REQUIRE_EQUAL(oss.str(), std::string("0.0.0.255"));
	}

	{
		addrip_v4 a4;
		BOOST_REQUIRE_EQUAL(a4.from_string("256.0.0.1", true), bool(false));
	}

	{
		addrip_v4 a5;
		std::istringstream iss1("192.168.255.255");
		iss1 >> a5;
		BOOST_REQUIRE_EQUAL(a5.to_string(), std::string("192.168.255.255"));
	}

	{
		utm::addrip_v4 mask("255.255.255.1");
		BOOST_REQUIRE_EQUAL(mask.is_netmask(), bool(false));
		mask.m_addr--;
		BOOST_REQUIRE_EQUAL(mask.is_netmask(), bool(true));
	}

	{
		unsigned char b[4] = { 0xC0, 0xA8, 0x0, 0x2 };
		addrip_v4 a6;
		a6.set(b);
		BOOST_REQUIRE_EQUAL(a6.to_string(), std::string("192.168.0.2"));
	}
}

}
