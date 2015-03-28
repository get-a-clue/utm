#include "StdAfx.h"
#include "http_utils.h"

#include "addrurl.h"
#include <base64.h>
#include <ubase_test.h>

namespace utm {

const char http_utils::this_class_name[] = "http_utils";

http_utils::http_utils(void)
{
}


http_utils::~http_utils(void)
{
}

void http_utils::decode_authorization_header(const char *p, std::string& authtype, std::string& user, std::string& passwd)
{
	if (p == NULL)
		return;

	const char* space = strchr(p, ' ');
	if (space == NULL)
		return;

	authtype.assign(p, space - p);

	while ((*space == ' ') && (*space != 0))
	{
		space++;
	}

	if (*space != 0)
	{
		std::string encoded(space);
		size_t esize = encoded.size();
		if (esize > 0)
		{
			// trim all ending '\r' and '\n' characters
			esize--;
			while (esize >= 0)
			{
				if (encoded[esize] == '\r' || encoded[esize] == '\n')
				{
					encoded[esize] = 0;
					esize--;
				}
				else
				{
					break;
				}
			}

			std::string decoded;
			if (utm::base64::decode(encoded.c_str(), decoded, false))
			{
				size_t pos = decoded.find(':');
				if (pos != std::string::npos)
				{
					user = decoded.substr(0, pos);
					passwd = decoded.substr(pos + 1);
				}
			}
		}
	}
}

void http_utils::parse_uri_parameters(const char *params, stringmap& strmap)
{
	strmap.empty();
	if (params == NULL)
		return;

	size_t previous = 0, pos, equal_pos;
	std::string current, key, value;
	std::string parameters(params);

	do
	{
		pos = parameters.find('&', previous);
		if (pos == std::string::npos)
			pos = parameters.size();

		current = parameters.substr(previous, pos - previous);
		equal_pos = current.find('=');
		if (equal_pos != std::string::npos)
		{
			key = current.substr(0, equal_pos);
			value = utm::addrurl::urldecode(current.substr(equal_pos + 1));
			strmap[key] = value;
		}
		else
		{
			strmap[current] = std::string("");
		}
		previous = pos + 1;
	} while (previous < parameters.size());
}

void http_utils::test_all()
{
	test_case::classname.assign(this_class_name);
	test_case::testcase_num = 1;

	{
		std::string a1("Basic SGVsbG86V29ybGQ=\r\n");
		std::string authtype, user, password;
		utm::http_utils::decode_authorization_header(a1.c_str(), authtype, user, password);
		TEST_CASE_CHECK(authtype, std::string("Basic"));
		TEST_CASE_CHECK(user, std::string("Hello"));
		TEST_CASE_CHECK(password, std::string("World"));
	}

	{
		std::string a1("Basic  SGVsbG86V29ybGQ=\r\n");
		std::string authtype, user, password;
		utm::http_utils::decode_authorization_header(a1.c_str(), authtype, user, password);
		TEST_CASE_CHECK(authtype, std::string("Basic"));
		TEST_CASE_CHECK(user, std::string("Hello"));
		TEST_CASE_CHECK(password, std::string("World"));
	}

	{
		const char *params1 = "test_";
		stringmap result;
		utm::http_utils::parse_uri_parameters(params1, result);
		TEST_CASE_CHECK(result.size(), size_t(1));
		TEST_CASE_CHECK(result.begin()->first, std::string("test_"));
		TEST_CASE_CHECK(result["test_"], std::string(""));
	}

	{
		const char *params1 = "test_&";
		stringmap result;
		utm::http_utils::parse_uri_parameters(params1, result);
		TEST_CASE_CHECK(result.size(), size_t(1));
		TEST_CASE_CHECK(result.begin()->first, std::string("test_"));
		TEST_CASE_CHECK(result["test_"], std::string(""));
	}

	{
		const char *params1 = "a1=aa&b2=bbb";
		stringmap result;
		utm::http_utils::parse_uri_parameters(params1, result);
		TEST_CASE_CHECK(result.size(), size_t(2));
		TEST_CASE_CHECK(result.begin()->first, std::string("a1"));
		TEST_CASE_CHECK(result["a1"], std::string("aa"));
		TEST_CASE_CHECK(result.rbegin()->first, std::string("b2"));
		TEST_CASE_CHECK(result["b2"], std::string("bbb"));
	}

	{
		const char *params1 = "a1=aa&b2=";
		stringmap result;
		utm::http_utils::parse_uri_parameters(params1, result);
		TEST_CASE_CHECK(result.size(), size_t(2));
		TEST_CASE_CHECK(result.begin()->first, std::string("a1"));
		TEST_CASE_CHECK(result["a1"], std::string("aa"));
		TEST_CASE_CHECK(result.rbegin()->first, std::string("b2"));
		TEST_CASE_CHECK(result["b2"], std::string(""));
	}

	{
		const char *params1 = "a1=aa&b2=bbb&";
		stringmap result;
		utm::http_utils::parse_uri_parameters(params1, result);
		TEST_CASE_CHECK(result.size(), size_t(2));
		TEST_CASE_CHECK(result.begin()->first, std::string("a1"));
		TEST_CASE_CHECK(result["a1"], std::string("aa"));
		TEST_CASE_CHECK(result.rbegin()->first, std::string("b2"));
		TEST_CASE_CHECK(result["b2"], std::string("bbb"));
	}
}

}
