#ifndef _UTM_FILTERAGENT_BASE_H
#define _UTM_FILTERAGENT_BASE_H

#pragma once
#include <utm.h>

#include <filteragent_base.h>
#include <filter2.h>

namespace utm {

class filteragent : public filteragent_base
{
public:
	filteragent();
	~filteragent();

	void create(unsigned int total_points, unsigned int next_point, unsigned int array_elements_count, const filter2& f, std::string& xml);
};

}

#endif // _UTM_FILTERAGENT_BASE_H
