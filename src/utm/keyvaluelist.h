#ifndef _UTM_KEYVALUELIST_H
#define _UTM_KEYVALUELIST_H

#pragma once

#include <keyvalue.h>
#include <ubaselist.h>

namespace utm {

template<typename T>
class keyvaluelist : public ubaselist<keyvalue<T>>
{
public:
	keyvaluelist() {};
	~keyvaluelist() {};

	bool operator==(const keyvaluelist& rhs) const
	{
		return items == rhs.items;
	}

	void xml_create()
	{
		xml_append_root("KeyValueList");
		xml_append_node(KEYVALUE_XMLTAG_ROOT, items);
	}

	void xml_catch_value(const char *keyname, const char *keyvalue) { };

	ubase* xml_catch_subnode(const char *keyname)
	{
		ubase *u = NULL;

		if (strcmp(keyname, KEYVALUE_XMLTAG_ROOT) == 0)
		{
			keyvalue<T>* r = insert_and_get_inserted();
			u = (ubase *)r;
		}

		return u;
	}
};

}

#endif // _UTM_KEYVALUELIST_H