#ifndef UTM_UDPSERVER_TEST_H
#define UTM_UDPSERVER_TEST_H

#pragma once
#include "udpserver.h"

#ifdef UTM_DEBUG

namespace utm {

class udpserver_test : public udpserver
{
public:
	udpserver_test(boost::asio::io_service& io_service, unsigned short port);
	~udpserver_test(void);

	void process_data(size_t bytes_transferred);
};

}

#endif // UTM_DEBUG

#endif // UTM_UDPSERVER_TEST_H
