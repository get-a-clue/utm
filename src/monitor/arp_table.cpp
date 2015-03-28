#include "StdAfx.h"
#include "arp_table.h"

#include <ubase_test.h>

#include <Iphlpapi.h>
#pragma comment(lib, "Iphlpapi.lib")

namespace utm {

const char arp_table::this_class_name[] = "arp_table";

arp_table::arp_table(void)
{
}


arp_table::~arp_table(void)
{
}

void arp_table::clear()
{
	boost::mutex::scoped_lock lock(guard);
	entries.clear();
}

size_t arp_table::size() const
{
	boost::mutex::scoped_lock lock(guard);
	return entries.size();
}

void arp_table::fill()
{
	boost::mutex::scoped_lock lock(guard);

	entries.clear();

	PMIB_IPNETTABLE pMib = NULL;
	DWORD dwSize = 0;

	DWORD dwRet = GetIpNetTable(pMib, &dwSize, TRUE);

	if (dwSize > 0)
	{
		pMib = (PMIB_IPNETTABLE)malloc(dwSize);
		if (pMib != NULL)
		{
			dwRet = GetIpNetTable(pMib, &dwSize, TRUE);

			if ((dwRet == NO_ERROR) || (dwRet == ERROR_NO_DATA))
			{
				for (UINT i = 0; i < pMib->dwNumEntries; i++)
				{
					if ((pMib->table[i].dwType == MIB_IPNET_TYPE_DYNAMIC) || (pMib->table[i].dwType == MIB_IPNET_TYPE_STATIC))
					{
						addrip_v4 ip;
						ip.m_addr = pMib->table[i].dwAddr;
						ip.revert_octets();

						addrmac mac;
						mac.set(pMib->table[i].bPhysAddr);

						entries.insert(std::make_pair(ip, mac));
					}
				}
			}

			free(pMib);
		}
	}
}

bool arp_table::findmac(const addrip_v4& ip, addrmac& mac) const
{
	boost::mutex::scoped_lock lock(guard);

	arptable_container::const_iterator iter = entries.find(ip);
	if (iter == entries.end())
		return false;

	mac = iter->second;
	return true;
}

void arp_table::test_all()
{
	test_case::classname.assign(this_class_name);

	arp_table arp;
	arp.fill();

	return;
}

}
