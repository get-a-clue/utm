#ifndef _UTM_STRINGTOOLS_H
#define _UTM_STRINGTOOLS_H

#pragma once
#include <utm.h>

#include <gstring.h>
#include <ubase_test.h>

#include <locale>
#include <algorithm>
#include <list>
#include <string>

#define LSTR_BEGIN _U("<LSTR>")
#define LSTR_BEGIN_SIZE 6
#define LSTR_END _U("</LSTR>")
#define LSTR_END_SIZE 7
#define STRLSTR(x) stringtools::strLSTR(_T(x))

namespace utm {

typedef const gchar* (__stdcall *LocalizationFuncPtr)(const gchar* str);

struct split
{
  enum empties_t { empties_ok, no_empties };
};

class stringtools {
public:
	static const char this_class_name[];

private:
	stringtools() { };
	~stringtools() { };

public:
	template <typename Container>
	static void split(
		Container& result,
		const std::string& s,
		const char delimiter,
		split::empties_t empties = split::empties_ok)
	{
		result.clear();
		std::istringstream iss(s);
		while (!iss.eof())
		{
			typename Container::value_type field;
			getline(iss, field, delimiter);
		    if ((empties == split::no_empties) && field.empty()) continue;
			result.push_back( field );
		}
	}

#ifdef UTM_WIN
	template <typename Container>
	static void split(
		Container& result,
		const std::wstring& s,
		const wchar_t delimiter,
		split::empties_t empties = split::empties_ok)
	{
		result.clear();
		std::wistringstream iss(s);
		while (!iss.eof())
		{
			typename Container::value_type field;
			getline(iss, field, delimiter);
		    if ((empties == split::no_empties) && field.empty()) continue;
			result.push_back( field );
		}
	}
#endif

	template <typename Container>
	static void split(
		Container& result,
		const char* s,
		const char delimiter,
		split::empties_t empties = split::empties_ok)
	{
		result.clear();
		std::istringstream iss(s);
		while (!iss.eof())
		{
			typename Container::value_type field;
			getline(iss, field, delimiter);
		    if ((empties == split::no_empties) && field.empty()) continue;
			result.push_back( field );
		}
	}

#ifdef UTM_WIN
	template <typename Container>
	static void split(
		Container& result,
		const wchar_t* s,
		const wchar_t delimiter,
		split::empties_t empties = split::empties_ok)
	{
		result.clear();
		std::wistringstream iss(s);
		while (!iss.eof())
		{
			typename Container::value_type field;
			getline(iss, field, delimiter);
		    if ((empties == split::no_empties) && field.empty()) continue;
			result.push_back( field );
		}
	}
#endif

	template <typename Container>
	static void join(
		const Container& src,
		const char delimiter,
		std::string& result)
	{
		std::ostringstream oss;

		for (Container::const_iterator iter = src.begin(); iter != src.end(); ++iter)
		{
			if (iter != src.begin())
				oss << delimiter;

			oss << *iter;
		}

		result = oss.str();
	}

	template <typename Container>
	static void join(
		const Container& src,
		const char* delimiter,
		std::string& result)
	{
		std::ostringstream oss;

		for (Container::const_iterator iter = src.begin(); iter != src.end(); ++iter)
		{
			if (iter != src.begin())
				oss << delimiter;

			oss << *iter;
		}

		result = oss.str();
	}

