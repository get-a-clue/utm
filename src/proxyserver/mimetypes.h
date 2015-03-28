#ifndef _UTM_MIMETYPES_H
#define _UTM_MIMETYPES_H

#pragma once
#include <utm.h>

#include <string>
#include <map>

namespace utm {

typedef std::map<std::string, std::string> mimetypes_map;

class mimetypes
{
public:
	mimetypes();
	~mimetypes();

	void init();
	void readfromfile(const std::string& configpath);
	const char* get_by_extension(const std::string& extension) const;
	const char* get_by_path(const std::string& path) const;

protected:
	mimetypes_map mtypes;
};

}

#endif
