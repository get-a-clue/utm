#include "stdafx.h"

#ifndef NOPROXYSERVER

#include "proxy-server.hpp"

#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>

namespace utm {

server::server(const ios_deque& io_services, int port, event_msg_queue *elogger)
	: io_services_(io_services),
	  acceptor_(*io_services.front(), ba::ip::tcp::endpoint(ba::ip::tcp::v4(), port)) {

    eventlogger = elogger;

	if (eventlogger != NULL)
		eventlogger->AddMessage(event_pane::EP_AGENT, event_type::ET_DEBUG, "ProxyServer accepted connections");

	start_accept();
}

void server::close()
{
	{
		boost::mutex::scoped_lock lock(guard);

		acceptor_.cancel();
		acceptor_.close();

		for (BOOST_AUTO(iter, io_services_.begin()); iter != io_services_.end(); ++iter)
		{
			iter->get()->stop();
		}
	}

	if (eventlogger != NULL)
		eventlogger->AddMessage(event_pane::EP_AGENT, event_type::ET_DEBUG, "ProxyServer closed all connections");
}

void server::start_accept() 
{
	io_service_ptr is;

	{
		boost::mutex::scoped_lock lock(guard);
		io_services_.push_back(io_services_.front());
		io_services_.pop_front();
		is = io_services_.front();
	}

	connection::pointer new_connection = connection::create(*is, this);
	acceptor_.async_accept(new_connection->socket(),
						boost::bind(&server::handle_accept, this, new_connection,
						ba::placeholders::error));

	return;
}

void server::handle_accept(connection::pointer new_connection, const bs::error_code& error) 
{
	if (!error) 
	{
		new_connection->start();
		start_accept();
	}
}

}

#endif
