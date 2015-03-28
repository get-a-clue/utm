#include "stdafx.h"
#include "ServiceHelper.h"

#ifdef UTM_WIN

namespace utm {

CServiceHelper::CServiceHelper(void)
{
}

CServiceHelper::~CServiceHelper(void)
{
}

BOOL CServiceHelper::Install(const TCHAR *pServiceName, const TCHAR *pPrgName, TCHAR *pErrBuf, int nErrBufLen)
{
	TCHAR szDependency[MAX_PATH];
	memset(szDependency, 0, sizeof(szDependency));

	return Install(pServiceName, pPrgName, szDependency, pErrBuf, nErrBufLen);
}

BOOL CServiceHelper::Install(const TCHAR *pServiceName, const TCHAR *pPrgName, const TCHAR *pDependency, TCHAR *pErrBuf, int nErrBufLen)
{
	if (Action(pServiceName, CServiceHelper::ServiceIsInstalled, pErrBuf, nErrBufLen))
	{
		_tcsncpy_s(pErrBuf, nErrBufLen, _T("The service is already installed."), _TRUNCATE);
        return TRUE;
	}

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
		_tcsncpy_s(pErrBuf, nErrBufLen, _T(SERVICEHELPER_ACCESSERROR), _TRUNCATE);
        return FALSE;
    };

    TCHAR szFilePath[MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, MAX_PATH);

	DWORD v_major = 0;
	DWORD v_minor = 0;
	DWORD v_build = 0;

	VS_FIXEDFILEINFO fileinfo;
	VS_FIXEDFILEINFO *fi = &fileinfo;
	UINT size_fileinfo = sizeof(fileinfo);

	DWORD InfoSize = GetFileVersionInfoSize(szFilePath, 0);
	if (InfoSize > 0)
	{
		LPVOID lpVoid = calloc(InfoSize, 1);
		if (lpVoid != NULL)
		{
			if (GetFileVersionInfo(szFilePath, NULL, InfoSize, lpVoid))
			{
				if (VerQueryValue(lpVoid, _T("\\"), (LPVOID *)&fi, &size_fileinfo))
				{
					v_major = ((fi->dwFileVersionMS >> 16) & 0x0000FFFF);
					v_minor = (fi->dwFileVersionMS & 0x0000FFFF);
					v_build = ((fi->dwFileVersionLS >> 16) & 0x0000FFFF);
					/*DWORD v_something = (fi->dwFileVersionLS & 0x0000FFFF);*/ // not used
				};
			};
			free(lpVoid);
		}
	};

	TCHAR szServiceDescr[MAX_PATH];
	_stprintf_s(szServiceDescr, MAX_PATH, _T("%s %u.%u.%u"), pPrgName, v_major, v_minor, v_build);

//	TCHAR szDependency[MAX_PATH];
//	memset(szDependency, 0, sizeof(szDependency));

    SC_HANDLE hService = ::CreateService(
        hSCM, pServiceName, szServiceDescr,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, pDependency, NULL, NULL);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
		_tcsncpy_s(pErrBuf, nErrBufLen, _T(SERVICEHELPER_ERRORCREATING), _TRUNCATE);
        return FALSE;
    }

    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

	_tcsncpy_s(pErrBuf, nErrBufLen, _T(SERVICEHELPER_INSTALL), _TRUNCATE);
	return TRUE;
}

BOOL CServiceHelper::UnInstall(const TCHAR *pServiceName, TCHAR *pErrBuf, int nErrBufLen)
{
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
		_tcsncpy_s(pErrBuf, nErrBufLen, _T(SERVICEHELPER_ACCESSERROR), _TRUNCATE);
        return FALSE;
    };

    SC_HANDLE hService = ::OpenService(hSCM, pServiceName, SERVICE_STOP | DELETE);
    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
		_tcsncpy_s(pErrBuf, nErrBufLen, _T(SERVICEHELPER_SERVICENOTEXIST), _TRUNCATE);
        return FALSE;
    }

	Stop(hService);

    BOOL bDelete = ::DeleteService(hService);

    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

	if (bDelete)
	{
		_tcsncpy_s(pErrBuf, nErrBufLen, _T(SERVICEHELPER_UNINSTALL), _TRUNCATE);
        return TRUE;
	};

	_tcsncpy_s(pErrBuf, nErrBufLen, _T(SERVICEHELPER_ERRORDELETING), _TRUNCATE);
	return TRUE;
}

