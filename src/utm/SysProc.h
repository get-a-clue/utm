#ifndef _UTM_SYSPROC_H
#define _UTM_SYSPROC_H

#pragma once

#ifdef UTM_WIN
#include <Psapi.h>
#include <process.h>
typedef	BOOL(WINAPI *pGetProcessMemoryInfo)(HANDLE, PPROCESS_MEMORY_COUNTERS, DWORD);
#endif

#include "SysNet.h"

namespace utm {

class CSysProc : public CSysNet
{
public:
	CSysProc();
	virtual ~CSysProc();

	virtual void Init();
	unsigned int GetProcessMemoryUsage();

protected:
#ifdef UTM_WIN
	bool	m_bPsapiDll;
	HMODULE m_hmPsapiDll;
	pGetProcessMemoryInfo m_GetProcessMemoryInfo;
#endif
};

}

#endif // _UTM_SYSPROC_H

