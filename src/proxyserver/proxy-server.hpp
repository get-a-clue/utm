#ifndef PROXY_SERVER_H
#define PROXY_SERVER_H

#ifndef NOPROXYSERVER

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <event_msg_queue.h>

#include "proxyserver_common.h"
#include "proxy-serverbase.h"
#include "proxy-conn.hpp"

#include <boost/thread/mutex.hpp>

#include <deque>

typedef std::deque<io_service_ptr> ios_deque;

namespace utm {

class server : public serverbase {
public:
	server(const ios_deque& io_services, int port = 3128, event_msg_queue *elogger = NULL);

	void close();

private:
	void start_accept();
	void handle_accept(connection::pointer new_connection, const bs::error_code& error);
	
	ios_deque io_services_;
	ba::ip::tcp::acceptor acceptor_;

	boost::mutex guard;
};

}

#endif

#endif // PROXY_SERVER_H

