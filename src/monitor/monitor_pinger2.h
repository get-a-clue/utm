#ifndef UTM_MONITOR_PINGER2_H
#define UTM_MONITOR_PINGER2_H

#pragma once
#include <boost/asio.hpp>
#include <utm.h>

#include <monitor_total.h>
#include <macvendors.h>

#include <boost/bind.hpp>

namespace utm {

class monitor_pinger2
{
public:
	monitor_pinger2(boost::asio::io_service* io_service);
	~monitor_pinger2(void);

	unsigned short get_pid();

	void main(monitor_total& mt, const macvendors& mv);
	void send_icmp_to(const addrip_v4& addr, unsigned short seq_number);
	void handle_next_ping(const boost::system::error_code &err);
	void handle_analyze(const boost::system::error_code &err);

	void start_receive();
	void handle_receive(const boost::system::error_code& e, std::size_t length);

private:
	unsigned short pid;
	monitor_total* pmt;
	size_t current_ping_index;

	boost::asio::io_service* ios_ptr;
	boost::asio::streambuf reply_buffer_;
	boost::asio::ip::icmp::socket socket_;
	boost::asio::ip::icmp::endpoint destination_;
	boost::posix_time::ptime time_sent_;
	boost::asio::deadline_timer timer_;
};

}

#endif // UTM_MONITOR_PINGER2_H