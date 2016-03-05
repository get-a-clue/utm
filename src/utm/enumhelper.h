#ifndef _UTM_ENUMHELPER_H
#define _UTM_ENUMHELPER_H

#pragma once
#include <utm.h>

#include <ubase_test.h>

namespace utm {

#define MAX_TESTSTATES_ITEMS 3

class enumhelper
{
public:
	static const char this_class_name[];

	enumhelper(void);
	virtual ~enumhelper(void);

	static const char* to_str(int cat, const char** enum_str, const int* enum_int, int maxitems);
	static int from_str(const char* str, const char** enum_str, const int* enum_int, int maxitems, int default_item = 0);
	static int from_str_ci(const char* str, const char** enum_str, const int* enum_int, int maxitems, int default_item = 0);

#ifdef UTM_DEBUG
	enum teststates
	{
		error = 0,
		info = 1,
		debug = 2
	};
	static const char* teststates_str[];
	static const int teststates_int[];

	
#endif
};

}

#endif // _UTM_ENUMHELPER_H
