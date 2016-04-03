#ifndef _UTM_FSUSER_H
#define _UTM_FSUSER_H

#pragma once
#include <utm.h>

#include <utime.h>
#include <fsuser_base.h>

namespace utm {

class fsuser : public fsuser_base
{
public:
	static const char this_class_name[];

public:
	fsuser(void);
	fsuser(const gstring& _username);
	~fsuser(void);

	const char *get_this_class_name() const { return this_class_name; };

	bool is_user_expired(const utime& now) const;
	bool is_user_enabled(const utime& now) const;

	void set_id(unsigned int id) { m_uid = id; };
	unsigned int get_id() const { return m_uid; };
	const gstring& get_name() const { return m_szName; };
};

}

#endif // _UTM_FSUSER_H
