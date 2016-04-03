#ifndef _UTM_FILTERCONSLIST_H
#define _UTM_FILTERCONSLIST_H

#pragma once
#include <utm.h>
#include <ubaselist.h>
#include <filtercons.h>

namespace utm {

class filterconslist : public ubaselist<utm::filtercons>
{
public:
	filterconslist();
	~filterconslist();

	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *tag_name, const char *class_name);
};

}

#endif