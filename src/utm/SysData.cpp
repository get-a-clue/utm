#include "stdafx.h"
#include "SysData.h"

namespace utm {

CSysData::CSysData(void)
{
	#pragma warning(push)
	#pragma warning(disable:4996)
	ZeroMemory(&osv, sizeof(OSVERSIONINFOEX));
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((LPOSVERSIONINFO)&osv);
	#pragma warning(pop)
}

CSysData::~CSysData(void)
{
}

void CSysData::DetectServices(service_status_container& ssc)
{
	HMODULE hmAdvapiDll = NULL;
	pOpenSCManager m_pOpenSCManager = NULL;
	pCloseServiceHandle m_pCloseServiceHandle = NULL;
	pEnumServicesStatusEx m_pEnumServicesStatusEx = NULL;

	if ((hmAdvapiDll = ::LoadLibrary(_T("advapi32.dll"))) == NULL)
		return;

#ifndef _UNICODE
	m_pOpenSCManager = (pOpenSCManager)::GetProcAddress(hmAdvapiDll, "OpenSCManagerA");
#else
	m_pOpenSCManager = (pOpenSCManager)::GetProcAddress(hmAdvapiDll, "OpenSCManagerW");
#endif

	m_pCloseServiceHandle = (pCloseServiceHandle)::GetProcAddress(hmAdvapiDll, "CloseServiceHandle");

#ifndef _UNICODE
	m_pEnumServicesStatusEx = (pEnumServicesStatusEx)::GetProcAddress(hmAdvapiDll, "EnumServicesStatusExA");
#else
	m_pEnumServicesStatusEx = (pEnumServicesStatusEx)::GetProcAddress(hmAdvapiDll, "EnumServicesStatusExW");
#endif

	SC_HANDLE mgr = (*m_pOpenSCManager)(NULL, SERVICES_ACTIVE_DATABASE, GENERIC_READ);
	if (mgr != NULL)
	{
		DWORD needed = 0;
		DWORD num = 0;
		DWORD pos = 0;

		(*m_pEnumServicesStatusEx)(mgr, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_ACTIVE, NULL, 0, &needed, &num, &pos, NULL);

		size_t spsize = sizeof(ENUM_SERVICE_STATUS_PROCESS);

		if (needed > 0)
		{
			size_t cnt = 1 + (needed / spsize);

			ENUM_SERVICE_STATUS_PROCESS *m_pServiceData = new ENUM_SERVICE_STATUS_PROCESS[cnt];
			memset(m_pServiceData, 0, cnt*spsize);

			if ((*m_pEnumServicesStatusEx)(mgr, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_ACTIVE, (LPBYTE)m_pServiceData, cnt*spsize, &needed, &num, &pos, NULL))
			{
				ENUM_SERVICE_STATUS_PROCESS *psd = m_pServiceData;
				for (unsigned int i = 0; i < num; i++, psd++)
				{
					servstat_proc ssp;
					ssp.service_name.assign(psd->lpServiceName);
					ssp.display_name.assign(psd->lpDisplayName);
					memcpy(&ssp.service_status_process, &psd->ServiceStatusProcess, sizeof(SERVICE_STATUS_PROCESS));
					ssc.push_back(ssp);
				}
			}

			delete[] m_pServiceData;
		}

		(*m_pCloseServiceHandle)(mgr);
	}

	::FreeModule(hmAdvapiDll);
}

bool CSysData::IsWindowsServer() const
{
	return ((osv.wProductType == VER_NT_DOMAIN_CONTROLLER) || (osv.wProductType == VER_NT_SERVER));
}

#ifdef UTM_DEBUG
void CSysData::test_all()
{
	service_status_container ssc;
	DetectServices(ssc);


	return;
}
#endif

}
