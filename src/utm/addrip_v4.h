#ifndef _UTM_ADDRIP_V4_H
#define _UTM_ADDRIP_V4_H

#pragma once
#include <utm.h>

#include <string>
#include <iostream>
#include <sstream>

#define MAX_ADDRIPV4_STR_LEN 16

namespace utm {
class addrip_v4
{
public:
	static const char this_class_name[];

	addrip_v4(void);
	~addrip_v4(void);

	addrip_v4(const addrip_v4& r);
	addrip_v4(unsigned long addr);
	addrip_v4(const char *str);
	addrip_v4& operator=(const addrip_v4& rhs);
	addrip_v4& operator=(unsigned int rhs);
	bool operator ==(const addrip_v4& rhs) const;
	bool operator !=(const addrip_v4& rhs) const;
	bool operator <(const addrip_v4& rhs) const;
	bool operator >(const addrip_v4& rhs) const;
	bool operator <=(const addrip_v4& rhs) const;
	bool operator >=(const addrip_v4& rhs) const;    
    static addrip_v4 AND(const addrip_v4& lhs, const addrip_v4& rhs);

	void clear() { m_addr = 0; };
	void set(const unsigned char* buffer);
	void revert_octets();
	bool is_netmask() const;
	bool is_zero() const { return m_addr == 0; };
	bool is_default() const { return m_addr == 0; };
    bool from_string(const char *str, bool check = false);
    bool from_string(const std::string& str, bool check = false);
	std::string to_string() const;
	void as_hex(char* buffer) const;

	inline unsigned int get_octet1() const { return ((m_addr >> 24) & 0x000000FF); };
	inline unsigned int get_octet2() const { return ((m_addr >> 16) & 0x000000FF); };
	inline unsigned int get_octet3() const { return ((m_addr >> 8) & 0x000000FF); };
	inline unsigned int get_octet4() const { return (m_addr & 0x000000FF); };

private:
	inline static void to_strbuffer(unsigned long addr, char *buffer)
	{
		unsigned int a1 = (addr >> 24) & 0x000000FF;
		unsigned int a2 = (addr >> 16) & 0x000000FF;
		unsigned int a3 = (addr >> 8) & 0x000000FF;
		unsigned int a4 = addr & 0x000000FF;

#ifdef UTM_WIN
		sprintf_s(buffer, MAX_ADDRIPV4_STR_LEN, "%u.%u.%u.%u", a1, a2, a3, a4);
#else
		sprintf(buffer, "%u.%u.%u.%u", a1, a2, a3, a4);
#endif
	}

public:
	unsigned long m_addr;

#ifdef _DEBUG
	char m_addr2[MAX_ADDRIPV4_STR_LEN];
#endif

	friend std::ostream&  operator<<(std::ostream& s, const addrip_v4& rhs);
	friend std::istream& operator>>(std::istream& s, addrip_v4& obj);
	friend unsigned int& operator<<(unsigned int& r, const utm::addrip_v4& rhs) { r = rhs.m_addr; return r; };

	static void convert_ipmask_to_range(const addrip_v4& ip, const addrip_v4& mask, addrip_v4& startip, addrip_v4& endip);

#ifdef UTM_DEBUG
	static void test_all();
#endif
};

}

typedef utm::addrip_v4 CAddrIp4;

#endif // _UTM_ADDRIP_V4_H