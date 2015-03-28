#include "stdafx.h"
#include "udpserver_test.h"

#include <iostream>

#ifdef UTM_DEBUG

namespace utm {

udpserver_test::udpserver_test(boost::asio::io_service& io_service, unsigned short port) : udpserver(io_service, port)
{
}


udpserver_test::~udpserver_test(void)
{
}

void udpserver_test::process_data(size_t bytes_transferred)
{
	char *u = recv_buffer_.data();
	if (bytes_transferred < UTM_UDPSERVER_MAXBUF_LEN)
	{
		u[bytes_transferred] = 0;
	}
	else
	{
		u[UTM_UDPSERVER_MAXBUF_LEN - 1] = 0;
	}

	std::cout << u << std::endl;
}

}

#endif // UTM_DEBUG