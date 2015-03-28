#include "stdafx.h"
#include "port_pid_table.h"

namespace utm {

port_pid_table::port_pid_table(void)
{
	call_count = 0;
	memset(&ports, 0, sizeof(port_pid_row)*UTM_MAX_PROC_COUNT);
	pGetExtendedTcpTable = (DWORD (WINAPI *)(PVOID,PDWORD,BOOL,ULONG,TCP_TABLE_CLASS,ULONG))GetProcAddress(LoadLibrary(_T("iphlpapi.dll")), "GetExtendedTcpTable");
	pGetExtendedUdpTable = (DWORD (WINAPI *)(PVOID,PDWORD,BOOL,ULONG,UDP_TABLE_CLASS,ULONG))GetProcAddress(LoadLibrary(_T("iphlpapi.dll")), "GetExtendedUdpTable");
}


port_pid_table::~port_pid_table(void)
{
}

bool port_pid_table::LoadTcpTable(PVOID *pptable, DWORD *pdwSize)
{
	if (pGetExtendedTcpTable == NULL)
		return false;

	*pptable = NULL;
	*pdwSize = 0;

	DWORD result = 0;

	try
	{
		// Вызов GetExtendedTcpTable необходимо заключать в try/catch
		// для ловли exception в в процессе загрузки Windows

		result = pGetExtendedTcpTable(NULL, pdwSize, true, AF_INET, TCP_TABLE_OWNER_PID_CONNECTIONS, 0);

		if (*pdwSize == 0)
			return false;

		*pptable = malloc(*pdwSize);
		if (*pptable == NULL)
			return false;

		result = pGetExtendedTcpTable(*pptable, pdwSize, true, AF_INET, TCP_TABLE_OWNER_PID_CONNECTIONS, 0);
	}
	catch(...)
	{
		if (*pptable != NULL) free(*pptable);
		return false;
	}

	return true;
}

bool port_pid_table::LoadUdpTable(PVOID *pptable, DWORD *pdwSize)
{
	if (pGetExtendedTcpTable == NULL)
		return false;

	*pptable = NULL;
	*pdwSize = 0;

	DWORD result = 0;

	try
	{
		// Вызов GetExtendedUdpTable необходимо заключать в try/catch
		// для ловли exception в в процессе загрузки Windows

		result = pGetExtendedUdpTable(*pptable, pdwSize, true, AF_INET, UDP_TABLE_OWNER_PID, 0);

		if (*pdwSize == 0)
			return false;

		*pptable = malloc(*pdwSize);
		if (*pptable == NULL)
			return false;

		result = pGetExtendedUdpTable(*pptable, pdwSize, true, AF_INET, UDP_TABLE_OWNER_PID, 0);
	}
	catch(...)
	{
		if (*pptable != NULL) free(*pptable);
		return false;
	}

	return true;
}

bool port_pid_table::load_ports(unsigned char proto, const proc_list& prlist)
{
	bool need_refresh_proclist = false;

	PVOID ptable = NULL;
	DWORD size = 0;
	unsigned short port_local;
	unsigned short pid_index;

	if (proto == 6)
	{
		if (!LoadTcpTable(&ptable, &size))
			return false;

		PMIB_TCPTABLE_OWNER_PID t = (PMIB_TCPTABLE_OWNER_PID)ptable;
		unsigned int dwNum = t->dwNumEntries;

		for (unsigned int i = 0; i < dwNum; i++)
		{
			port_local = (((static_cast<unsigned short>(t->table[i].dwLocalPort)) & 0xFF) << 8)
				+ (((static_cast<unsigned short>(t->table[i].dwLocalPort)) & 0xFF00) >> 8);

			if ((t->table[i].dwOwningPid > 0) && (t->table[i].dwState < 12))
			{
				// Получаем индекс ячейки процесса внутри класса CProcList
				pid_index = static_cast<unsigned short>(t->table[i].dwOwningPid / 4);

				if (update_entry(pid_index, port_local, prlist))
					need_refresh_proclist = true;
			}
		}
		free(ptable);
	}

	if (proto == 17)
	{
		if (!LoadUdpTable(&ptable, &size))
			return false;

		PMIB_UDPTABLE_OWNER_PID t = (PMIB_UDPTABLE_OWNER_PID)ptable;
		unsigned int dwNum = t->dwNumEntries;

		for (unsigned int i = 0; i < dwNum; i++)
		{
			port_local = (((static_cast<unsigned short>(t->table[i].dwLocalPort)) & 0xFF) << 8)
				+ (((static_cast<unsigned short>(t->table[i].dwLocalPort)) & 0xFF00) >> 8);

			if (t->table[i].dwOwningPid > 0)
			{
				// Получаем индекс ячейки процесса внутри класса CProcList
				pid_index = static_cast<unsigned short>(t->table[i].dwOwningPid / 4);

				if (update_entry(pid_index, port_local, prlist))
					need_refresh_proclist = true;
			}
		}
		free(ptable);
	}

	call_count++;
	return need_refresh_proclist;
}

bool port_pid_table::update_entry(unsigned short pid_index, unsigned short port_local, const proc_list& prlist)
{
	ports[port_local].pid_index = pid_index;
	ports[port_local].call_number = call_count + 1;

	if (pid_index < UTM_MAX_PROC_COUNT)
	{
		// Проверяем самый младший бит в битовом наборе wFlags

		proc_info* pi = prlist.proc_ptr.get();
		if (pi != NULL)
		{
			if ((pi[pid_index].flag & 0x0001) == 0)
			{
				return true;
			}
		}
	}

	return false;
}

}
