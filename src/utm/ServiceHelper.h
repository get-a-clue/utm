#ifndef _UTM_SERVICEHELPER_H
#define _UTM_SERVICEHELPER_H

#pragma once
#include <utm.h>

#ifdef UTM_WIN

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <WinSvc.h>

#pragma comment(lib, "version.lib")

#define SERVICEHELPER_ACCESSERROR "Error! Couldn't open Service Manager. Probably, you don't have sufficient priviledges on this computer."
#define SERVICEHELPER_SERVICENOTEXIST "Service doesn't exist."
#define SERVICEHELPER_INSTALL "Service has been installed successfully."
#define SERVICEHELPER_UNINSTALL "Service has been uninstalled successfully."
#define SERVICEHELPER_ERRORCREATING "Error creating the service."
#define SERVICEHELPER_ERRORDELETING "Error deleting the service."
#define SERVICEHELPER_CHANGEOK "ChangeServiceParameters::OK"

#define SERVICEHELPER_WAITTIMEOUT 5000
#define SERVICEHELPER_SLEEPINTERVAL 100

namespace utm {

class CServiceHelper
{
public:
	CServiceHelper(void);
	~CServiceHelper(void);

	enum ServiceActions { ServiceStop = 0, ServiceStart = 1, ServiceIsInstalled = 2 };

	static BOOL Install(const TCHAR *pServiceName, const TCHAR *pPrgName, TCHAR *pErrBuf, int nErrBufLen);
	static BOOL Install(const TCHAR *pServiceName, const TCHAR *pPrgName, const TCHAR *pDependency, TCHAR *pErrBuf, int nErrBufLen);
	static BOOL UnInstall(const TCHAR *pServiceName, TCHAR *pErrBuf, int nErrBufLen);
	static BOOL Action(const TCHAR *pServiceName, ServiceActions action, TCHAR *pErrBuf, int nErrBufLen); 
	static BOOL Stop(SC_HANDLE hService);

	static BOOL GetServiceOptions(const TCHAR *pServiceName, const TCHAR *pDependencyName, DWORD *pStartupType, BOOL *pDependencyResult, TCHAR *pErrBuf, int nErrBufLen);
	static DWORD GetServiceStartupType(const TCHAR *pServiceName);
	static BOOL CheckServiceDependency(const TCHAR *pServiceName, const TCHAR *pDependencyName);
	static BOOL ChangeServiceParameters(const TCHAR *pServiceName, const TCHAR *pDescr, TCHAR *pErrBuf, int nErrBufLen);
};

}

#endif // UTM_WIN

#endif // _UTM_SERVICEHELPER_H
