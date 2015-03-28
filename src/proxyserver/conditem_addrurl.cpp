#include "stdafx.h"
#include "conditem_addrurl.h"

#include <stringtools.h>
#include <ubase_test.h>

namespace utm {

const char conditem_addrurl::this_class_name[] = "conditem_addrurl";

conditem_addrurl::conditem_addrurl(void)
{
}

conditem_addrurl::conditem_addrurl(urlparts _urlpart, const char *u)
{
	set_conditem_addrurl(_urlpart, u);
}

conditem_addrurl::~conditem_addrurl(void)
{
}

void conditem_addrurl::copy_properties(const conditem& rhs)
{
	conditem_addrurl *p = (conditem_addrurl *)&rhs;

	urlpart = p->urlpart;
	urls = p->urls;

	conditem::copy_properties(rhs);
}

void conditem_addrurl::set_conditem_addrurl(urlparts _urlpart, const char* u)
{
	urlpart = _urlpart;
	urls.clear();

	if (u != NULL)
	{
		parse(std::string(u));
		condvalue.assign(u);
	}
}

void conditem_addrurl::clear()
{
	urls.clear();
	conditem::clear();
}

bool conditem_addrurl::parse(const std::string& str)
{
	if (str.empty())
		return false;

	if (str[0] != '"')
	{
		std::vector<std::string> args;
		stringtools::split(args, str.c_str(), ' ', split::no_empties);
		for (std::vector<std::string>::iterator iter = args.begin(); iter != args.end(); ++iter)
		{
			urls.push_back(*iter);
		}
	}
	else
	{
	// TODO: Написать код, читающий из файла url'ы, если в качестве аргумента передано имя файла
	}

	return true;
}

std::string conditem_addrurl::to_string() const
{
	std::ostringstream oss;

	url_container::const_iterator iter;
	for (iter = urls.begin(); iter != urls.end(); ++iter)
	{
		if (iter != urls.begin())
			oss << " ";

		oss << *iter;
	}

	return oss.str();
}

bool conditem_addrurl::do_check(proxyrequest_params *params)
{
	bool retval = 0;

	url_iterator iter;
	for (iter = urls.begin(); iter != urls.end(); ++iter)
	{
		retval = do_check_url((*iter), params);
		if (retval)
			break;
	}

	return retval;
}

bool conditem_addrurl::do_check_url(const std::string& url, proxyrequest_params *params) const
{
	if (url.length() == 0)
		return false;

	std::string *pstring = NULL;

	if (urlpart == urlpart_any)
		pstring = &params->request->url.original_addrurl;

	if (urlpart == urlpart_host)
		pstring = &params->request->url.host;

	if (urlpart == urlpart_path)
		pstring = &params->request->url.path;

	if (pstring == NULL)
		return false;

	char c = *(url.end() - 1);
	bool rfind = c == '$' ? true : false;

	if (!rfind)
	{
		size_t pos = pstring->find(url);
		if (pos == std::string::npos)
			return false;
		else
			return true;
	}
	else
	{
		std::string pattern = url.substr(0, url.length() - 1);
		size_t pos = pstring->rfind(pattern);

		if (pos == std::string::npos)
			return false;

		size_t len = pstring->length();
		size_t pattern_len = pattern.length();

		if ((pos + pattern_len) == len)
			return true;

		return false;
	}

	return false;
}

#ifdef UTM_DEBUG
bool conditem_addrurl::test_all()
{
	test_report tr(this_class_name);

//	addrurl url;
	request_http_headers request;
	request.url.parse("http://www.swf.test.com/video.swf?id=1");

	proxyrequest_params pp(request);

	conditem_addrurl c1(urlpart_any, "test");
	if (!c1.do_check(&pp))
		throw test_exception(this_class_name, "1");

	conditem_addrurl c2(urlpart_any, "id=");
	if (!c2.do_check(&pp))
		throw test_exception(this_class_name, "2");

	conditem_addrurl c3(urlpart_host, "test.com");
	if (!c3.do_check(&pp))
		throw test_exception(this_class_name, "3");

	conditem_addrurl c4(urlpart_host, "test.com$");
	if (!c4.do_check(&pp))
		throw test_exception(this_class_name, "4");

	conditem_addrurl c5(urlpart_host, "test.ru");
	if (c5.do_check(&pp))
		throw test_exception(this_class_name, "5");

	conditem_addrurl c6(urlpart_path, "swf$");
	if (!c6.do_check(&pp))
		throw test_exception(this_class_name, "6");

	conditem_addrurl c7(urlpart_path, "video$");
	if (c7.do_check(&pp))
		throw test_exception(this_class_name, "7");

	conditem_addrurl c8(urlpart_any, "fake meteor www");
	if (!c8.do_check(&pp))
		throw test_exception(this_class_name, "8");

	return true;
}
#endif


}