#ifndef _UTM_UBASE_TEST_H
#define _UTM_UBASE_TEST_H

#pragma once
#include <utm.h>

#include <string>
#include <ostream>
#include <iostream>
#include <sstream>

#define TEST_CASE_CHECK(x,y) utm::test_case::check(true, __FILE__, __LINE__, x, y);
#define TEST_CASE_NOTCHECK(x,y) utm::test_case::notcheck(true, __FILE__, __LINE__, x, y);
#define TEST_CASE_CHECK_NOSERIALIZE(x,y) utm::test_case::check_noserialize(true, __FILE__, __LINE__, x, y);

namespace utm {

class test_exception : public std::exception
{
private:
	test_exception() { };

	std::string reason;

public:
	~test_exception() { };

	test_exception(const char* classname, const char* testtitle, int testcase = 0)
	{
		std::ostringstream oss;
		oss << "TEST FAILED! " << classname << ":: case " << testtitle << ", set " << testcase;

		reason = oss.str();
	}

	test_exception(const char* classname, const std::string& testtitle, int testcase = 0)
	{
		std::ostringstream oss;
		oss << "TEST FAILED! " << classname << ":: case " << testtitle.c_str() << ", set " << testcase;

		reason = oss.str();
	}

	const char* what() const
	{
		return reason.c_str();
	}
};

class test_case
{
public:
	test_case() {};
	~test_case() {};

	static std::string classname;
	static int testcase_num;

	static void check(bool fake, const char *sourcefile, int sourceline, const char* arg1, const char* arg2, int testcase_num = 0)
	{
		if (arg1 == arg2)
			return;

		if (strcmp(arg1, arg2) == 0) 
			return;

		std::ostringstream oss;	oss << arg1 << " != " << arg2 << "  (" << sourcefile << ", line: " << sourceline << ")";
		throw test_exception(classname.c_str(), oss.str(), testcase_num);
	}

	template<typename T>
	static void check(bool fake, const char *sourcefile, int sourceline, const T& arg1, const T& arg2, int testcase_num = 0)
	{
		if (arg1 == arg2) 
			return;

		std::ostringstream oss;	oss << arg1 << " != " << arg2 << "  (" << sourcefile << ", line: " << sourceline << ")";
		throw test_exception(classname.c_str(), oss.str(), testcase_num);
	}

	template<typename T>
	static void notcheck(bool fake, const char *sourcefile, int sourceline, const T& arg1, const T& arg2, int testcase_num = 0)
	{
		if (!(arg1 == arg2)) 
			return;

		std::ostringstream oss;	oss << arg1 << " == " << arg2 << "  (" << sourcefile << ", line: " << sourceline << ")";
		throw test_exception(classname.c_str(), oss.str(), testcase_num);
	}

	template<typename T>
	static void check_noserialize(bool fake, const char *sourcefile, int sourceline, const T& arg1, const T& arg2, int testcase_num = 0)
	{
		if (arg1 == arg2) 
			return;

		std::ostringstream oss;	oss << "operator= failed, no serialization for class (" << sourcefile << ", line: " << sourceline << ")";
		throw test_exception(classname.c_str(), oss.str(), testcase_num);
	}

	template<typename T>
	static void check(const char *classname, const T& arg1, const T& arg2, int testcase_num = 0)
	{
		if (arg1 == arg2)
			return;

		std::ostringstream oss;	oss << arg1 << " != " << arg2;
		throw test_exception(classname, oss.str(), testcase_num);
	}
};

class test_report
{
public:
	test_report(const char* classname)
	{
		testclassname.assign(classname);
		std::cout << "Start unittest for class '" << testclassname << "'  ";

		test_case::classname.assign(classname);
		test_case::testcase_num = 1;
	}

	~test_report()
	{
		std::cout << "...unittest finished OK!" << std::endl;
	}

	std::string testclassname;
};

#define UBASE_TEST_REGISTRY_PATH "Software\\UTM\\UtmLibrary"

template<class T>
class ubase_test 
{
public:
	ubase_test() 
	{ 
		xml_catch_value_callbacks = 0;
	};
	
