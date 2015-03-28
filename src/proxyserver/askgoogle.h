#ifndef ASKGOOGLE_H
#define ASKGOOGLE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <list>

#include "http_headers.h"

#define GOOGLE_ERROR 0
#define GOOGLE_SEX 1
#define GOOGLE_SUSPICIOUS 2
#define GOOGLE_OK 3

namespace utm {

class askgoogle_exception
{
public:
	enum phases
	{
		phase_noerror = 0,
		phase_nohttp = 1,
		phase_request = 2,
		phase_analyze = 3,
		phase_nolinks = 4
	};

	askgoogle_exception()
	{
		phase = phase_noerror;
	}

	askgoogle_exception(const char *w, int _response_code, const std::string& _response_header, const std::string& _response_data, phases _phase)
	{
		if (w != NULL)
			whatstr.assign(w);

		response_code = _response_code;
		response_header = _response_header;
		response_data = _response_data;
		phase = _phase;
	}
	
	const char* what() const
	{
		return whatstr.c_str();
	}

	phases phase;
	std::string response_header;
	std::string response_data;
	int response_code;

protected:
	std::string whatstr;
};

typedef std::list<std::string> links_container;

class askgoogle
{
public:
	static const char this_class_name[];

	askgoogle(void);
	~askgoogle(void);

	static int analyze_site(const char *site, const request_http_headers *p, const char *googlehost);

	static std::string get_reply(int replycode);

private:
	static void query(const char *site, const request_http_headers *request, const char *googlehost, std::string& response_header, std::string& response_data);
	static void construct_query(const char *site, const request_http_headers *request, const char *googlehost, std::string& query);
	static int analyze_query(const char *site, const char* googlehost, const std::string& response_header, const std::string& response_data);
	static void parse_result(const std::string& page_content, std::string& search_result);
	static void extract_links(const std::string& search_result, links_container& links);
	static int analyze_link(const std::string& link, const char *site);

public:
	static void test_all();
};

}

#endif // ASKGOOGLE_H
