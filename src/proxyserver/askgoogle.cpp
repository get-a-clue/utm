#include "stdafx.h"
#include "askgoogle.h"

#include "addrurl.h"
#include "http_client.h"
#include "googlecookies.h"


#include <algorithm>

#include <ubase_test.h>

namespace utm {

const char askgoogle::this_class_name[] = "askgoogle";

askgoogle::askgoogle(void)
{
}


askgoogle::~askgoogle(void)
{
}

#define GOOGLE_RESULT_START "<div id=\"ires\"><ol"
#define GOOGLE_RESULT_END "</ol></div></div>"
#define GOOGLE_RESULT_EMPTY "<div id=\"ires\"></div>"
#define DIV_OPEN "<div"
#define DIV_CLOSE "</div>"
#define LI_OPEN "<li"
#define LI_CLOSE "</li>"
#define AHREF_OPEN "<a href="
#define TAG_CLOSE "\">"
#define HTTP_PREFIX "http://"
#define HTTPS_PREFIX "https://"
#define GOOGLE_COM "www.google.com"
#define GOOGLE_DEFAULT_HTTPPORT 80
#define HTTP_OK 200
#define MAX_ANALYZE_LINKS 10

std::string askgoogle::get_reply(int replycode)
{
	std::string reply;

	if (replycode == GOOGLE_ERROR)
		reply = "Error during query";

	if (replycode == GOOGLE_SEX)
		reply = "Site is not safe (Sexual or dangerous content)";

	if (replycode == GOOGLE_SUSPICIOUS)
		reply = "Suspicious site";

	if (replycode == GOOGLE_OK)
		reply = "Good site";

	return reply;
}

int askgoogle::analyze_site(const char *site, const request_http_headers *request, const char *googlehost)
{
	int result = GOOGLE_ERROR;
	std::string response_header, response_data;

	askgoogle_exception::phases phase = askgoogle_exception::phase_request;
	try
	{
		query(site, request, googlehost, response_header, response_data);
	
		phase = askgoogle_exception::phase_analyze;
		result = analyze_query(site, googlehost, response_header, response_data);
	}
	catch(const std::exception& e)
	{
		const char* what = e.what();
		throw askgoogle_exception(what, HTTP_OK, response_header, response_data, askgoogle_exception::phase_request);
	}

	return result;
}

void askgoogle::query(const char *site, const request_http_headers *request, const char *googlehost,
	std::string& response_header, std::string& response_data)
{
	std::string query;
	askgoogle::construct_query(site, request, googlehost, query);
	int response_code = http_client::do_query(googlehost, GOOGLE_DEFAULT_HTTPPORT, query.c_str(), &response_header, &response_data);

	if (HTTP_OK != response_code)
	{
		std::ostringstream oss;
		oss << "HTTP_OK was not received from " << googlehost;
		throw askgoogle_exception(oss.str().c_str(), response_code, response_header, response_data, askgoogle_exception::phase_nohttp);
	}
}

int askgoogle::analyze_query(const char *site, const char* googlehost, const std::string& response_header, const std::string& response_data)
{
	int result = GOOGLE_ERROR;

	std::string search_result;
	links_container links;

	parse_result(response_data, search_result);
	extract_links(search_result, links);

	if (links.empty())
	{
		std::string::size_type index = response_data.find(GOOGLE_RESULT_EMPTY);
		if (index != std::string::npos)
		{
			result = GOOGLE_SEX;
		}
		else
		{
			std::ostringstream oss;
			oss << "The recieved document from " << googlehost << " doesn't contains any links";
			throw askgoogle_exception(oss.str().c_str(), HTTP_OK, response_header, response_data, askgoogle_exception::phase_nolinks);
		}
	}

	int i = 0;
	addrurl prevlink;

	links_container::iterator iter;
	for (iter = links.begin(); iter != links.end(); ++iter)
	{
		addrurl link(*iter);

		if (prevlink.second_level_domain != link.second_level_domain)
		{
			result = askgoogle::analyze_link(*iter, site);
			if ((result == GOOGLE_SUSPICIOUS) || (result == GOOGLE_OK))
			{
				break;
			}

			prevlink = link;
			i++;
		}

		if (i >= MAX_ANALYZE_LINKS)
			break;
	}

	return result;
}

void askgoogle::parse_result(const std::string& page_content, std::string& search_result)
{
	std::string::size_type start_index = page_content.find(GOOGLE_RESULT_START);
	if (start_index == std::string::npos)
		return;

	start_index += sizeof(GOOGLE_RESULT_START) - 1;

	int divs_open = 0;
	int divs_close = 0;

	std::string::size_type i = start_index;
	while(i < page_content.length())
	{
		std::string::size_type pos = page_content.find('<', i);
		if (pos == std::string::npos)
			return;

		i = pos;

		if (page_content.find(DIV_OPEN, i) == i)
		{
			divs_open++;
			i += sizeof(DIV_OPEN) - 1;
			continue;
		}

		if (page_content.find(DIV_CLOSE, i) == i)
		{
			divs_close++;
			i += sizeof(DIV_CLOSE) - 1;

			if (divs_close == (divs_open + 2))
			{
				std::string::size_type chars_num = i - start_index;
				search_result = page_content.substr(start_index, chars_num);
				std::transform(search_result.begin(), search_result.end(), search_result.begin(), ::tolower);

				break;
			}
			continue;
		}

		i++;
	}
}

void askgoogle::extract_links(const std::string& search_result, links_container& links)
{
	std::string::size_type i = 0;
	std::string::size_type li_pos = 0;

	while(i < search_result.length())
	{
		std::string::size_type pos = search_result.find('<', i);
		if (pos == std::string::npos)
			break;

		i = pos + 1;

		if (search_result.find(LI_OPEN, pos) == pos)
		{
			li_pos = search_result.find('>', pos) + 1;
			continue;
		}

		if (search_result.find(LI_CLOSE, pos) == pos)
		{
			if (li_pos > 0)
			{
				std::string::size_type chars_num = pos - li_pos;
				std::string link = search_result.substr(li_pos, chars_num);
				links.push_back(link);
			}

			li_pos = 0;
			continue;
		}
	}
}

int askgoogle::analyze_link(const std::string& link, const char *site)
{
	std::string::size_type ahref_pos = link.find(AHREF_OPEN, 0);
	if (ahref_pos == std::string::npos)
		return GOOGLE_ERROR;

	std::string::size_type link_pos = ahref_pos + sizeof(AHREF_OPEN);

	std::string::size_type ahref_endpos = link.find(TAG_CLOSE, link_pos);
	if (ahref_endpos == std::string::npos)
		return GOOGLE_ERROR;

	std::string::size_type ahref_quote_pos = link.find('"', link_pos);
	if (ahref_quote_pos < ahref_endpos)
		ahref_endpos = ahref_quote_pos;

	std::string href = link.substr(link_pos, ahref_endpos - link_pos);
	addrurl hr(href);

	std::string::size_type pos_http = hr.query.find(HTTP_PREFIX);
	std::string::size_type pos_https = hr.query.find(HTTPS_PREFIX);

	if ((pos_http != std::string::npos) || (pos_https != std::string::npos))
	{
		addrurl nested_hr(hr.query);
		addrurl siteurl(site);

		switch (siteurl.depth)
		{
			case addrurl::depth_second:
				if (nested_hr.second_level_domain != siteurl.second_level_domain)
					return GOOGLE_SEX;

				break;

			case addrurl::depth_third:
				if (nested_hr.third_level_domain != siteurl.third_level_domain)
					return GOOGLE_SEX;

				break;

			default:
				return GOOGLE_SEX;
		}

		if (nested_hr.path.find("/&amp") == 0)
		{
			return GOOGLE_OK;
		}

		return GOOGLE_SUSPICIOUS;
	}
	else
	{
		std::string::size_type last_pos = hr.host.rfind(site);
		if (last_pos != std::string::npos)
		{
			std::string::size_type sz = last_pos + strlen(site);
			if (sz == hr.host.size())
			{
				return GOOGLE_OK;
			}
		}
	}

	return GOOGLE_SEX;
}

void askgoogle::construct_query(const char *site, const request_http_headers *request, const char *googlehost, std::string& query)
{
	std::ostringstream os; 

	os << "GET /search?q=" << HTTP_PREFIX << site;
//	os << "+site%3A" << site;
	os << "&btnG=Search";
	os << "&safe=active HTTP/1.1\r\n";
	os << "Host: " << googlehost << "\r\n";
	os << "Accept: text/html\r\n";
	os << "Accept-Language: en\r\n";

	if (request != NULL)
	{
//  	const char *accept = request->find_header("accept");
//	    if (accept != NULL)
//			os << "Accept: " << accept << "\r\n";

//		const char *acceptlanguage = request->find_header("accept-language");
//		if (acceptlanguage != NULL)
//			os << "Accept-Language: " << acceptlanguage << "\r\n";

		const char *useragent = request->find_header("user-agent");
//		if (useragent != NULL)
//			os << "User-Agent: " << useragent << "\r\n";

		std::string cookie;
		googlecookies* gc = googlecookies::instance();
		gc->get_cookie(request->client_addr, useragent, googlehost, cookie);
		if (!cookie.empty())
		{
			os << "Cookie: " << cookie << "\r\n";
		}
	}

	os << "Connection: close\r\n\r\n";

	query = os.str();
}

void askgoogle::test_all()
{
	test_report tr(this_class_name);

	const char googlehost[] = "www.google.com";

	int as1 = askgoogle::analyze_site("google.com", NULL, googlehost);
	if (as1 != GOOGLE_OK)
		throw test_exception(this_class_name, "a1");

	int as2 = askgoogle::analyze_site("microsoft.com", NULL, googlehost);
	if (as2 != GOOGLE_OK)
		throw test_exception(this_class_name, "a2");

	int as3 = askgoogle::analyze_site("rbc.ru", NULL, googlehost);
	if (as3 != GOOGLE_OK)
		throw test_exception(this_class_name, "a3");

	int as4 = askgoogle::analyze_site("xhamster.com", NULL, googlehost);
	if (as4 != GOOGLE_SEX)
		throw test_exception(this_class_name, "a4");
}

}