#include "stdafx.h"
#include "SysProc.h"

namespace utm {

CSysProc::CSysProc() : CSysNet()
{
	m_bPsapiDll = false;
}

CSysProc::~CSysProc()
{
#ifdef UTM_WIN
	if (m_bPsapiDll)
	{
		FreeModule(m_hmPsapiDll);
	}
#endif
}

void CSysProc::Init()
{
	CSysNet::Init();

#ifdef UTM_WIN
	if ((m_hmPsapiDll = ::LoadLibrary(_T("psapi.dll"))) != NULL)
	{
		m_bPsapiDll = true;
		m_GetProcessMemoryInfo = (pGetProcessMemoryInfo)::GetProcAddress(m_hmPsapiDll, "GetProcessMemoryInfo");
	};
#endif
}

// Detect the size of the current process in KiloBytes
unsigned int CSysProc::GetProcessMemoryUsage()
{
#ifdef UTM_WIN
	if (m_GetProcessMemoryInfo != NULL)
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _getpid());
		if (hProcess)
		{
			PROCESS_MEMORY_COUNTERS m_ProcMemCounters;
			m_GetProcessMemoryInfo(hProcess, &m_ProcMemCounters, sizeof(m_ProcMemCounters));
			CloseHandle(hProcess);
			return (unsigned int)(m_ProcMemCounters.WorkingSetSize / 1024);
		}
	}
#endif
	return 0;
}

}