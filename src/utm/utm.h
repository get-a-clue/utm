#ifndef _UTM_UTM_H
#define _UTM_UTM_H

#if defined(_MSC_VER)
#define UTM_WIN
#endif

#ifndef UTM_VERSION
#define UTM_VERSION "15.0.106"
#endif

#ifdef UTM_WIN
#include <Windows.h>
#include <tchar.h>
#define MAX_REGISTRY_VALUE 16383
#endif

#ifdef UTM_DEBUG
#define UDEBUG(x) udebug(x)
#else
#define UDEBUG(x) udebug(NULL)
#endif

#include <cstdint>

#define MAXLEN_MSG 256

namespace utm {
	const char* get_utmversion();
	int get_random(int min, int max);
	unsigned short get_pid();
	void udebug(const char *msg);
}

#endif // _UTM_UTM_H