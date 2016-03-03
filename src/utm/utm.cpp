#include "stdafx.h"
#include "utm.h"

#include "utime.h"

#include <random>
#include <ctime>
#include <iostream>

namespace utm {

const char* get_utmversion()
{
	return UTM_VERSION;
}

int get_random(int min, int max)
{
	std::mt19937 rng;
	std::uniform_int_distribution<> gen(min, max);
	return gen(rng);
}

unsigned short get_pid()
{
#if defined(UTM_WIN)
  return static_cast<unsigned short>(::GetCurrentProcessId());
#else
  return static_cast<unsigned short>(::getpid());
#endif
}

void udebug(const char *msg)
{
	if (msg == NULL) return;
	utime ut(true);
	size_t len = strlen(msg);

	std::string str;
	str.reserve(len + 48);
	str.assign(ut.to_string());
	str.append(" ");
	str.append(msg);

	std::cout << str << std::endl;
}

}