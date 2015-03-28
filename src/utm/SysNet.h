#ifndef _UTM_SYSNET_H
#define _UTM_SYSNET_H

#pragma once
#include <utm.h>

#ifdef UTM_WIN

#include <addrip_v4.h>
#include <addrtablebase.h>

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#include <Winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "Ws2_32.lib")

typedef DWORD (WINAPI *pGetAdaptersAddresses)(ULONG Family, ULONG Flags, PVOID Reserved, PIP_ADAPTER_ADDRESSES AdapterAddresses, PULONG SizePointer);
typedef DWORD (WINAPI *pGetAdaptersInfo)(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);
typedef DWORD (WINAPI *pGetIfEntry)(PMIB_IFROW pIfRow);
typedef DWORD (WINAPI *pGetIpAddrTable)(PMIB_IPADDRTABLE pIpAddrTable, PULONG pdwSize, BOOL bOrder);

namespace utm {

class CSysNet
{
public:
	CSysNet(void);
	virtual ~CSysNet(void);

private:
	enum AdapterParam { PARAM_IPADDR = 1, PARAM_MASK = 2, PARAM_TYPE = 3, PARAM_INDEX = 4 };

public:
	virtual void Init();
	addrtablebase<addrip_v4> locals_ip_v4;

protected:
	BOOL	m_bNeedInit;
	BOOL	m_bIphlpapiDll;
	HMODULE	m_hmIphlpapiDll;
	BOOL	IsInitOk();

	pGetIfEntry m_GetIfEntry;
	pGetAdaptersAddresses m_GetAdaptersAddresses;
	pGetAdaptersInfo m_GetAdaptersInfo;
	pGetIpAddrTable m_GetIpAddrTable;

	PIP_ADAPTER_INFO m_pai;
	PIP_ADAPTER_ADDRESSES m_paa;
	int		m_nNumAdaptersIphlpapi;
	int		m_nNumAdaptersAdresses;

private:
	PIP_ADAPTER_INFO GetAdapterInfoPtr(int index) const;
	const char* GetAPIAddrOrMaskStr(AdapterParam param, int index, int indexIP) const;
	DWORD GetAPIAddrOrMask(AdapterParam param, int index, int indexIP) const;

public:
	UINT GetAPIAdapterHwAddressLength(int index) const;
	LPBYTE GetAPIAdapterHwAddressPtr(int index) const;
	BOOL GetAPIAdapterSpeedStatus(int index, DWORD *pdwSpeed, DWORD *pdwStatus) const;
	DWORD GetAPIAdapterIndex(int index) const;
	const char* GetAPIAdapterMaskStr(int index, int indexIP) const { return GetAPIAddrOrMaskStr(CSysNet::PARAM_MASK, index, indexIP); };
	const char* GetAPIAdapterIPStr(int index, int indexIP) const { return GetAPIAddrOrMaskStr(CSysNet::PARAM_IPADDR, index, indexIP); };
	DWORD GetAPIAdapterMask(int index, int indexIP) const { return GetAPIAddrOrMask(CSysNet::PARAM_MASK, index, indexIP); };
	DWORD GetAPIAdapterIP(int index, int indexIP) const { return GetAPIAddrOrMask(CSysNet::PARAM_IPADDR, index, indexIP); };
	int GetAPINumAddresses(int index) const;
	BOOL GetAdapter(int index, LPSTR pszName, int nNameSize, LPSTR pszDescr, int nDescrSize) const;
	int GetAPIAdapterType(int index) const;
	int GetAPINumAdapters() const { return m_nNumAdaptersIphlpapi; };
	void DetectAdaptersIphlpapi();
	void RefreshLocalAddresses();
	static void GetDefaultDnsServer(addrip_v4& addr);

	PIP_ADAPTER_INFO GetIpAdapterInfoPtr() { return m_pai; };
};
}

#endif // UTM_WIN

#endif // _UTM_SYSNET_H