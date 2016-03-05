#ifndef _UTIMESTAMP_H
#define _UTIMESTAMP_H

#pragma once
#include <utm.h>

#include <string>
#include <time.h>

#include <utime.h>

namespace utm {

class utimestamp
{
public:
	static const char this_class_name[];

	utimestamp(void);
	utimestamp(time_t _ts);
	utimestamp(unsigned int _ts);
	utimestamp(const utime& ut);
	~utimestamp(void);

	bool operator==(const utimestamp& rhs) const { return ts == rhs.ts; }
	bool operator<(const utimestamp& rhs) const { return ts < rhs.ts; }

	void clear() { ts = 0; };
	void now() { time(&ts); };

	std::string to_string() const;
	void from_string(const char *t);
	void from_uint(unsigned int _ts);
	unsigned int to_uint() const { return (unsigned int)ts; };

	friend std::ostream& operator<<(std::ostream& s, const utimestamp& rhs) { s << rhs.to_string(); return s; };
	friend std::istream& operator>>(std::istream& s, utimestamp& obj);

	inline bool is_greater_than_by_sec(const utimestamp& arg, unsigned int interval) const
	{
		if (ts < arg.ts) return false;
		unsigned int diff = (unsigned int)(ts - arg.ts);
		return (diff > interval);
	}

	time_t ts;

#ifdef UTM_WIN
	void from_filetime(const FILETIME& ft);
#endif

#ifdef UTM_DEBUG
	
#endif

};

}

#endif // _UTIMESTAMP_H