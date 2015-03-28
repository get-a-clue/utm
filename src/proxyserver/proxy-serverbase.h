#pragma once

#include "configproxy.h"
#include "proxyserver_common.h"
#include "addrurl.h"
#include "askgoogle.h"
#include "html_document.h"
#include "accessrecord.h"
#include "squidrecord.h"
#include "googlerecord.h"
#include "proxyrequest_params.h"
#include "http_headers.h"
#include "googlecache.h"

#include <event_holder.h>
#include <event_msg_queue.h>

#include <string>

#include <boost/bind.hpp>

namespace utm {

class serverbase
{
public:
	serverbase(void);
	virtual ~serverbase(void);

	std::unique_ptr<configproxy> cfg;
	bool is_accept_connections;

	event_holder<accessrecord> *accesslogger;
	event_holder<squidrecord> *squidlogger;
	event_holder<googlerecord> *googlelogger;
	event_msg_queue *eventlogger;

	std::string debug_prefix;
	googlecache gc;

	void init_loggers(bool enable_file = false);

	bool process_request(proxyrequest_params* pp);
	int whatsite(const request_http_headers* req);
	int whatsiteref(const request_http_headers* req);

	virtual void process_webserver_request(const utm::request_http_headers& request, utm::response_http_headers& response);
};

}
