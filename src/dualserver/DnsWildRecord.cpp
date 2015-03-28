#include "stdafx.h"

#include "DnsWildRecord.h"

#include <algorithm>
using namespace std;

namespace utm {

CDnsRecords::CDnsRecords(void)
{
	m_defaultDnsServer = 0;
}

CDnsRecords::~CDnsRecords(void)
{
}

void CDnsRecords::Sort(void)
{
	sort(m_dd.begin(), m_dd.end(), DnsDataCompare());
}

void CDnsRecords::ReadFromFile(TCHAR *pszFile)
{
	m_dd.clear();

	TCHAR szLine[MAX_PATH];
	char sz[MAX_PATH];
	char *p = &sz[0];

	FILE *fp = NULL;
	if (_tfopen_s(&fp, pszFile, _T("rt")) == 0)
	{
		while (NULL != _fgetts(szLine, MAX_PATH, fp))
		{
			int len = _tcslen(szLine);
			if (len == 0)
			{
				continue;
			};

#ifdef _UNICODE
			memset(sz, 0, sizeof(sz));
			WideCharToMultiByte(CP_ACP, NULL, szLine, -1, sz, MAX_PATH, NULL, NULL);
#else
			strncpy_s(sz, MAX_PATH, szLine, _TRUNCATE);
#endif

			len = strlen(sz);

			while ((sz[len-1] == 10) || (sz[len-1] == 13) || (sz[len-1] == 32))
			{
				sz[len-1] = 0;
				len = len - 1;
				if (len == 0)
				{
					break;
				};
			}

			_strlwr(sz);

			UINT key = DnsDataCompare::GetStringKey(p);
			string value(p);

			pair<UINT, string> pa;
			pa.first = key;
			pa.second = value;

			m_dd.push_back(pa);
		}
		fclose(fp);
	}

	Sort();
}

bool CDnsRecords::Search2Level(char *p)
{
	char r[MAX_PATH];
	memset(r, 0, sizeof(r));

	int k = 0;
	int i = strlen(p);
	int dots = 0;
	char c;

	while (i > 0)
	{
		c = *(p + i - 1);
		if (c == '.')
		{
			dots++;

			if (dots == 2)
			{
				break;
			}
		}

		r[k] = *(p + i - 1);

		k++;
		i--;
	}

	_strrev(r);

	return Search(r);
}

bool CDnsRecords::Search(char *p)
{
	UINT key = DnsDataCompare::GetStringKey(p);
	vector<DnsData>::iterator iter = lower_bound(m_dd.begin(), m_dd.end(), key, DnsDataCompare());

	bool retval = false;

	if (iter != m_dd.end())
	{
		while (iter != m_dd.end())
		{
			if (iter->first != key)
			{
				break;
			}

			if (strcmp(p, iter->second.c_str()) == 0)
			{
				retval = true;
				break;
			}

			iter++;
		}
	}

	return retval;
}

void CDnsRecords::PrintData()
{
/*
	vector<DnsData>::iterator iter;
	for (iter = m_dd.begin(); iter != m_dd.end(); iter++)
	{
		cout << iter->second.c_str() << endl;
	}
*/
	cout << m_dd.size() << " records" << endl;
}

}