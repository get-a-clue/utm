#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

namespace utm {

typedef std::pair<UINT, std::string> DnsData;

class DnsDataCompare
{
public:
	bool operator() (const DnsData& lhs,
		const DnsData& rhs) const
	{
		return keyLess(lhs.first, rhs.first);
	}

	bool operator() (const DnsData& lhs,
		const DnsData::first_type& k) const
	{
		return keyLess(lhs.first, k);
	}

	bool operator() (const DnsData::first_type& k,
		const DnsData& rhs) const
	{
		return keyLess(k, rhs.first);
	}

	static UINT GetStringKey(char *p)
	{
		UINT retval = 0;

		BYTE b[4];
		memset(b, 0, sizeof(b));

		int len = strlen(p);
		if (len > 4)
		{
			len = 4;
		}

		for(int i = 0; i < len; i++)
		{
			b[i] = *(p+i);
		}

		retval = MAKELONG(MAKEWORD(b[3], b[2]), MAKEWORD(b[1], b[0]));

		return retval;
	}

private:
	bool keyLess(const DnsData::first_type& k1,
		const DnsData::first_type& k2) const
	{
		return k1 < k2;
	}

};


class CDnsRecords
{
public:
	CDnsRecords(void);
	~CDnsRecords(void);

	std::vector<DnsData> m_dd;
	UINT m_defaultDnsServer;

	void Sort();
	void ReadFromFile(TCHAR *pszFile);
	bool Search(char *p);
	bool Search2Level(char *p);
	void PrintData();
};

}
