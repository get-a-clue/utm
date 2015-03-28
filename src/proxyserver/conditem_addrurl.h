#pragma once

#if defined(_MSC_VER)
#define UTM_WIN
#endif

#ifdef UTM_WIN

#include <Windows.h>
#include <tchar.h>

#endif

#include <string>
#include <vector>

#include "conditem.h"

namespace utm {

typedef std::vector<std::string> url_container;
typedef std::vector<std::string>::iterator url_iterator;

class conditem_addrurl : public conditem
{
public:
	enum urlparts { urlpart_none = 0, urlpart_any = 1, urlpart_host = 2, urlpart_path = 3 };

	static urlparts get_urlpart_from_conditem_type(condtypes ct)
	{
		urlparts u;

		switch(ct)
		{
			case condtype_urlhostpath: u = urlpart_any; break;
			case condtype_urlhost: u = urlpart_host; break;
			case condtype_urlpath: u = urlpart_path; break;
			default: u = urlpart_none; break;
		}

		return u;
	}

public:
	static const char this_class_name[];
	const char* get_this_class_name() const { return conditem_addrurl::this_class_name; };

	conditem_addrurl(urlparts part, const char *u);
	~conditem_addrurl(void);

	void copy_properties(const conditem& rhs);

	void set_conditem_addrurl(urlparts _url, const char* u);

	void clear();
	bool parse(const std::string& str);
	std::string to_string() const;

	condtypes get_conditem_type() const
	{
		if (urlpart == urlpart_any)	return condtype_urlhostpath;
		if (urlpart == urlpart_host) return condtype_urlhost;
		if (urlpart == urlpart_path) return condtype_urlpath;
		return condtype_none;
	};
	bool do_check(proxyrequest_params *params);
	bool do_check_url(const std::string& url, proxyrequest_params *params) const;

	urlparts urlpart;
	url_container urls;

#ifdef UTM_DEBUG
	static bool test_all();
#endif

private:
	conditem_addrurl(void);
};

}