BOOL CServiceHelper::Stop(SC_HANDLE hService)
{
    SERVICE_STATUS status;
    BOOL retval = ::ControlService(hService, SERVICE_CONTROL_STOP, &status);

	int i = 0;
	while(i < SERVICEHELPER_WAITTIMEOUT)
	{
	    if (!QueryServiceStatus(hService, &status)) break;
		if (status.dwCurrentState == SERVICE_STOPPED) break;
		i = i + SERVICEHELPER_SLEEPINTERVAL;
		Sleep(SERVICEHELPER_SLEEPINTERVAL);
	};

	return retval;
}

BOOL CServiceHelper::Action(const TCHAR *pServiceName, 
	ServiceActions action, TCHAR *pErrBuf, int nErrBufLen)
{
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
		_tcsncpy_s(pErrBuf, nErrBufLen, _T(SERVICEHELPER_ACCESSERROR), _TRUNCATE);
        return FALSE;
    };
	
	SC_HANDLE hService = NULL;

	switch (action)
	{
		case ServiceStop:
			hService = ::OpenService(hSCM, pServiceName, SERVICE_STOP);
			break;
		case ServiceStart:
			hService = ::OpenService(hSCM, pServiceName, SERVICE_START);
			break;
		default:
			hService = ::OpenService(hSCM, pServiceName, SERVICE_QUERY_CONFIG);
			break;
	};

    if (hService == NULL)
	{
        ::CloseServiceHandle(hSCM);
		_tcsncpy_s(pErrBuf, nErrBufLen, _T(SERVICEHELPER_SERVICENOTEXIST), _TRUNCATE);
        return FALSE;
	};

	BOOL bSuccess = TRUE;

	switch(action)
	{
		case ServiceStop:
			bSuccess = Stop(hService);
			break;

		case ServiceStart:
			bSuccess = ::StartService(hService, NULL, NULL);
			break;
	};

	if (!bSuccess)
	{
		DWORD dwErr = ::GetLastError();

		TCHAR szErr[2*MAX_PATH];
		memset(szErr, 0, sizeof(szErr));
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, 0, szErr, 2*MAX_PATH, NULL);

		switch (action)
		{
			case ServiceStop:
				_tcsncpy_s(pErrBuf, nErrBufLen, _T("Can't stop the service. Reason: "), _TRUNCATE);
				break;

			case ServiceStart:
				_tcsncpy_s(pErrBuf, nErrBufLen, _T("Can't start the service. Reason: "), _TRUNCATE);
				break;
		};

		_tcsncat_s(pErrBuf, nErrBufLen, szErr, _TRUNCATE);
	};

	::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

	return bSuccess;
}

