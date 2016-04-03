#include "stdafx.h"
#include "userlist.h"

#include <md5.h>

#include <algorithm>

namespace utm {

const char userlist::this_class_name[] = "userlist";

userlist::userlist(void)
{
}


userlist::~userlist(void)
{
}

void userlist::clear()
{
	ubaselist::clear();
}
    
void userlist::xml_create()
{
	xml_append_root(USERLIST_XMLTAG_ROOT);
    xml_append_node(USERLIST_XMLTAG_USER, items);
}

void userlist::authenticate_user(const std::string& username, const std::string& userpass, fsuser& founduser)
{
	gstring uname(username);

	for (auto iter = items.begin(); iter != items.end(); ++iter)
	{
		auto *p = dynamic_cast<fsuser *>(iter->get());

		if ((p != NULL) && (p->get_name().is_ci_equal(uname)))
		{
			if (((p->m_szPasswd.size() == MD5_HASH_STRING_SIZE) && (userpass.size() == MD5_HASH_STRING_SIZE)) ||
				((p->m_szPasswd.size() != MD5_HASH_STRING_SIZE) && (userpass.size() != MD5_HASH_STRING_SIZE)))
			{
				const std::string& saved_pass = p->m_szPasswd;
				if (saved_pass == userpass)
				{
					founduser = *p;
				}
			}
			else if ((userpass.size() != MD5_HASH_STRING_SIZE) && (p->m_szPasswd.size() == MD5_HASH_STRING_SIZE))
			{
				std::string userpass_md5;
				utm::md5::md5_string(userpass.c_str(), userpass_md5);
				if (p->m_szPasswd == userpass_md5)
				{
					founduser = *p;
				}
			}
			break;
		}
	}
}

void userlist::xml_catch_value(const char *keyname, const char *keyvalue)
{
}

ubase* userlist::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, USERLIST_XMLTAG_USER) == 0)
	{
		u = new fsuser();
		add_element(u);
	}

	return u;
}

#ifdef UTM_DEBUG
void userlist::test_fillparams(int test_num)
{
	fsuser u;
	u.m_szName.assign("Pupkin");
	u.m_szDescr.assign("Pupkin Vasily Ivanovich");
	add_element(u, 0);
}
#endif // UTM_DEBUG

}