#ifndef _UTM_CRC32_H
#define _UTM_CRC32_H

#pragma once
#include <utm.h>

namespace utm {

class crc32
{
public:
	static const char this_class_name[];

	crc32(void);
	~crc32(void);

	static unsigned int calc(const char *text);

#ifdef _DEBUG
	static void test_all();
#endif
};

}

#endif // _UTM_CRC32_H