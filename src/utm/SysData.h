#ifndef _UTM_SYSDATA_H
#define _UTM_SYSDATA_H

#pragma once
#include <utm.h>

#include <list>
#include <map>

#include <gstring.h>

typedef SC_HANDLE (WINAPI *pOpenSCManager)(LPCTSTR lpMachineName, LPCTSTR lpDatabaseName, DWORD dwDesiredAccess);
typedef BOOL (WINAPI *pCloseServiceHandle)(SC_HANDLE hSCObject);
typedef BOOL (WINAPI *pEnumServicesStatusEx)(SC_HANDLE hSCManager, SC_ENUM_TYPE InfoLevel, DWORD dwServiceType, DWORD dwServiceState, LPBYTE lpServices, DWORD cbBufSize, LPDWORD pcbBytesNeeded, LPDWORD lpServicesReturned, LPDWORD lpResumeHandle, LPCTSTR pszGroupName);

namespace utm {

struct servstat_proc
{
	utm::gstring service_name;
	utm::gstring display_name;
	SERVICE_STATUS_PROCESS service_status_process;

	bool is_service_running(const utm::gstring& _service_name) const
	{
		return ((service_name == _service_name) && (service_status_process.dwCurrentState & SERVICE_RUNNING));
	}
};

typedef std::list<servstat_proc> service_status_container;

class CSysData
{
public:
	CSysData(void);
	~CSysData(void);

	OSVERSIONINFOEX osv;
	bool IsWindowsServer() const;

	static void DetectServices(service_status_container& ssc);

#ifdef UTM_DEBUG
	static void test_all();
#endif
};

}

#endif // _UTM_SYSDATA_H
