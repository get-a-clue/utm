#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>

namespace utm {

class http_client
{
public:
	http_client(void);
	~http_client(void);

	static int do_query(const char *host, const int port, const char *httprequest, std::string *response_header, std::string *response_data);
	static std::string rotate_logs(const int port, const std::string& webserver_hostname, const std::string& webserver_password);
};

}

#endif // HTTP_CLIENT_H