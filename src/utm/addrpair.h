#ifndef _UTM_ADDRPAIR_H
#define _UTM_ADDRPAIR_H

#pragma once
#include <utm.h>

#define ADDRPAIR_DELIM_HYPEN '-'
#define ADDRPAIR_DELIM_COMMA ','

#include <string>
#include <ostream>
#include <memory>

namespace utm {

template<class T>
class addrpair 
{
public:
	addrpair(const char c = ADDRPAIR_DELIM_HYPEN)
	{
		delim = c;
	};

	addrpair(const T& a1, const T& a2, const char c = ADDRPAIR_DELIM_HYPEN)
	{
		addr1 = a1;
		addr2 = a2;
		delim = c;
	};

	~addrpair() { };

	char delim;
	T addr1;
	T addr2;

	bool operator ==(const addrpair<T>& rhs) const
	{
		if (addr1.is_default() && rhs.addr2.is_default())
			return addr2 == rhs.addr1;

		if (addr2.is_default() && rhs.addr1.is_default())
			return addr1 == rhs.addr2;

		if (addr1 != rhs.addr1)
			return false;

		if (addr2 != rhs.addr2)
			return false;

		return true;
	}

	bool operator !=(const addrpair<T>& rhs) const
	{
		bool result = operator==(rhs);

		return !result;
	}

	bool is_default() const
	{
		if ((true == addr1.is_default()) && (true == addr2.is_default()))
			return true;

		return false;
	}

	void set_delimiter(const char c)
	{
		delim = c;
	}

	void clear()
	{
		addr1.clear();
		addr2.clear();
	}

	std::string to_string(const char c = 0, bool exclude_default = false) const
	{
		std::ostringstream oss;

		bool use_addr1 = (!exclude_default) || (!addr1.is_default());
		bool use_addr2 = (!exclude_default) || (!addr2.is_default());

		if (use_addr1)
			oss << addr1;

		if ((use_addr1) && (use_addr2))
		{
			if (c == 0)
				oss << delim;
			else
				oss << c;
		}
		
		if (use_addr2)
			oss << addr2;

		return oss.str();
	}

	void from_string(const char* s, const char c = 0, bool allow_empty_second = false)
	{
		char mydelim = c == 0 ? delim : c;

		addr1.clear();
		addr2.clear();

		const char *str2 = strchr(s, mydelim);

		if (str2 != NULL)
		{
			std::string str(s, str2 - s);
			while (*str2 == mydelim)
			{
				str2++;
			}

			std::istringstream iss1(str);
			iss1 >> addr1;

			std::istringstream iss2(str2);
			iss2 >> addr2;
		}
		else if (allow_empty_second)
		{
			std::istringstream iss(s);
			iss >> addr1;
		}
	}

	
};

}

#endif  // _UTM_ADDRPAIR_H