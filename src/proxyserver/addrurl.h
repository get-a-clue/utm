#pragma once

#if defined(_MSC_VER)
#define UTM_WIN
#endif

#ifdef UTM_WIN

#include <Windows.h>
#include <tchar.h>

#endif

#include <string>
#include <map>

#define MAX_URL_DOTS 3
#define SECOND_LEVEL_DOMAIN 2
#define THIRD_LEVEL_DOMAIN 3
#define PROTO_END "://"
#define HTTPS_PORT "443"
#define HTTPS_PROTOCOL "https"
#define HTTP_PROTOCOL "http"
#define HTTP_PORT "80"

namespace utm {

class addrurl
{
public:
	static const char this_class_name[];

	enum domain_depth { depth_unknown = 0, depth_second = 1, depth_third = 2 };

	addrurl(void);
	addrurl(const std::string& url);
	~addrurl(void);

	static std::string urldecode(const std::string& input_url);
	static bool check_is_host_ipaddr(const char *host);
	bool is_host_ipaddr() { return is_ipaddr; }

	void parse(const char *url);
	void parse(const std::string& url);

	std::string original_addrurl;
	std::string protocol;
	std::string host;
	std::string second_level_domain;
	std::string third_level_domain;
	std::string second_level_host;
	std::string port;
	std::string path;
	std::string query;
	domain_depth depth;

	const char *get_port() const;

private:
	void init();
	void extract_domains();

	bool is_ipaddr;
	unsigned int second_level_domain_crc;
	static unsigned int calc_crc32(const std::string& str);

public:
#ifdef UTM_DEBUG
	static void test_all();
	static void test_parse();
	static void test_urldecode();
#endif
};

typedef std::map<std::string, std::string> addrurl_params_container;

class addrurl_params
{
public:
	static const char this_class_name[];

	addrurl_params(const std::string& query);
	~addrurl_params();

	size_t size() const { return params.size(); };
	void parse(const std::string& query);
	const char* find(const std::string& paramname) const;

	template<typename T>
	T get(const char* paramname, const T& default_value) const
	{
		const char *p = find(paramname);
		if (p != NULL)
		{
			try
			{
				T retval = boost::lexical_cast<T>(p);
				return retval;
			}
			catch (const boost::bad_lexical_cast&)
			{
			}
		}

		return default_value;
	}

private:
	addrurl_params_container params;

public:
#ifdef UTM_DEBUG
	static void test_all();
#endif
};

}