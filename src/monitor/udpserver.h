#ifndef UTM_UDPSERVER_H
#define UTM_UDPSERVER_H

#pragma once
#include <boost/asio.hpp>
#include <utm.h>

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

using boost::asio::ip::udp;

namespace utm {

#define UTM_UDPSERVER_MAXBUF_LEN 10240

struct ioservice_holder
{
	boost::asio::io_service ios;
};

class udpserver
{
public:
	udpserver(boost::asio::io_service& io_service, unsigned short port);
	virtual ~udpserver(void);

	virtual void process_data(size_t bytes_transferred) = 0;
	void stop_receive();

protected:
	void start_receive();
	void handle_receive(const boost::system::error_code& error, size_t bytes_transferred);

public:
	void handle_send(const boost::system::error_code& error, std::size_t bytes_transferred);

protected:
	bool has_reply;
	unsigned short port_;
	boost::asio::io_service* io_service_ptr;
	udp::socket socket_;
	udp::endpoint remote_endpoint_;
	boost::array<char, UTM_UDPSERVER_MAXBUF_LEN> recv_buffer_;
};

}

#endif // UTM_UDPSERVER_H