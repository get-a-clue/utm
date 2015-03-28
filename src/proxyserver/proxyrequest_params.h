#pragma once

#if defined(_MSC_VER)
#define UTM_WIN
#endif

#ifdef UTM_WIN

#include <Windows.h>
#include <tchar.h>

#endif

#include <gstring.h>

#include "askgoogle.h"
#include "addrurl.h"
#include "html_document.h"
#include "http_headers.h"

#include <boost/asio/ip/address.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace utm {

//typedef int (*pWhatSitePtr)(const request_http_headers *);

class proxyrequest_params
{
public:
	enum request_phases { phase_unknown = -1, phase_requestheader = 0, phase_replybody = 1 };

	proxyrequest_params(const request_http_headers& req, const html_document& html)
	{
		clear();
		request_phase = proxyrequest_params::phase_replybody;
		request = const_cast<request_http_headers *>(&req);
		phtml = const_cast<html_document *>(&html);
	}

	proxyrequest_params(const request_http_headers& req)
	{
		clear();
		request_phase = proxyrequest_params::phase_requestheader;
		request = const_cast<request_http_headers *>(&req);
	}

	~proxyrequest_params(void) { };

	request_phases request_phase;
	request_http_headers *request;
	html_document *phtml;

	boost::function<int (const request_http_headers *)> whatsite;
	std::unique_ptr<askgoogle_exception> askexc;

	const char* proxyrulename4log;
	const char* proxyrulename4web;
	bool proxyrule_nolog_access;
	int proxyruleaction;

private:
	void clear()
	{
		request = NULL;
		phtml = NULL;
		whatsite = NULL;

		proxyrulename4log = NULL;
		proxyrulename4web = NULL;
		proxyrule_nolog_access = false;
		proxyruleaction = 0;
	}
};

}
