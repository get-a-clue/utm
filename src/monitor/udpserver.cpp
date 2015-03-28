#include "stdafx.h"
#include "udpserver.h"

namespace utm {

udpserver::udpserver(boost::asio::io_service& io_service, unsigned short port) : socket_(io_service, udp::endpoint(udp::v4(), port))
{
	has_reply = false;
	io_service_ptr = &io_service;
	port_ = port;
	start_receive();
}

udpserver::~udpserver(void)
{
}

void udpserver::stop_receive()
{
	io_service_ptr->stop();
}

void udpserver::start_receive()
{
	socket_.async_receive_from(
		boost::asio::buffer(recv_buffer_), remote_endpoint_,
		boost::bind(&udpserver::handle_receive, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void udpserver::handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error || error == boost::asio::error::message_size)
    {
		process_data(bytes_transferred);
		if (!has_reply)
			start_receive();
	}
}

void udpserver::handle_send(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	start_receive();
}

}