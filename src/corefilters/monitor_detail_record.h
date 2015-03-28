#ifndef _UTM_MONITOR_DETAIL_RECORD_H
#define _UTM_MONITOR_DETAIL_RECORD_H

#pragma once

#include "monitor_detail_record_base.h"

namespace utm {

class monitor_detail_record : public monitor_detail_record_base
{
public:
	monitor_detail_record(void);
	~monitor_detail_record(void);

	void set_id(unsigned int id) { this->id = id; };
	unsigned int get_id() const { return id; };
	const gstring& get_name() const { return comment; };
};

}

#endif // _UTM_MONITOR_DETAIL_RECORD_H
