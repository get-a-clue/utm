#include "stdafx.h"

#include "SysNet.h"

#ifdef UTM_WIN
#include "addrip_v4.h"

#include <Windns.h>
#pragma comment(lib, "Dnsapi.lib")

namespace utm {

CSysNet::CSysNet(void)
{
	m_bNeedInit = FALSE;
	m_hmIphlpapiDll = NULL;
	m_bIphlpapiDll = FALSE;

	m_GetAdaptersAddresses = NULL;
	m_GetAdaptersInfo = NULL;
	m_GetIfEntry = NULL;
	m_GetIpAddrTable = NULL;

	m_nNumAdaptersIphlpapi = 0;
	m_nNumAdaptersAdresses = 0;
	m_pai = NULL;
	m_paa = NULL;
}

CSysNet::~CSysNet(void)
{
	free(m_pai);
	free(m_paa);

	if (m_bIphlpapiDll) 
	{
		FreeModule(m_hmIphlpapiDll);
	}
}

void CSysNet::Init()
{
	if (!m_bNeedInit)
	{
		if((m_hmIphlpapiDll = ::LoadLibrary(_T("iphlpapi.dll"))) != NULL)
		{
			// FARPROC GetProcAddress(
			//		HMODULE hModule,    // handle to DLL module
			//		LPCSTR lpProcName   // function name
			// );
			m_bIphlpapiDll = TRUE;
			m_GetAdaptersAddresses = (pGetAdaptersAddresses)::GetProcAddress(m_hmIphlpapiDll, "GetAdaptersAddresses");
			m_GetAdaptersInfo = (pGetAdaptersInfo)::GetProcAddress(m_hmIphlpapiDll, "GetAdaptersInfo");
			m_GetIfEntry = (pGetIfEntry)::GetProcAddress(m_hmIphlpapiDll, "GetIfEntry");
			m_GetIpAddrTable = (pGetIpAddrTable)::GetProcAddress(m_hmIphlpapiDll, "GetIpAddrTable");

			DetectAdaptersIphlpapi();
		};

		m_bNeedInit = FALSE;
	}
}

BOOL CSysNet::IsInitOk()
{
	if (m_GetAdaptersInfo == NULL)
	{
		Init();
		if (m_GetAdaptersInfo == NULL)
			return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//
// Get Network Adapters Info with iphlpapi
//     (only for Windows 2000+ or Windows 98+)
//
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void CSysNet::DetectAdaptersIphlpapi()
{
	if (!IsInitOk())
	{
		return;
	}

	m_nNumAdaptersIphlpapi = 0;

	if (m_pai != NULL)
	{
		free(m_pai);
		m_pai = NULL;
	}

	if (m_paa != NULL)
	{
		free(m_paa);
		m_paa = NULL;
	}

	if (m_GetAdaptersInfo != NULL)
	{
		ULONG t = 0;
		PIP_ADAPTER_INFO pip = NULL;
		(*m_GetAdaptersInfo)(pip, &t);

		if (t > 0)
		{
			m_pai = (PIP_ADAPTER_INFO)malloc(t);

			if (m_pai != NULL)
			{
				DWORD r = (*m_GetAdaptersInfo)(m_pai, &t);
				if (r == ERROR_SUCCESS)
				{
					PIP_ADAPTER_INFO tmp_pai = m_pai;

					while (tmp_pai != NULL)
					{
						m_nNumAdaptersIphlpapi++;
						tmp_pai = tmp_pai->Next;
					};
				}
			}
		}
	}

	if (m_GetAdaptersAddresses != NULL)
	{
		ULONG t = 0;
		PIP_ADAPTER_ADDRESSES pip = NULL;
		(*m_GetAdaptersAddresses)(AF_INET, 0, NULL, pip, &t);

		if (t > 0)
		{
			m_paa = (PIP_ADAPTER_ADDRESSES)malloc(t);

			if (m_paa != NULL)
			{
				DWORD r = (*m_GetAdaptersAddresses)(AF_INET, 0, NULL, m_paa, &t);
				if (r == ERROR_SUCCESS)
				{
					PIP_ADAPTER_ADDRESSES tmp_paa = m_paa;

					while (tmp_paa != NULL)
					{
						m_nNumAdaptersAdresses++;
						tmp_paa = tmp_paa->Next;
					};
				}
			}
		}
	}
}

PIP_ADAPTER_INFO CSysNet::GetAdapterInfoPtr(int index) const
{
	if (m_pai == NULL)
		return NULL;

	if (m_nNumAdaptersIphlpapi <= index)
		return NULL;

	PIP_ADAPTER_INFO retval = m_pai;
	while(index > 0)
	{
		retval = retval->Next;
		index--;
	}

	return retval;
}

BOOL CSysNet::GetAdapter(int index, LPSTR pszName, int nNameSize, LPSTR pszDescr, int nDescrSize) const
{
	PIP_ADAPTER_INFO pai = GetAdapterInfoPtr(index);
	if (pai == NULL)
		return NULL;

	strncpy_s(pszName, nNameSize, pai->AdapterName, _TRUNCATE);
	strncpy_s(pszDescr, nDescrSize, pai->Description, _TRUNCATE);

	int nType = pai->Type;

	if ((pszDescr[0] == 0) || (nType == MIB_IF_TYPE_PPP))
	{
		PIP_ADAPTER_ADDRESSES paa = m_paa;
		do
		{
			if (strcmp(pszName, paa->AdapterName) == 0)
			{
				if (nType == MIB_IF_TYPE_PPP)
				{
					WideCharToMultiByte(CP_ACP, NULL, paa->FriendlyName, -1, pszDescr, nDescrSize, NULL, NULL);
				}
				else
				{
					WideCharToMultiByte(CP_ACP, NULL, paa->Description, -1, pszDescr, nDescrSize, NULL, NULL);
				}
				break;
			}

			paa = paa->Next;
		} while (paa != NULL);
	}

	return TRUE;
}

int CSysNet::GetAPINumAddresses(int index) const
{
	PIP_ADAPTER_INFO pai = GetAdapterInfoPtr(index);
	if (pai == NULL)
		return NULL;

	// walk to the ip address list to count all ip addresses of given adapter

	int retval = 1; 
	PIP_ADDR_STRING pip = const_cast<PIP_ADDR_STRING>(&pai->IpAddressList);
	while (pip->Next != NULL)
	{
		retval++; 
		pip = pip->Next;
	};

	return retval;
}

// This function returns the IP address or mask of given adapter
// Incoming parameters:		index - zero-based index of network adapter
//							indexIP - zero-based index of IP address of network adapter
//
const char* CSysNet::GetAPIAddrOrMaskStr(AdapterParam param, int index, int indexIP) const
{
	PIP_ADAPTER_INFO pai = GetAdapterInfoPtr(index);
	if (pai == NULL)
		return NULL;

	int j = 0; 
	PIP_ADDR_STRING pip = const_cast<PIP_ADDR_STRING>(&pai->IpAddressList);

	do
	{
		if (j == indexIP)
		{
			if (param == CSysNet::PARAM_IPADDR)
				return (const char *)pip->IpAddress.String;

			if (param == CSysNet::PARAM_MASK)
				return (const char *)pip->IpMask.String;
		}

		j++; 
		pip = pip->Next;
	} 
	while(pip != NULL);

	return NULL;
}

DWORD CSysNet::GetAPIAddrOrMask(AdapterParam param, int index, int indexIP) const
{
	const char *p = GetAPIAddrOrMaskStr(param, index, indexIP);

	addrip_v4 addr;
	addr.from_string(p);

	return addr.m_addr;
}

int CSysNet::GetAPIAdapterType(int index) const
{
	PIP_ADAPTER_INFO pai = GetAdapterInfoPtr(index);
	if (pai == NULL)
		return 0;

	return pai->Type;
}

DWORD CSysNet::GetAPIAdapterIndex(int index) const
{
	PIP_ADAPTER_INFO pai = GetAdapterInfoPtr(index);
	if (pai == NULL)
		return 0xFFFFFFFF;

	return pai->Index;
}

UINT CSysNet::GetAPIAdapterHwAddressLength(int index) const
{
	PIP_ADAPTER_INFO pai = GetAdapterInfoPtr(index);
	if (pai == NULL)
		return 0;

	return pai->AddressLength;
};

LPBYTE CSysNet::GetAPIAdapterHwAddressPtr(int index) const
{
	PIP_ADAPTER_INFO pai = GetAdapterInfoPtr(index);
	if (pai == NULL)
		return NULL;

	return &pai->Address[0];
};

BOOL CSysNet::GetAPIAdapterSpeedStatus(int index, DWORD *pdwSpeed, DWORD *pdwStatus) const
{
	// This function works only in Windows 98+ or in Windows NT 4.0 SP4+

	if (m_GetIfEntry == NULL) 
		return FALSE;

	DWORD AdapterIndex = GetAPIAdapterIndex(index);
	if (AdapterIndex == 0xFFFFFFFF)
		return FALSE;

	MIB_IFROW ifrow;
	memset(&ifrow, 0, sizeof(MIB_IFROW));
	ifrow.dwIndex = AdapterIndex;

	if ((*m_GetIfEntry)(&ifrow) == NO_ERROR)
	{	
		*pdwSpeed = ifrow.dwSpeed;
		*pdwStatus = ifrow.dwOperStatus;
		return TRUE;
	}

	return FALSE;
}

void CSysNet::RefreshLocalAddresses()
{
	// RefreshLocalAddresses() function makes a list of all IP addresses belonged of this PC.

	if (m_GetIpAddrTable == NULL)
		Init();

	if (m_GetIpAddrTable == NULL)
		return;

	addrtablebase<addrip_v4> tmp;

	PMIB_IPADDRTABLE pAddrTable = NULL;
	DWORD dwAddrTableSize = 0;
	
	(*m_GetIpAddrTable)(pAddrTable, &dwAddrTableSize, TRUE);

	if (dwAddrTableSize > 0)
	{
		pAddrTable = (PMIB_IPADDRTABLE)malloc(dwAddrTableSize);
		if (pAddrTable != NULL)
		{
			if ((*m_GetIpAddrTable)(pAddrTable, &dwAddrTableSize, TRUE) == NO_ERROR)
			{
				DWORD dwNum = pAddrTable->dwNumEntries;
				for (DWORD i = 0; i < dwNum; i++)
				{
					addrip_v4 addr(pAddrTable->table[i].dwAddr);
					addrip_v4 mask(pAddrTable->table[i].dwMask);
					if (addr.m_addr != 0)
					{
						addr.revert_octets();
						mask.revert_octets();
						tmp.AddAddrPair(addr, mask, false);
					};
				};
			};
			free(pAddrTable);
		};
	};

	if (locals_ip_v4.is_equal(tmp))
		return;

	locals_ip_v4 = tmp;
}

void CSysNet::GetDefaultDnsServer(addrip_v4& addr)
{
	DNS_STATUS status;
	DNS_CONFIG_TYPE	Config = DnsConfigDnsServerList;
	DWORD dwBufferLength = 1024;
	BYTE dwBuf[1024];
	DWORD dwDns = 0;

	status = DnsQueryConfig(Config, FALSE, NULL, NULL, &dwBuf, &dwBufferLength);
	if (status == 0)
	{
		if (dwBuf[0] != 0)
		{
			dwDns = (dwBuf[4] << 24) + (dwBuf[5] << 16) + (dwBuf[6] << 8) + dwBuf[7];
		}
	}

	addr.m_addr = dwDns;
}

}

#endif
