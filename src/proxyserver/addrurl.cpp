#include "stdafx.h"
#include "addrurl.h"

#include <boost/crc.hpp>
#include <boost/algorithm/string.hpp>

#include <stringtools.h>
#include <ubase_test.h>

namespace utm {

const char addrurl::this_class_name[] = "addrurl";
const char addrurl_params::this_class_name[] = "addrurl_params";

addrurl::addrurl(void)
{
	init();
}

addrurl::addrurl(const std::string& url)
{
	init();
	parse(url);
}

addrurl::~addrurl(void)
{
}

void addrurl::init()
{
	is_ipaddr = false;
	second_level_domain_crc = 0;
	depth = depth_unknown;
}

const char *addrurl::get_port() const
{
	if (!port.empty())
		return port.c_str();

	return HTTP_PORT;
}

void addrurl::extract_domains()
{
	is_ipaddr = check_is_host_ipaddr(host.c_str());
	if (is_ipaddr)
		return;

	std::string::size_type dot_pos[MAX_URL_DOTS];
	int dot_count = 0;

	std::string::size_type pos = 0;
	std::string::size_type cur = host.length();

	while(dot_count < MAX_URL_DOTS)
	{
		pos = host.find_last_of('.', cur);
		if (pos == std::string::npos)
			break;

		dot_pos[dot_count] = pos;
		dot_count++;

		if (dot_count == MAX_URL_DOTS)
			break;

		if (pos == 0)
			break;

		cur = pos - 1;
	}

	if (dot_count == 0)
		return;

	std::string::size_type secpos = 0;

	if (dot_count == 1)
	{
		if (dot_pos[0] == 0)
			return;

		depth = depth_second;
	}
	else
	{
		depth = depth_third;
		secpos = dot_pos[1] + 1;

		if (dot_count == 2)
			third_level_domain = host;

		if (dot_count == 3)
			third_level_domain = host.substr(dot_pos[2] + 1/*, host.length() - dot_pos[2] - 1*/);
	}

	second_level_domain = host.substr(secpos/*, host.length() - secpos*/);
	second_level_host = host.substr(secpos, dot_pos[0] - secpos);
}

bool addrurl::check_is_host_ipaddr(const char *host)
{
	bool digitonly = true;

	const char *c = host;
	while(*c != 0)
	{
		if (!isdigit(*c) && *c != '.')
		{
			digitonly = false;
			break;
		}
		c++;
	}

	return digitonly;
}

void addrurl::parse(const char *url)
{
	if (url == NULL)
		return;

	std::string u(url);
	parse(u);
}

void addrurl::parse(const std::string& url)
{
	std::string url_trimmed(url);
	boost::algorithm::trim(url_trimmed);

	original_addrurl = url_trimmed;

	std::string::size_type urllen = url.length();
	if (urllen == 0)
		return;

	std::string::size_type host_finish = std::string::npos;

	if (url[0] != '/')
	{
		std::string::size_type proto_pos = url.find(PROTO_END);
		
		if (proto_pos == std::string::npos)
		{
			std::string::size_type https_pos = url.find(HTTPS_PORT);
			if (https_pos != std::string::npos)
			{
				protocol = HTTPS_PROTOCOL;
				host = url.substr(0, https_pos - 1);
				port = HTTPS_PORT;
			}
			else
			{
				proto_pos = 0;
			}
		}

		if (proto_pos != std::string::npos)
		{
			std::string::size_type host_start  = 0;

			if (proto_pos != 0)
			{
				protocol = url.substr(0, proto_pos);
				host_start = proto_pos + sizeof(PROTO_END) - 1;
			}
			else
				protocol = HTTP_PROTOCOL;

			host_finish = url.find('/', host_start);
			if (host_finish == std::string::npos)
			{
				std::string::size_type len = urllen - host_start;
				host = url.substr(host_start, len);
			}
			else
			{
				std::string::size_type len = host_finish - host_start;
				host = url.substr(host_start, len);
			}

			std::string::size_type port_pos = host.find(':');
			if (port_pos != std::string::npos)
			{
				port = host.substr(port_pos + 1, host.length() - port_pos - 1);
				host = host.substr(0, port_pos);
			};
		};

		extract_domains();
		second_level_domain_crc = calc_crc32(second_level_domain);

		if (host_finish == std::string::npos)
			return;
	}

	if (host_finish == std::string::npos)
		host_finish = 0;

	std::string::size_type query_pos = url.find('?', host_finish);
	if (query_pos == std::string::npos)
	{
		path = url.substr(host_finish);
	}
	else
	{
		std::string::size_type len = query_pos - host_finish;
		path = url.substr(host_finish, len);
		query = url.substr(query_pos + 1);
	}
}

unsigned int addrurl::calc_crc32(const std::string& str)
{
	boost::crc_32_type result;
	result.process_bytes(str.data(), str.length());
	
	return result.checksum(); 
}

std::string addrurl::urldecode(const std::string& input_url)
{
    enum decode_state
    {
        STATE_SEARCH = 0, ///< searching for an ampersand to convert
        STATE_CONVERTING, ///< convert the two proceeding characters from hex
    };

	std::string retval;

    decode_state state = STATE_SEARCH;
	int ascii_char;
	char temp_buf[3];
	char c;

	for(std::string::const_iterator iter = input_url.begin(); iter != input_url.end(); ++iter)
    {
		c = (*iter);

		switch(state)
        {
	        case STATE_SEARCH:
    
				if (c != '%')
				{
					if (c == '+')
						c = ' ';

					retval.push_back(c);
				}
				else
	                state = STATE_CONVERTING;

				break;

	        case STATE_CONVERTING:
                
				state = STATE_SEARCH;

				if ((iter + 1) != input_url.end())
				{
					temp_buf[0] = c;
					temp_buf[1] = *(iter + 1);
					temp_buf[2] = 0;

					if ((isxdigit(temp_buf[0])) && (isxdigit(temp_buf[1])))
					{
#if defined(_MSC_VER)
						sscanf_s(temp_buf, "%x", &ascii_char);
#else
						sscanf(temp_buf, "%x", &ascii_char);
#endif
						retval.push_back((char)ascii_char);

						++iter;
					}
				}
	            break;
        }
    }

	return retval;
}

addrurl_params::addrurl_params(const std::string& query)
{
	parse(query);
}

addrurl_params::~addrurl_params()
{
}

void addrurl_params::parse(const std::string& query)
{
	std::list<std::string> p;
	size_t pos;

	utm::stringtools::split(p, query.c_str(), '&', utm::split::no_empties);
	for (auto iter = p.begin(); iter != p.end(); ++iter)
	{
		pos = iter->find('=');
		if (pos == std::string::npos)
		{
			params.insert(std::make_pair(*iter, std::string()));
		}
		else
		{
			params.insert(std::make_pair(iter->substr(0, pos), addrurl::urldecode(iter->substr(pos + 1))));
		}
	}
}

const char* addrurl_params::find(const std::string& paramname) const
{
	auto iter = params.find(paramname);
	if (iter == params.end())
	{
		return NULL;
	}

	return iter->second.c_str();
}

#ifdef UTM_DEBUG

#define ADDRURL_MAX_TEST_CASES 12

void addrurl::test_parse()
{
	const char *p[ADDRURL_MAX_TEST_CASES][2] = 
	{ { "test.com:443", "" }, { "test.com", "" }, { "www.test.com", "www.test.com" }, 
	{ "http://www.test.com", "www.test.com" }, { "https://www.test.com/", "www.test.com" }, { "http://www.test.com/path.htm", "www.test.com" }, 
	{ "http://www.test.com/path.htm?q", "www.test.com" }, { "http://www.test.com/path.htm?q=1", "www.test.com" }, 
	{ "http://www.test.com:80/path.htm?q=1", "www.test.com" }, { "http://www.test.com/url?q=http://www.rest.com", "www.test.com" },
	{ "http://www.about.test.com/", "about.test.com" }, { "http://www.company.about.test.com/?q=1", "about.test.com" }
	};

	for (int i = 0; i < ADDRURL_MAX_TEST_CASES; i++)
	{
		addrurl u;
		u.parse(p[i][0]);

		if (u.second_level_domain != "test.com")
			throw test_exception(this_class_name, "p1", i);

		if (u.third_level_domain != p[i][1])
			throw test_exception(this_class_name, "p2", i);

		if ((p[i][1][0]) == 0)
		{
			if (u.depth != addrurl::depth_second)
				throw test_exception(this_class_name, "p3", i);
		}
		else
		{
			if (u.depth != addrurl::depth_third)
				throw test_exception(this_class_name, "p4", i);
		}
	}
}

void addrurl::test_urldecode()
{
	std::string t1 = "rolling+stones+%D0%B6%D1%83%D1%80%D0%BD%D0%B0%D0%BB";
	std::string r1 = urldecode(t1);
	unsigned char q1[] = { 0x72, 0x6F, 0x6C, 0x6C, 0x69, 0x6E, 0x67, 0x20, 0x73, 0x74, 0x6F, 0x6E, 0x65, 0x73, 0x20, 0xD0, 0xB6, 0xD1, 0x83, 0xD1, 0x80, 0xD0, 0xBD, 0xD0, 0xB0, 0xD0, 0xBB, 0x00 };
	if (strcmp(r1.c_str(), (const char *)q1) != 0)
		throw test_exception(this_class_name, "u1");

	std::string t2 = "C%23+language+specification+4.0";
	std::string r2 = urldecode(t2);
	if (r2 != "C# language specification 4.0")
		throw test_exception(this_class_name, "u2");
}

void addrurl::test_all()
{
	test_report tr(this_class_name);

	addrurl a1(std::string(""));
	addrurl a2(std::string(HTTP_PROTOCOL));
	addrurl a3(std::string(PROTO_END));

	test_parse();
	test_urldecode();
}

void addrurl_params::test_all()
{
	test_report tr(this_class_name);

	{
		std::string q1 = "null";
		addrurl_params ap1(q1);
		TEST_CASE_CHECK(ap1.size(), size_t(1));
		TEST_CASE_CHECK(strcmp(ap1.find("null"), ""), 0);
		TEST_CASE_CHECK(ap1.find("fake"), NULL);
	}

	{
		std::string q2 = "null&fake";
		addrurl_params ap2(q2);
		TEST_CASE_CHECK(ap2.size(), size_t(2));
		TEST_CASE_CHECK(strcmp(ap2.find("null"), ""), 0);
		TEST_CASE_CHECK(strcmp(ap2.find("fake"), ""), 0);
	}

	{
		std::string q3 = "p1=null&fake";
		addrurl_params ap3(q3);
		TEST_CASE_CHECK(ap3.size(), size_t(2));
		TEST_CASE_CHECK(strcmp(ap3.find("p1"), "null"), 0);
		TEST_CASE_CHECK(strcmp(ap3.find("fake"), ""), 0);
	}

	{
		std::string q4 = "p1=null&p2=fake";
		addrurl_params ap4(q4);
		TEST_CASE_CHECK(ap4.size(), size_t(2));
		TEST_CASE_CHECK(strcmp(ap4.find("p1"), "null"), 0);
		TEST_CASE_CHECK(strcmp(ap4.find("p2"), "fake"), 0);
	}
}

#endif

}