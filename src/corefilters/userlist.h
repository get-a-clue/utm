#ifndef _UTM_USERLIST_H
#define _UTM_USERLIST_H

#pragma once
#include <utm.h>

#include <ubaselist.h>
#include <fsuser.h>

#define USERLIST_XMLTAG_ROOT "UserList"
#define USERLIST_XMLTAG_USER "User"

namespace utm {

class userlist : public ubaselist<fsuser>
{
public:
    static const char this_class_name[];

public:
	userlist(void);
	~userlist(void);

	bool operator==(const userlist& rhs) const;

	void authenticate_user(const std::string& username, const std::string& userpass, fsuser& founduser);

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

#endif // _UTM_USERLIST_H