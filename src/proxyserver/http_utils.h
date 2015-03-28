#pragma once

#include <string>
#include <map>

namespace utm {

typedef std::map<std::string, std::string> stringmap;

class http_utils
{
public:
	static const char this_class_name[];

	http_utils(void);
	~http_utils(void);

	static void decode_authorization_header(const char *p, std::string& authtype, std::string& user, std::string& passwd);
	static void parse_uri_parameters(const char *param_string, stringmap& strmap);

	static void test_all();
};

}