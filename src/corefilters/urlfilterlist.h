#ifndef _UTM_URLFILTERLIST_H
#define _UTM_URLFILTERLIST_H

#pragma once

#include <ubaselist.h>
#include "urlfilter.h"

#define URLFILTERLIST_XMLTAG_URLFILTER "UrlFilter"

namespace utm {

class urlfilterlist : public ubaselist<urlfilter>
{
public:
    static const char this_class_name[];

public:
	urlfilterlist(void);
	virtual ~urlfilterlist(void);

	bool operator==(const urlfilterlist& rhs) const;

    void clear();
    void xml_create();
    void xml_catch_value(const char *keyname, const char *keyvalue);
	ubase* xml_catch_subnode(const char *keyname);

#ifdef UTM_DEBUG
	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num);
#endif
};

}

#endif // _UTM_URLFILTERLIST_H