	// input string:  <RootTag><ChildTag>Value</ChildTag></RootTag>
	// output string: <ChildTag>Value</ChildTag>
	static void remove_root_tag(const std::string& str_in, std::string& str_out)
	{
		size_t s1 = str_in.find('<');
		size_t s2 = str_in.find('>');
		if ((s1 == std::string::npos || s2 == std::string::npos) || (s1 > s2)) { str_out = str_in; return; }

		size_t e1 = str_in.rfind('>');
		size_t e2 = str_in.rfind('<');

		if ((e1 < e2) || (s2 > e2)) { str_out = str_in; return; }

		const char *open_tag = str_in.c_str() + s1 + 1;
		const char *close_tag = str_in.c_str() + e2 + 2;
		if (open_tag == close_tag) { str_out = str_in; return; }

		size_t tag_len = s2 - s1 - 1;
		if ((e2 - e1 - 2) < tag_len) { str_out = str_in; return; }
		if (strncmp(open_tag, close_tag, tag_len) != 0) { str_out = str_in; return; }

		str_out = str_in.substr(s2 + 1, e2 - (s2 + 1));
	}

	template<typename charT>
	struct my_equal {
	    my_equal( const std::locale& loc ) : loc_(loc) {}
		bool operator()(charT ch1, charT ch2) {
	        return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
		}
		private:
			const std::locale& loc_;
	};

	// find substring (case insensitive)
	template<typename T>
	static size_t ci_find_substr( const T& str1, const T& str2, const std::locale& loc = std::locale() )
	{
		T::const_iterator it = std::search( str1.begin(), str1.end(), 
	        str2.begin(), str2.end(), my_equal<T::value_type>(loc) );
		if ( it != str1.end() ) return it - str1.begin();
		else return std::string::npos; // not found
	}

	static void LocalizeMessage(const gstring& srcmsg, gstring& dstmsg);
	static const gchar* strLSTR(const gchar* str);
	static LocalizationFuncPtr localize;

#ifdef UTM_DEBUG
	static void test_all()
	{
		std::string tokens = "a1 b2 c3 d4*e5";

		std::list<std::string> mylist;
		split(mylist, tokens.c_str(), ' ', split::no_empties);

		if (mylist.size() != 4)
			throw test_exception(this_class_name, "t1");

		std::string tokens2;
		join(mylist, ' ', tokens2);

		if (tokens != tokens2)
			throw test_exception(this_class_name, "t2");

		LocalizationFuncPtr prev = localize;
		localize = NULL;

		const char *t1 = "This is a test";

		gstring s1;
		LocalizeMessage(gstring(t1), s1);
		if (gstring(t1) != s1)
			throw test_exception(this_class_name, "s1");

		gstring s2;
		LocalizeMessage(gstring("<LSTR>This is a test</LSTR>"), s2);
		if (gstring(t1) != s2)
			throw test_exception(this_class_name, "s2");

		gstring s3;
		LocalizeMessage(gstring("<LSTR>This is a</LSTR> test"), s3);
		if (gstring(t1) != s3)
			throw test_exception(this_class_name, "s3");

		gstring s4;
		LocalizeMessage(gstring("<LSTR>This</LSTR> <LSTR>is</LSTR> <LSTR>a</LSTR> test"), s4);
		if (gstring(t1) != s4)
			throw test_exception(this_class_name, "s4");

		localize = prev;


		std::string r1 = "<RootTag><ChildTag>Value</ChildTag></RootTag>";
		std::string r2 = "<ChildTag>Value</ChildTag>";
		std::string r0;

		remove_root_tag(r1, r0);
		if (r0 != r2)
			throw test_exception(this_class_name, "r0");

		remove_root_tag(r2, r0);
		if (r0 != "Value")
			throw test_exception(this_class_name, "r1");

		std::string r3;
		remove_root_tag(r0, r3);
		if (r3 != "Value")
			throw test_exception(this_class_name, "r2");

		std::list<unsigned int> ids;
		for (unsigned int i = 1; i < 4; i++) ids.push_back(i);

		std::string ids1;
		join(ids, ',', ids1);
		TEST_CASE_CHECK(ids1, std::string("1,2,3"));

		std::string ids2;
		join(ids, ", ", ids2);
		TEST_CASE_CHECK(ids2, std::string("1, 2, 3"));
	}

#endif
};

}

#endif // _UTM_STRINGTOOLS_H