BOOL CServiceHelper::GetServiceOptions(const TCHAR *pServiceName, const TCHAR *pDependencyName, 
	DWORD *pStartupType, BOOL *pDependencyResult, TCHAR *pErrBuf, int nErrBufLen)
{
	*pStartupType = 0;
	*pDependencyResult = FALSE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, GENERIC_READ);
    if (hSCM == NULL)
    {
		_tcsncpy_s(pErrBuf, nErrBufLen, _T(SERVICEHELPER_ACCESSERROR), _TRUNCATE);
        return FALSE;
    };

	SC_HANDLE hService = ::OpenService(hSCM, pServiceName, SERVICE_QUERY_CONFIG);
    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
		_tcsncpy_s(pErrBuf, nErrBufLen, _T(SERVICEHELPER_SERVICENOTEXIST), _TRUNCATE);
        return FALSE;
    }

	DWORD cbBufSize = 0;
	DWORD dwBytesNeeded = 0;
	LPQUERY_SERVICE_CONFIG lpsc = NULL;

    if (!QueryServiceConfig(hService, NULL, 0, &dwBytesNeeded))
    {
        DWORD dwError = GetLastError();
        if (ERROR_INSUFFICIENT_BUFFER == dwError)
        {
            cbBufSize = dwBytesNeeded;
            lpsc = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT, cbBufSize);

		    if( QueryServiceConfig(hService, lpsc, cbBufSize, &dwBytesNeeded))
		    {
				*pStartupType = lpsc->dwStartType;

				if (pDependencyName != NULL)
				{
					TCHAR szSvc[MAX_PATH];
					TCHAR *p = lpsc->lpDependencies;

					while ((*p != _T('\0')) && (*(p + 1) != _T('\0')))
					{
						_tcsncpy_s(szSvc, MAX_PATH, p, _TRUNCATE);

						if (_tcsicmp(szSvc, pDependencyName) == 0)
						{
							*pDependencyResult = TRUE;
							break;
						};
						p += _tcslen(szSvc) + 1;
					};
				}
			}
		}
	}

	if (lpsc != NULL)
		LocalFree(lpsc);

	if (hService != NULL)
		::CloseServiceHandle(hService);

	if (hSCM != NULL)
		::CloseServiceHandle(hSCM);

	return TRUE;
}

DWORD CServiceHelper::GetServiceStartupType(const TCHAR *pServiceName)
{
	TCHAR sz[MAX_PATH];

	DWORD dwResult = 0;
	BOOL bTmp;

	GetServiceOptions(pServiceName, NULL, &dwResult, &bTmp, sz, MAX_PATH);
	return dwResult;
}

BOOL CServiceHelper::CheckServiceDependency(const TCHAR *pServiceName, const TCHAR *pDependencyName)
{
	TCHAR sz[MAX_PATH];

	DWORD dwStartupType = 0;
	BOOL bRetval = FALSE;

	GetServiceOptions(pServiceName, pDependencyName, &dwStartupType, &bRetval, sz, MAX_PATH);
	return bRetval;
}

BOOL CServiceHelper::ChangeServiceParameters(const TCHAR *pServiceName, const TCHAR *pDescr, TCHAR *pErrBuf, int nErrBufLen)
{
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
		_tcsncpy_s(pErrBuf, nErrBufLen, _T(SERVICEHELPER_ACCESSERROR), _TRUNCATE);
        return FALSE;
    };

    SC_HANDLE hService = ::OpenService(hSCM, pServiceName, SERVICE_ALL_ACCESS);
    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
		_tcsncpy_s(pErrBuf, nErrBufLen, _T(SERVICEHELPER_SERVICENOTEXIST), _TRUNCATE);
        return FALSE;
    }

	// Change a full description of the service

	SERVICE_DESCRIPTION sd;
	sd.lpDescription = const_cast<TCHAR *>(pDescr);

    ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, (LPVOID)&sd);

	// Change a recovery action of the service

	SC_ACTION sa;
	sa.Type = SC_ACTION_RESTART;
	sa.Delay = 500;

	SERVICE_FAILURE_ACTIONS sf;
	memset(&sf, 0, sizeof(sf));
	sf.dwResetPeriod = 1;
	sf.cActions = 1;
	sf.lpsaActions = &sa;

    ChangeServiceConfig2(hService, SERVICE_CONFIG_FAILURE_ACTIONS, (LPVOID)&sf);

	::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

	_tcsncpy_s(pErrBuf, nErrBufLen, _T(SERVICEHELPER_CHANGEOK), _TRUNCATE);
	return TRUE;
}

}

#endif // UTM_WIN