#ifndef _UTM_IDSET_H
#define _UTM_IDSET_H

#pragma once
#include <utm.h>

#include <string>
#include <set>
#include <list>
#include <vector>
#include <algorithm>

#include <stringtools.h>
#include <ubase_test.h>

#include <boost/lexical_cast.hpp>

typedef std::set<unsigned int> idset_container;
typedef std::list<unsigned int> idlist_container;
typedef std::vector<unsigned int> idvector_container;

namespace utm {

template<class T>
class idset_template
{
public:
	idset_template(void) {};
	~idset_template(void) {};

	T ids;

	bool operator==(const idset_template<T>& rhs) const
	{
		return ids == rhs.ids;
	}

	void clear()
	{
		ids.clear();
	}

	bool empty() const
	{
		return ids.empty();
	}

	size_t size() const
	{
		return ids.size();
	}

	virtual void add_id(unsigned int id) = 0;
	virtual void remove_id(unsigned int id) = 0;

	bool check_id(unsigned int id) const
	{
		T::const_iterator iter = ids.find(id);
		return iter != ids.end();
	}

	std::string to_string() const
	{
		std::string retval;
		retval.reserve(ids.size() * 5);

		for (T::const_iterator iter = ids.begin(); iter != ids.end(); ++iter)
		{
			if (!retval.empty())
				retval.append(" ");

			retval.append(boost::lexical_cast<std::string>(*iter));
		}

		return retval;
	}

	void from_string(const char *p)
	{
		clear();
		std::vector<std::string> tmp_ids;
		stringtools::split(tmp_ids, p, ' ');
		for (std::vector<std::string>::iterator iter = tmp_ids.begin(); iter != tmp_ids.end(); ++iter)
		{
			try
			{
				add_id(boost::lexical_cast<unsigned int>(*iter));
			}
			catch (boost::bad_lexical_cast&)
			{
			}
		}
	}

#ifdef UTM_DEBUG
	void test_all_base()
	{
		for (unsigned int i = 0; i < 11; i++)
		{
			add_id(i);
		}

		std::string str1 = to_string();
		TEST_CASE_CHECK(str1, std::string("0 1 2 3 4 5 6 7 8 9 10"));
		from_string(str1.c_str());
		TEST_CASE_CHECK(str1, std::string("0 1 2 3 4 5 6 7 8 9 10"));

		remove_id(3);
		remove_id(5);
		remove_id(11);
		std::string str2 = to_string();
		TEST_CASE_CHECK(str2, std::string("0 1 2 4 6 7 8 9 10"));
	}
#endif
};

class idset : public idset_template<idset_container>
{
public:
	void add_id(unsigned int id)
	{
		ids.insert(id);
	}

	void remove_id(unsigned int id)
	{
		auto iter = ids.find(id);
		if (iter != ids.end())
		{
			ids.erase(iter);
		}
	}

#ifdef UTM_DEBUG
	static void test_all()
	{
		test_report("idset");
		idset id1;
		id1.test_all_base();
	}
#endif
};

class idlist : public idset_template<idlist_container>
{
public:
	void add_id(unsigned int id)
	{
		ids.push_back(id);
	}

	void remove_id(unsigned int id)
	{
		for (auto iter = ids.begin(); iter != ids.end(); ++iter)
		{
			if (id == *iter)
			{
				ids.erase(iter);
				break;
			}
		}
	}

#ifdef UTM_DEBUG
	static void test_all()
	{
		test_report("idlist");
		idset id1;
		id1.test_all_base();
	}
#endif
};

class idgeneric
{
public:
	idgeneric() {};
	~idgeneric() {};

	template<typename T>
	static std::string list2string(const std::list<T>& items, const std::string& delim)
	{
		std::string retval;
		for (std::list<T>::const_iterator iter = items.begin(); iter != items.end(); ++iter)
		{
			if (!retval.empty()) retval.append(delim);
			std::ostringstream oss;
			oss << *iter;
			retval.append(oss.str());
		}
		return retval;
	}

#ifdef UTM_DEBUG
	static void test_all()
	{
		test_report("idgeneric");
		std::list<int> arr;
		arr.push_back(101);
		arr.push_back(102);
		arr.push_back(103);
		std::string str = idgeneric::list2string(arr, std::string(", "));
		TEST_CASE_CHECK(str, std::string("101, 102, 103"))
	}
#endif
};

}

#endif // _UTM_IDSET_H