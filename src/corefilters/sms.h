#ifndef _UTM_SMS_H
#define _UTM_SMS_H

#pragma once
#include <utm.h>

#ifdef UTM_WIN
#include <Windows.h>
#include <tchar.h>
#endif

#include "sms_base.h"

#include <boost/random.hpp>

namespace utm {

class sms : public sms_base
{
public:
	sms(void);
	~sms(void);

	static boost::uniform_int<int> rnd;
	static boost::random::mt19937 eng;

	void generate_msg_id();
};

}

#endif // _UTM_SMS_H
