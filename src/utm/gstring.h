#ifndef _UTM_GSTRING_H
#define _UTM_GSTRING_H

#pragma once
#include <utm.h>

#include <string>

namespace utm {

#ifndef _U
#ifdef UTM_WIN
#define _U(x) _T(x)
#else
#define _U(x) x
#endif
#endif

#ifndef GCHAR
#define GCHAR
#ifndef UTM_WIN
typedef char gchar;
typedef std::string ugstring;
#else
#ifndef UNICODE
typedef char gchar;
typedef std::string ugstring;
#else
typedef wchar_t gchar;
typedef std::wstring ugstring;
#endif
#endif
#endif

class gstring
{
public:
	gstring();
	gstring(const gstring& s);
	gstring(const std::string& s);
	gstring(const char* s);

#ifdef UTM_WIN
	gstring(const std::wstring& s);
	gstring(const WCHAR* s);
#endif

	~gstring();

	gstring& operator=(const char* p) { assign(p); return *this; };

	bool operator <(const gstring& rhs) const;
	bool operator ==(const gstring& rhs) const;
	bool operator !=(const gstring& rhs) const;
	bool is_ci_equal(const gstring& rhs) const;
	int gstrncmp(const gstring& rhs, size_t size) const;
	int gstrncmp(const gchar* rhs, size_t size) const;
	int gstrcmp(const gchar* rhs) const;

	void assign(const char *p, bool appendmode = false, size_t n = 0);
	void assign(const gstring& s);
	void assign(const std::string& s);

	void append(const char *p, size_t n = 0);
	void append(const gstring& g);
	void append(int r);

	gstring& operator <<(const std::string& s) { append(s); return *this; };
	gstring& operator <<(const char *p) { append(p); return *this; };
	gstring& operator <<(const gstring& g) { append(g); return *this; };
	gstring& operator <<(int r) { append(r); return *this; };

	friend std::ostream&  operator<<(std::ostream& s, const gstring& rhs) { s << rhs.getutf8(); return s; };

#ifdef UTM_WIN
	void assign(const WCHAR *p, bool appendmode = false, size_t n = 0);
	void assign_fromutf8(const char *p);
	void append(const WCHAR *p, size_t n = 0);

	std::string getutf8() const;
	std::wstring getw() const;
	std::string getmb() const;
#endif

#ifdef UTM_WIN
#ifndef _UNICODE
	const std::string& get() const { return content; };
	const char* c_str() const { return content.c_str(); };
	static size_t get_npos() { return std::string::npos; };
#else
	const std::wstring& get() const { return content; };
	const wchar_t* c_str() const { return content.c_str(); };
	static size_t get_npos() { return std::wstring::npos; };
#endif
#else
	const std::string& get() const { return content; };
	const char* c_str() const { return content.c_str(); };
	static size_t get_npos() { return std::string::npos; };
#endif

	size_t size() const { return content.size(); };
	void reserve(size_t capacity) { content.reserve(capacity); };
	bool empty() const { return content.empty(); };
	void clear() { content.clear(); };
	
	std::string::size_type find(const char* p, size_t pos) const;
#ifdef UTM_WIN
#ifdef _UNICODE
	std::wstring::size_type find(const wchar_t* p, size_t pos) const { return content.find(p, pos); };
#endif
#endif

	template<typename charT>
	static std::string to_string(const charT* p)
	{
		std::string retval;
		if (p == NULL) return retval;

#ifdef UTM_WIN
#ifdef _UNICODE
		gstring tmp(p);
		retval = tmp.getmb();
#else
		retval.assign(p);
#endif
#else
		retval.assign(p);
#endif

		return retval;
	}

	template<typename charT>
	void replace(size_t pos, size_t len, const charT* p, size_t n)
	{
		content.replace(pos, len, p, n);
	}

	template<typename charT>
	void at(size_t pos, charT& c)
	{
		if (size() > pos)
			c = content.at(pos);
		else
			c = 0;
	}

	void at(size_t pos, int& c)
	{
		if (size() > pos)
			c = static_cast<int>(content.at(pos));
		else
			c = 0;
	}

#ifndef UTM_WIN
	static std::string convert(const std::string& source_str, const std::string& source_charset, bool lowercase)
	{
		return source_str;
	}
#else
	static std::wstring convert(const std::string& source_str, const std::string& source_charset, bool lowercase)
	{
		gstring gs(source_str);
		return gs.getw();
	}
#endif

private:

#ifdef UTM_WIN
#ifndef _UNICODE
	std::string content;
#else
	std::wstring content;
#endif
#else
	std::string content;
#endif

};

}

#endif // _UTM_GSTRING_H