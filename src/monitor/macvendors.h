#ifndef UTM_MONITOR_MACVENDORS_H
#define UTM_MONITOR_MACVENDORS_H

#pragma once
#include <utm.h>

#include <gstring.h>

#include <map>
#include <list>
#include <string>

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread.hpp>


namespace utm {

typedef std::map<unsigned int, std::string> macvendors_container;

class macvendors
{
public:
	static const char this_class_name[];
	static const char no_vendor_found[];

	macvendors(void);
	~macvendors(void);

private:
	mutable boost::shared_mutex lock;
	macvendors_container vendors;

public:
	void load_from_file(const gstring& filepath);
	void parse_from_array(const std::list<std::string>& stringarray);
	const char* find_vendor(const unsigned char* mac) const;
	size_t size() const;

	static void test_all();
};

}

#endif // UTM_MONITOR_MACVENDORS_H