	~ubase_test() { };

	int xml_catch_value_callbacks;

	void xml_catch_value_callback(const char* key, const char *value)
	{
		xml_catch_value_callbacks++;
	}

	void test_all()
	{
		test_report tr(T::this_class_name);

		{
			std::vector<T> dummy_vector;
			for (int u = 0; u < 10; u++)
			{
				T t1;
				dummy_vector.push_back(t1);
			}
		}

		for (int t = 0; t < T::test_get_testcases_number(); t++)
		{
			test_set(t);
		}
	}

	void test_set(int testcase_num)
	{
		T myclass1;
		myclass1.test_fillparams(testcase_num);
		myclass1.xml_create();
		std::string xml;
		myclass1.xml_get_string(xml);

		{
			T myclass2(myclass1);
			std::string xml2;
			myclass2.xml_get_string(xml2);

			if (!xml2.empty())
			{
				throw test_exception(T::this_class_name, "1", testcase_num);
			}

			T myclass3(myclass1);
			myclass3.xml_create();
			std::string xml3;
			myclass3.xml_get_string(xml3);

			if (xml != xml3)
			{
				throw test_exception(T::this_class_name, "2", testcase_num);
			}

			T myclass4;
			myclass4.xml_catch_value_callback = boost::bind(&ubase_test::xml_catch_value_callback, this, _1, _2);
			myclass4.xml_parse(xml.c_str());
			std::string xml4;
			myclass4.xml_get_string(xml4);

			if (!xml4.empty())
			{
				throw test_exception(T::this_class_name, "3", testcase_num);
			}

			T myclass5;
			myclass5.xml_parse(xml.c_str());
			myclass5.xml_create();
			std::string xml5;
			myclass5.xml_get_string(xml5);

			if (xml != xml5)
			{
				throw test_exception(T::this_class_name, "4", testcase_num);
			}

			if (!(myclass1 == myclass5))
			{
				throw test_exception(T::this_class_name, "5", testcase_num);
			}
		}

		if (myclass1.has_registry())
		{
			myclass1.CreateRegistry(_T(UBASE_TEST_REGISTRY_PATH), HKEY_CURRENT_USER);
			myclass1.SaveToRegistry(_T(UBASE_TEST_REGISTRY_PATH), HKEY_CURRENT_USER);
			T myclass6;
			myclass6.ReadFromRegistry(_T(UBASE_TEST_REGISTRY_PATH), HKEY_CURRENT_USER);
			myclass6.xml_create();
			std::string xml6;
			myclass6.xml_get_string(xml6);

			if (xml != xml6)
			{
				throw test_exception(T::this_class_name, "6 (registry)", testcase_num);
			}

			T myclass7;
			myclass7.SaveToRegistry(_T(UBASE_TEST_REGISTRY_PATH), HKEY_CURRENT_USER);
		}

		std::ostringstream ossfilepath;
		ossfilepath << "c://tmp//" << T::this_class_name << ".xml";
		std::string filepath = ossfilepath.str();

		myclass1.xml_save(filepath);

		{
			T myclass10;
			myclass10.xml_load(filepath);

			if (!(myclass1 == myclass10))
			{
				throw test_exception(T::this_class_name, "10", testcase_num);
			}
		}

		{
			T myclass8;
			myclass8.xml_load(filepath);

			if (!(myclass1 == myclass8))
			{
				throw test_exception(T::this_class_name, "8", testcase_num);
			}
		}

		{
			T myclass9;
			myclass9.ReadFromFileOrRegistry(filepath, NULL);

			if (!(myclass1 == myclass9))
			{
				throw test_exception(T::this_class_name, "9", testcase_num);
			}
		}

//		if (xml_catch_value_callbacks == 0)
//		{
//			throw test_exception(T::this_class_name, "10 (no_xml_catch_value_callback)", testcase_num);
//		}
	}
};

}

#endif // _UTM_UBASE_TEST_H
