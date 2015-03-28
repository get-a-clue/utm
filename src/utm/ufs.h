#ifndef _UTM_UFS_H
#define _UTM_UFS_H

#pragma once
#include <utm.h>

#include <gstring.h>
#include <utime.h>

#define FILENAME_PATTERN_PREFIX _U("%")
#define FILENAME_PATTERN_YEAR _U('Y')
#define FILENAME_PATTERN_YEAR_SHORT _U('y')
#define FILENAME_PATTERN_MONTH_LONG _U('T')
#define FILENAME_PATTERN_MONTH_SHORT _U('M')
#define FILENAME_PATTERN_MONTH _U('m')
#define FILENAME_PATTERN_DAY _U('d')

#define SLASH1 _U('/')
#define SLASH2 _U('\\')

namespace utm {

class ufs
{
public:
	static const char this_class_name[];

	ufs(void);
	~ufs(void);

	static bool create_nested_directories_for_filename(const gstring& filename);
	static bool derive_fullpath(const utime& timestamp, const gstring& filename, gstring& retval);
	static void make_strong_directory_path(const gstring& path, gstring& retval);

	static gstring get_current_directory();

	static gstring pathname_from_current_directory(const char* subdir);
	static gstring pathname_from_current_directory(const gstring& subdir);
	static gstring replace_file_extension(const gstring& filename, const gstring& newext);

	static gstring make_full_filepath(const gstring& folder, const gstring& filename);
	static gstring make_full_filepath(const gstring& folder, const gstring& filename, const gstring& newext);

	static const gstring& get_slash() { return slash; };
	static const gstring slash;

	static bool is_slash(const gchar& c);

public:
#ifdef UTM_DEBUG
	static void test_all();
#endif
};

}

#endif // _UTM_UFS_H