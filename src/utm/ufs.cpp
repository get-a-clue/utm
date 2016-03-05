#include "stdafx.h"
#include "ufs.h"

#include <boost/test/unit_test.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

namespace utm {

const char ufs::this_class_name[] = "ufs";

#ifdef UTM_WIN
const gstring ufs::slash = gstring("\\");
#else
const gstring ufs::slash = gstring("/");
#endif

ufs::ufs(void)
{
}


ufs::~ufs(void)
{
}

bool ufs::is_slash(const gchar& c)
{
	if ((c == SLASH1) || (c == SLASH2))
	{
		return true;
	}

	return false;
}

bool ufs::create_nested_directories_for_filename(const gstring& filename)
{
	boost::filesystem::path path(filename.get());
	boost::filesystem::path dir = path.parent_path();

	bool retval = false;
	try
	{
		if (!boost::filesystem::is_directory(dir))
		{
			boost::filesystem::create_directories(dir);
		}
		retval = true;
	}
	catch (const boost::filesystem::filesystem_error&)
	{
	}
	return retval;
}

bool ufs::derive_fullpath(const utime& timestamp, const gstring& filename, gstring& retval)
{
	if (filename.empty())
		return false;

	utime utm;

	if (!timestamp.is_valid())
		utm.now();
	else
		utm = timestamp;

	retval = filename;

	gchar c;

	size_t pos = 0;
	while(true)
	{
		pos = retval.find(FILENAME_PATTERN_PREFIX, pos);
		if (pos == std::string::npos)
			break;

		pos++;

		retval.at(pos, c);
		if (c == 0)
			break;

		gstring str;

		switch(c)
		{
			case FILENAME_PATTERN_YEAR:
				utm.get_year(str);
				retval.replace(pos - 1, 2, str.c_str(), 4);
				break;

			case FILENAME_PATTERN_YEAR_SHORT:
				utm.get_year(str);
				retval.replace(pos - 1, 2, str.c_str() + 2, 2);
				break;

			case FILENAME_PATTERN_MONTH_LONG:
				utm.get_month_long(str);
				retval.replace(pos - 1, 2, str.c_str(), str.size());
				break;

			case FILENAME_PATTERN_MONTH_SHORT:
				utm.get_month_short(str);
				retval.replace(pos - 1, 2, str.c_str(), str.size());
				break;

			case FILENAME_PATTERN_MONTH:
				utm.get_month(str);
				retval.replace(pos - 1, 2, str.c_str(), 2);
				break;

			case FILENAME_PATTERN_DAY:
				utm.get_day(str);
				retval.replace(pos - 1, 2, str.c_str(), 2);
				break;
		}
	}

	return true;
}

void ufs::make_strong_directory_path(const gstring& path, gstring& retval)
{
	retval = path;
	if (retval.empty())
		return;

	gchar c;
	retval.at(retval.size() - 1, c);

	if (!ufs::is_slash(c))
	{
		retval.append(ufs::get_slash());
	}
}

gstring ufs::get_current_directory()
{
	gstring current_directory;

#ifdef UTM_WIN
	TCHAR szWorkingDir[MAX_PATH];
	szWorkingDir[MAX_PATH - 1] = 0;
	DWORD dwRes = ::GetModuleFileName(NULL, szWorkingDir, MAX_PATH - 1);
	if (0 != dwRes)
	{
		while ((dwRes > 0) && (szWorkingDir[dwRes] != _T('\\')))
		{
			szWorkingDir[dwRes] = 0;
			dwRes--;
		};
		current_directory.assign(szWorkingDir);
	}
#endif

	return current_directory;
}

gstring ufs::pathname_from_current_directory(const char* subdir)
{
	gstring gstr(subdir);
	return pathname_from_current_directory(gstr);
}

gstring ufs::pathname_from_current_directory(const gstring& subdir)
{
	gstring retval = get_current_directory();
	retval.append(subdir);
	retval.append("\\");
	return retval;
}

gstring ufs::replace_file_extension(const gstring& filename, const gstring& newext)
{
	size_t pos = filename.get().rfind('.');
	if (pos == gstring::get_npos())
		return filename;

	gstring retval = filename.get().substr(0, pos + 1);
	retval << newext;

	return retval;
}

gstring ufs::make_full_filepath(const gstring& folder, const gstring& filename)
{
	utm::gstring retval;

	size_t folder_size = folder.size();
	if (folder_size > 0)
	{
		retval.assign(folder);
		gchar c;
		retval.at(folder_size - 1, c);

		if (!ufs::is_slash(c))
		{
			retval.append(ufs::get_slash());
		}
	}
	retval << filename;

	return retval;
}

gstring ufs::make_full_filepath(const gstring& folder, const gstring& filename, const gstring& newext)
{
	return make_full_filepath(folder, replace_file_extension(filename, newext));
}

BOOST_AUTO_TEST_CASE(ufs_test_all)
{
	

	gstring str1 = ufs::replace_file_extension("default.tmf", "firewall");
	BOOST_REQUIRE_EQUAL(str1, gstring("default.firewall"));

	gstring str2 = ufs::make_full_filepath(gstring("c:\\tmp\\"), gstring("test.txt"));
	BOOST_REQUIRE_EQUAL(str2, gstring("c:\\tmp\\test.txt"));

	gstring str3 = ufs::make_full_filepath(gstring("c:\\some path\\tmp\\"), gstring("test.txt"), gstring("exe"));
	BOOST_REQUIRE_EQUAL(str3, gstring("c:\\some path\\tmp\\test.exe"));

	gstring str4 = ufs::make_full_filepath(gstring("c:\\tmp"), gstring("test4.txt"));
	BOOST_REQUIRE_EQUAL(str4, gstring("c:\\tmp\\test4.txt"));

	gstring& currentdir = ufs::get_current_directory();
	BOOST_CHECK(currentdir != gstring(""));

	utime u1;
	u1.set(2012, 12, 31, 23, 59, 30);
	gstring temp1("program files common %Y_%m_%d %Y-%m-%d %y*%T %y*%M");
	gstring res1;
	ufs::derive_fullpath(u1, temp1, res1);
	BOOST_REQUIRE_EQUAL(res1, gstring("program files common 2012_12_31 2012-12-31 12*December 12*Dec"));
}

}