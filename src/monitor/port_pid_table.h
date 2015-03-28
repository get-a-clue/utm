#ifndef UTM_PORT_PID_TABLE_H
#define UTM_PORT_PID_TABLE_H

#pragma once
#include <utm.h>

#include <Iphlpapi.h>

#include <proc_list.h>

namespace utm {

struct port_pid_row {
	unsigned short call_number;
	unsigned short pid_index;
};

#define UTM_MAX_PORTS 65536

class port_pid_table
{
public:
	port_pid_table(void);
	~port_pid_table(void);

	bool load_ports(unsigned char proto, const proc_list& prlist);

	unsigned short call_count;
	port_pid_row ports[UTM_MAX_PORTS];

private:
	DWORD (WINAPI *pGetExtendedTcpTable)(PVOID pTcpTable, PDWORD pdwSize, BOOL bOrder, ULONG ulAf, TCP_TABLE_CLASS TableClass, ULONG Reserved);
	DWORD (WINAPI *pGetExtendedUdpTable)(PVOID pTcpTable, PDWORD pdwSize, BOOL bOrder, ULONG ulAf, UDP_TABLE_CLASS TableClass, ULONG Reserved);

	bool LoadTcpTable(PVOID *pptable, DWORD *pdwSize);
	bool LoadUdpTable(PVOID *pptable, DWORD *pdwSize);

	inline bool update_entry(unsigned short pid_index, unsigned short local_port, const proc_list& prlist);
};

}

#endif // UTM_PORT_PID_TABLE_H
