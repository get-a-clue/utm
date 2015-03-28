#include "stdafx.h"

#include "RegistryHelper.h"

#include <gstring.h>

namespace utm {

RegistryHelper::RegistryHelper(void)
{
}


RegistryHelper::~RegistryHelper(void)
{
}

LONG RegistryHelper::CreateKey(HKEY hk, const TCHAR* key)
{
	HKEY h;
	LONG retval = RegCreateKeyEx(hk, key, 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &h, NULL);
	return retval;
}

LONG RegistryHelper::QueryValueDword(HKEY hk, const TCHAR* key, DWORD& dw)
{
	dw = 0;
	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof(DWORD);
	LONG res = RegQueryValueEx(hk, key, 0, &dwType, (LPBYTE)&dw, &dwSize);
	return res;
}

LONG RegistryHelper::QueryValueString(HKEY hk, const TCHAR* key, DWORD buffersize, TCHAR* buffer)
{
	memset(buffer, 0, buffersize);
	DWORD dwType = REG_SZ;
	LONG res = RegQueryValueEx(hk, key, 0, &dwType, (LPBYTE)buffer, &buffersize);
	return res;
}

LONG RegistryHelper::QueryValueBinary(HKEY hk, const TCHAR* key, DWORD& datasize, BYTE** databuffer)
{
	DWORD dwType = REG_BINARY;
	DWORD dwSize = 0;

	LONG retval = RegQueryValueEx(hk, key, NULL, &dwType, NULL,	&dwSize);
	if (ERROR_SUCCESS == retval)
	{
		*databuffer = new BYTE[dwSize];
		memset(*databuffer, 0, dwSize);

		retval = RegQueryValueEx(hk, key, NULL, &dwType, *databuffer, &dwSize);
	}

	datasize = dwSize;

	return retval;
}

LONG RegistryHelper::SetValueDword(HKEY hk, const char* key, DWORD dw)
{
#ifndef _UNICODE
	const TCHAR *p = key;
#else
	std::wstring w = gstring(key).getw();
	const TCHAR *p = w.c_str();
#endif

	return RegSetValueEx(hk, p, 0, REG_DWORD, (LPBYTE)&dw, sizeof(DWORD));
}

LONG RegistryHelper::SetValueDword(HKEY hk, const WCHAR* key, DWORD dw)
{
#ifndef _UNICODE
	std::string s = gstring(key).getmb();
	const TCHAR *p = s.c_str();
#else
	const TCHAR *p = key;
#endif

	return RegSetValueEx(hk, p, 0, REG_DWORD, (LPBYTE)&dw, sizeof(DWORD));
}

LONG RegistryHelper::SetValueString(HKEY hk, const char* key, const char* str)
{
#ifndef _UNICODE
	const TCHAR *p = key;
#else
	std::wstring w = gstring(key).getw();
	const TCHAR *p = w.c_str();
#endif

	return SetValueStr(hk, p, str);
}

LONG RegistryHelper::SetValueString(HKEY hk, const WCHAR* key, const char* str)
{
#ifndef _UNICODE
	std::string s = gstring(key).getmb();
	const TCHAR *p = s.c_str();
#else
	const TCHAR *p = key;
#endif

	return SetValueStr(hk, p, str);
}

LONG RegistryHelper::SetValueString(HKEY hk, const char* key, const std::string& str)
{
#ifndef _UNICODE
	const TCHAR *p = key;
#else
	std::wstring w = gstring(key).getw();
	const TCHAR *p = w.c_str();
#endif

	return SetValueStr(hk, p, str.c_str());
}

LONG RegistryHelper::SetValueString(HKEY hk, const WCHAR* key, const std::string& str)
{
#ifndef _UNICODE
	std::string s = gstring(key).getmb();
	const TCHAR *p = s.c_str();
#else
	const TCHAR *p = key;
#endif

	return SetValueStr(hk, p, str.c_str());
}

LONG RegistryHelper::SetValueString(HKEY hk, const char* key, const utm::gstring& str)
{
#ifndef _UNICODE
	const TCHAR *p = key;
#else
	std::wstring w = gstring(key).getw();
	const TCHAR *p = w.c_str();
#endif

	LPBYTE ptr = (LPBYTE)str.c_str();
	DWORD dwSize = static_cast<DWORD>(str.size()) * static_cast<DWORD>(sizeof(TCHAR));

	return RegSetValueEx(hk, p, 0, REG_SZ, ptr, dwSize);
}

LONG RegistryHelper::SetValueString(HKEY hk, const WCHAR* key, const utm::gstring& str)
{
#ifndef _UNICODE
	std::string s = gstring(key).getmb();
	const TCHAR *p = s.c_str();
#else
	const TCHAR *p = key;
#endif

	LPBYTE ptr = (LPBYTE)str.c_str();
	DWORD dwSize = static_cast<DWORD>(str.size()) * static_cast<DWORD>(sizeof(TCHAR));

	return RegSetValueEx(hk, p, 0, REG_SZ, ptr, dwSize);
}

LONG RegistryHelper::SetValueStr(HKEY hk, const TCHAR* key, const char *str)
{
	LPBYTE ptr = NULL;
	DWORD dwSize = 0;
	size_t srcsize = strlen(str);

#ifndef _UNICODE
	ptr = (LPBYTE)str;
	dwSize = srcsize;
#else
	std::wstring ws = utm::gstring(str).getw();
	ptr = (LPBYTE)(const_cast<wchar_t*>(ws.c_str()));
	dwSize = static_cast<DWORD>(sizeof(WCHAR)) * static_cast<DWORD>(ws.size());
#endif

	return RegSetValueEx(hk, key, 0, REG_SZ, ptr, dwSize);
}

LONG RegistryHelper::SetValueBinary(HKEY hk, const char* key, DWORD datasize, BYTE* databuffer)
{
#ifndef _UNICODE
	const TCHAR *p = key;
#else
	std::wstring w = gstring(key).getw();
	const TCHAR *p = w.c_str();
#endif

	return RegSetValueEx(hk, p, 0, REG_BINARY,	(LPBYTE)databuffer,	datasize);
}

LONG RegistryHelper::SetValueBinary(HKEY hk, const WCHAR* key, DWORD datasize, BYTE* databuffer)
{
#ifndef _UNICODE
	std::string s = gstring(key).getmb();
	const TCHAR *p = s.c_str();
#else
	const TCHAR *p = key;
#endif

	return RegSetValueEx(hk, p, 0, REG_BINARY,	(LPBYTE)databuffer,	datasize);
}

}