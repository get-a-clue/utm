#include "StdAfx.h"
#include "macvendors.h"

#include <iostream>
#include <fstream>

#include <uhex.h>
#include <addrmac.h>
#include <ubase_test.h>

namespace utm {

const char macvendors::this_class_name[] = "macvendors";
const char macvendors::no_vendor_found[] = "";

macvendors::macvendors(void)
{
}

macvendors::~macvendors(void)
{
}

void macvendors::load_from_file(const gstring& filepath)
{
	std::list<std::string> stringarray;
	std::string str;
	size_t pos;

	std::ifstream f(filepath.c_str(), std::ios_base::in);
	if (f) 
	{
		while (!f.eof()) 
		{
			getline(f, str);
			pos = str.find("(base 16)");
			if (pos != std::string::npos)
			{
				stringarray.push_back(str);
			}
		}
		f.close();
	}

	parse_from_array(stringarray);
}

void macvendors::parse_from_array(const std::list<std::string>& stringarray)
{
	boost::unique_lock<boost::shared_mutex> lockObj(lock);
	vendors.clear();

	const char *p, *s;
	std::string str, vendor, trimmed_vendor;
	unsigned char machead[3];
	unsigned int m, mackey;
	size_t beginpos, endpos;

	for (std::list<std::string>::const_iterator iter = stringarray.begin(); iter != stringarray.end(); ++iter)
	{
		str = *iter;

		if (str.size () < 10)
			continue;

		p = strstr(str.c_str(), "(base 16)");
		if (p != NULL)
		{
			s = str.c_str();
			while (*s == ' ') s++;

			uhex::parse_hex(s, 6, machead);
			m = machead[0];	mackey = m << 24;
			m = machead[1];	mackey += m << 16;
			m = machead[2];	mackey += m << 8;

			p = p + sizeof("(base 16)") + 1;
			vendor.assign(p);

			beginpos = vendor.find(' ');
			if (beginpos != std::string::npos)
				trimmed_vendor = vendor.substr(beginpos);
	
			endpos = vendor.find_last_not_of(" \t\r\n");
			if (endpos != std::string::npos)
				trimmed_vendor = vendor.substr(0, endpos + 1);

			if (!trimmed_vendor.empty())
				vendors.insert(std::make_pair(mackey, trimmed_vendor));
		}
	}
}

const char* macvendors::find_vendor(const unsigned char* mac) const
{
	unsigned int m, mackey;
	m = (unsigned int)(*mac); mackey = m << 24;
	m = (unsigned int)(*(mac + 1)); mackey += m << 16;
	m = (unsigned int)(*(mac + 2)); mackey += m << 8;

	if (mackey == 0)
		return no_vendor_found;

	macvendors_container::const_iterator iter;
	{
		boost::shared_lock<boost::shared_mutex> lockObj(lock);
		iter = vendors.find(mackey);

		if (iter == vendors.end())
			return no_vendor_found;
	}

	return iter->second.c_str();
}

size_t macvendors::size() const
{
	boost::shared_lock<boost::shared_mutex> lockObj(lock);
	return vendors.size();
}

void macvendors::test_all()
{
	test_case::classname.assign(this_class_name);

	std::list<std::string> testdata;
	testdata.push_back("  ");
	testdata.push_back("  OUI				Organization");
	testdata.push_back("  company_id			Organization");
	testdata.push_back("  00-00-01   (hex)		XEROX CORPORATION");
	testdata.push_back("  000001     (base 16)		XEROX CORPORATION");
	testdata.push_back("  				ZEROX SYSTEMS INSTITUTE");
	testdata.push_back("				M/S 105-50C 800 PHILLIPS ROAD");
	testdata.push_back("				WEBSTER NY 14580");
	testdata.push_back("				UNITED STATES");
	testdata.push_back("  F0-DC-E2   (hex)		Apple Inc");
	testdata.push_back("  F0DCE2     (base 16)		Apple Inc");
	testdata.push_back("  				1 Infinite Loop");
	testdata.push_back("				Cupertino CA 95014");
	testdata.push_back("				UNITED STATES");

	macvendors mv;
	mv.parse_from_array(testdata);

	TEST_CASE_CHECK(mv.size(), size_t(2));

	{
		addrmac m1("00-00-01-02-03-04");
		std::string v1(mv.find_vendor(m1.get()));
		TEST_CASE_CHECK(v1, std::string("XEROX CORPORATION"));
	}

	{
		addrmac m1("00-00-02-02-03-04");
		std::string v1(mv.find_vendor(m1.get()));
		TEST_CASE_CHECK(v1, std::string(""));
	}

	{
		addrmac m1("F0-DC-E2-02-03-04");
		std::string v1(mv.find_vendor(m1.get()));
		TEST_CASE_CHECK(v1, std::string("Apple Inc"));
	}

	return;
}

}