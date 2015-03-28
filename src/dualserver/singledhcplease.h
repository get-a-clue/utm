#ifndef _UTM_SINGLEDHCPLEASE_H
#define _UTM_SINGLEDHCPLEASE_H

#pragma once
#include <singledhcplease_base.h>

namespace utm
{

class singledhcplease : public singledhcplease_base
{
public:
	singledhcplease();
	~singledhcplease();

	unsigned int get_id() const;
	void set_id(unsigned int id);
};

}

#endif // _UTM_SINGLEDHCPLEASE_H