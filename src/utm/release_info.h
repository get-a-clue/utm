#ifndef _UTM_RELEASE_INFO_H
#define _UTM_RELEASE_INFO_H

#pragma once
#include <utm.h>

#include <string>

namespace utm {

class release_info
{
public:
	release_info();
	release_info(unsigned short _major, unsigned short _minor, unsigned short _build);
	~release_info();

	bool operator==(const release_info& rhs) const;

	unsigned short major;
	unsigned short minor;
	unsigned short build;
	unsigned short update;   // not used, but required for memory align

	std::string to_string() const;
	void from_string(const char *p);
};

}

#endif // _UTM_RELEASE_INFO_H