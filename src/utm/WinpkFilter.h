#pragma once
#include <utm.h>

#define ADAPTER_NAME_SIZE	256
#define ADAPTER_LIST_SIZE	32
#define ETHER_ADDR_LENGTH	6

#ifndef _TCP_ADAPTER_LIST
#define _TCP_ADAPTER_LIST

typedef
struct _TCP_AdapterList
{
   unsigned long	m_nAdapterCount; // Number of adapters
   unsigned char	m_szAdapterNameList[ ADAPTER_LIST_SIZE ][ ADAPTER_NAME_SIZE ]; // Array of adapter names
   HANDLE			m_nAdapterHandle [ ADAPTER_LIST_SIZE ]; // Array of adapter handles, this are key handles for any adapter relative operation 
   unsigned int		m_nAdapterMediumList[ ADAPTER_LIST_SIZE ]; // List of adapter mediums
   unsigned char	m_czCurrentAddress[ ADAPTER_LIST_SIZE ][ ETHER_ADDR_LENGTH ]; // current (configured) ethernet address
   unsigned short	m_usMTU [ ADAPTER_LIST_SIZE ]; // current adapter MTU

} TCP_AdapterList, *PTCP_AdapterList;

#endif
