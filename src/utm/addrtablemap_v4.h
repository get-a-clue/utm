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
		addrtablemaprec_v4* t = dynamic_cast<addrtablemaprec_v4 *>(findptr_by_id(key_id));
		if (t != NULL)
		{
			t->addrtable.AddAddrPair(start, end, false);
		}
	}
};

}

#endif // _UTM_ADDRTABLEMAP_V4_H
