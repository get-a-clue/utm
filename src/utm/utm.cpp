#include "stdafx.h"
#include "utm.h"

#include "utime.h"

#include <boost/random.hpp>
#include <ctime>  

namespace utm {

const char* get_utmversion()
{
	return UTM_VERSION;
}

int get_random(int min, int max)
{
    static boost::minstd_rand gen((unsigned int)std::time(NULL));
    boost::uniform_int<int> dist(min, max);
    boost::variate_generator<
        boost::minstd_rand&,
        boost::uniform_int<int>> combgen(gen, dist);

    return combgen();
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