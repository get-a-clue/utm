#include "stdafx.h"
#include "release_info.h"

namespace utm {

release_info::release_info() : major(0), minor(0), build(0), update(0)
{
}

release_info::release_info(unsigned short _major, unsigned short _minor, unsigned short _build) :
	major(_major), minor(_minor), build(_build), update(0)
{
}

release_info::~release_info()
{
}

bool release_info::operator==(const release_info& rhs) const
{
	if (major != rhs.major) return false;
	if (minor != rhs.minor) return false;
	if (build != rhs.build) return false;
	return (update == rhs.update);
}

std::string release_info::to_string() const
{
	char sz[64];
#ifdef _MSC_VER
	sprintf_s(sz, 64, "%u.%u.%u", major, minor, build);
#else
	sprintf(sz, 64, "%u.%u.%u", major, minor, build);
#endif

	return std::string(sz);
}

void release_info::from_string(const char *p)
{
	unsigned int a1 = 0, a2 = 0, a3 = 0;
#ifdef _MSC_VER
	sscanf_s(p, "%u.%u.%u", &a1, &a2, &a3);
#else
	sscanf(p, "%u.%u.%u", &a1, &a2, &a3);
#endif

	major = static_cast<unsigned short>(a1);
	minor = static_cast<unsigned short>(a2);
	build = static_cast<unsigned short>(a3);
}

}