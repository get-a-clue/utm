#include "stdafx.h"

#include "gstring.h"
#include "stringtools.h"

#include <boost/lexical_cast.hpp>

namespace utm {

gstring::gstring()
{
}

gstring::~gstring()
{
}

gstring::gstring(const gstring& s)
{
	assign(s);
}

gstring::gstring(const std::string& s)
{
	assign(s.c_str());
}

gstring::gstring(const char* s)
{
	assign(s);
}

#ifdef UTM_WIN
gstring::gstring(const std::wstring& s)
{
	assign(s.c_str());
}

gstring::gstring(const WCHAR* s)
{
	assign(s);
}
#endif

bool gstring::operator <(const gstring& rhs) const
{
	if (content < rhs.content)
		return true;

	return false;
}

bool gstring::operator==(const gstring& rhs) const
{
	if (content == rhs.content)
		return true;

	return false;
}

bool gstring::operator!=(const gstring& rhs) const
{
	bool result = operator==(rhs);

	return !result;
}

bool gstring::is_ci_equal(const gstring& rhs) const
{
#ifdef UTM_WIN
#ifndef UNICODE
	return (_stricmp(content.c_str(), rhs.c_str()) == 0);
#else
	return (_wcsicmp(content.c_str(), rhs.c_str()) == 0);
#endif
#endif

	return content == rhs.content;
}

int gstring::gstrncmp(const gstring& rhs, size_t size) const
{
#ifdef UTM_WIN
#ifndef UNICODE
	return strncmp(content.c_str(), rhs.content.c_str(), size);
#else
	return wcsncmp(content.c_str(), rhs.content.c_str(), size);
#endif
#else
	return strncmp(content.c_str(), rhs.content.c_str(), size);
#endif
}

int gstring::gstrncmp(const gchar* rhs, size_t size) const
{
#ifdef UTM_WIN
#ifndef UNICODE
	return strncmp(content.c_str(), rhs, size);
#else
	return wcsncmp(content.c_str(), rhs, size);
#endif
#else
	return strncmp(content.c_str(), rhs, size);
#endif
}

int gstring::gstrcmp(const gchar* rhs) const
{
#ifdef UTM_WIN
#ifndef UNICODE
	return strcmp(content.c_str(), rhs);
#else
	return wcscmp(content.c_str(), rhs);
#endif
#else
	return strcmp(content.c_str(), rhs);
#endif
}

void gstring::append(const char *p, size_t n)
{
	assign(p, true, n);
}

void gstring::append(const gstring& g)
{
	assign(g.c_str(), true, 0);
}

void gstring::append(int r)
{
	assign(boost::lexical_cast<std::string>(r).c_str(), true, 0);
}

void gstring::assign(const char *p, bool appendmode, size_t n)
{
	if (!appendmode)
		content.clear();

	if (p == NULL)
	{
		if (!appendmode)
			content.clear();
	
		return;
	}

#ifdef UTM_WIN
#ifndef _UNICODE
	if (!appendmode)
		content.assign(p);
	else
	{
		if (n == 0)
			content.append(p);
		else
			content.append(p, n);
	}
#else
	DWORD len = static_cast<DWORD>(strlen(p));
	wchar_t *u = new wchar_t[len + 1];
	memset(u, 0, (len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, NULL, p, -1, u, len);

	if (!appendmode)
		content.assign(u);
	else
	{
		if (n == 0)
			content.append(u);
		else
			content.append(u, n);
	}

	delete[] u;
#endif
#else
	if (!appendmode)
		content.assign(p);
	else
	{
		if (n == 0)
			content.append(p);
		else
			content.append(p, n);
	}
#endif
}

void gstring::assign(const std::string& s)
{
	assign(s.c_str());
}

void gstring::assign(const gstring& s)
{
	content = s.get();
}

#ifdef UTM_WIN
void gstring::append(const WCHAR *p, size_t n)
{
	assign(p, true, n);
}

void gstring::assign(const WCHAR *p, bool appendmode, size_t n)
{
	if (!appendmode)
		content.clear();

	if (p == NULL)
	{
		if (!appendmode)
			content.clear();
	
		return;
	}

#ifndef _UNICODE
	size_t len = wcslen(p);


	char *s = new char[len + 1];
	memset(s, 0, len + 1);

	if (n == 0)
		WideCharToMultiByte(CP_ACP, NULL, p, -1, s, len, NULL, NULL);
	else
	{
		size_t t = n;
		if (t > len)
			t = len;

		WideCharToMultiByte(CP_ACP, NULL, p, (int)t, s, len, NULL, NULL);
	}

	if (!appendmode)
		content.assign(s);
	else
	{
		content.append(s);
	}

	delete[] s;
#else
	if (!appendmode)
		content.assign(p);
	else
	{
		if (n == 0)
			content.append(p);
		else
			content.append(p, n);
	}
#endif
}

void gstring::assign_fromutf8(const char *p)
{
	if (p == NULL)
	{
		content.clear();
		return;
	}

	DWORD len = static_cast<DWORD>(strlen(p));
	wchar_t *w = new wchar_t[len + 1];
	memset(w, 0, (len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, NULL, p, -1, w, len);

#ifndef _UNICODE
	char *s = new char[len + 1];
	memset(s, 0, len + 1);
	WideCharToMultiByte(CP_ACP, NULL, w, -1, s, len, NULL, NULL);

	content.assign(s);

	delete[] s;
#else
	content.assign(w);
#endif

	delete[] w;
}

std::string gstring::getutf8() const
{
	std::string::size_type len = content.size();

	if (len == 0)
		return std::string("");

#ifndef _UNICODE
	wchar_t *w = new wchar_t[len + 1];
	memset(w, 0, (len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, NULL, content.c_str(), -1, w, len);
#else
	const wchar_t *w = content.c_str();
#endif
	
	size_t utflen = (len + 1) * 4;
	char *u = new char[utflen];
	memset(u, 0, utflen);
	WideCharToMultiByte(CP_UTF8, NULL, w, -1, u, static_cast<int>(utflen), NULL, NULL);

	std::string retval(u);

	delete[] u;

#ifndef _UNICODE
	delete[] w;
#endif

	return retval;
}

std::wstring gstring::getw() const
{
	std::string::size_type len = content.size();

	if (len == 0)
		return std::wstring(L"");

#ifndef _UNICODE
	wchar_t *w = new wchar_t[len + 1];
	memset(w, 0, (len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, NULL, content.c_str(), -1, w, len);
	
	std::wstring retval(w);

	delete[] w;
	return retval;
#else
	return content;
#endif
}

std::string gstring::getmb() const
{
#ifndef _UNICODE
	return content;
#else

	size_t len = content.size();
	char *u = new char[(len * 2) + 1];
	memset(u, 0, (len * 2) + 1);
	WideCharToMultiByte(CP_ACP, NULL, content.c_str(), -1, u, static_cast<int>(len * 2), NULL, NULL);

	std::string retval(u);

	delete[] u;

	return retval;
#endif
}

#endif

#ifdef UTM_WIN
#ifndef _UNICODE
std::string::size_type gstring::find(const char* p, size_t pos) const
{
	return content.find(p, pos);
}
#else
std::string::size_type gstring::find(const char* p, size_t pos) const
{
	std::string str = getmb();
	return str.find(p, pos);
}
#endif
#else
std::string::size_type gstring::find(const char* p, size_t pos) const
{
	return content.find(p, pos);
}
#endif

}