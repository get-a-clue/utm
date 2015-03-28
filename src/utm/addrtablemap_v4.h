#ifndef _UTM_ADDRTABLEMAP_V4_H
#define _UTM_ADDRTABLEMAP_V4_H

#pragma once
#include <utm.h>

#include <addrtable_v4.h>
#include <addrtablemap.h>

namespace utm {

class addrtablemap_v4 : public utm::addrtablemaplist<utm::addrtable_v4> 
{
public:
	addrtablemap_v4() { };
	virtual ~addrtablemap_v4() { };

    void AddAddrPair(unsigned int key_id, const addrip_v4& start, const addrip_v4& end)
	{
		addrtablemaprec_v4 t;
		if (this->find_by_id(key_id, t))
		{
			t.addrtable.AddAddrPair(start, end, false);
			this->modify_element(t);
		}
	}
};

}

#endif // _UTM_ADDRTABLEMAP_V4_H
