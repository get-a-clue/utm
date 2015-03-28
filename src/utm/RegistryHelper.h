#ifndef _UTM_REGISTRYHELPER_H
#define _UTM_REGISTRYHELPER_H

#pragma once
#include <utm.h>

#include <string>

#include <gstring.h>

namespace utm {

class RegistryHelper
{
public:
	RegistryHelper(void);
	~RegistryHelper(void);

	static LONG CreateKey(HKEY hk, const TCHAR* key);

	static LONG QueryValueDword(HKEY hk, const TCHAR* key, DWORD& dw);
	static LONG QueryValueString(HKEY hk, const TCHAR* key, DWORD buffersize, TCHAR* buffer);
	static LONG QueryValueBinary(HKEY hk, const TCHAR* key, DWORD& datasize, BYTE** databuffer);

	static LONG SetValueDword(HKEY hk, const char* key, DWORD dw);
	static LONG SetValueDword(HKEY hk, const WCHAR* key, DWORD dw);
	static LONG SetValueString(HKEY hk, const char* key, const char* str);
	static LONG SetValueString(HKEY hk, const WCHAR* key, const char* str);
	static LONG SetValueString(HKEY hk, const char* key, const std::string& str);
	static LONG SetValueString(HKEY hk, const WCHAR* key, const std::string& str);
	static LONG SetValueString(HKEY hk, const char* key, const utm::gstring& str);
	static LONG SetValueString(HKEY hk, const WCHAR* key, const utm::gstring& str);
	static LONG SetValueBinary(HKEY hk, const char* key, DWORD datasize, BYTE* databuffer);
	static LONG SetValueBinary(HKEY hk, const WCHAR* key, DWORD datasize, BYTE* databuffer);

private:
	static LONG SetValueStr(HKEY hk, const TCHAR* key, const char *str);
};

}

#endif // _UTM_REGISTRYHELPER_H
