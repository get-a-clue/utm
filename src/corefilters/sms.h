#ifndef _UTM_SMS_H
#define _UTM_SMS_H

#pragma once
#include <utm.h>

#ifdef UTM_WIN
#include <Windows.h>
#include <tchar.h>
#endif

#include "sms_base.h"

namespace utm {

class sms : public sms_base
{
public:
	static const char this_class_name[];

public:
	sms(void);
	~sms(void);

	const char *get_this_class_name() const { return this_class_name; };

	void generate_msg_id();
};

}

#endif // _UTM_SMS_H
