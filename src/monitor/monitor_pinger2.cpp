#include "stdafx.h"
#include "monitor_pinger2.h"

#include <addrip_v4.h>

#include "monitor_total.h"
#include "icmp_header.hpp"
#include "ipv4_header.hpp"
#include "arp_table.h"

#include <istream>
#include <iostream>
#include <ostream>


namespace utm {

monitor_pinger2::monitor_pinger2(boost::asio::io_service* io_service) : 
	ios_ptr(io_service), socket_(*io_service, boost::asio::ip::icmp::v4()), timer_(*io_service)
{
}

monitor_pinger2::~monitor_pinger2(void)
{
}

unsigned short monitor_pinger2::get_pid()
{
	if (pid == 0)
	{
		pid = utm::get_pid();
	}

	return pid;
}


void monitor_pinger2::main(monitor_total& mt, const macvendors& mv)
{
	UDEBUG("monitor_pinger2::main()");

	pid = 0;
	current_ping_index = 0;
	pmt = &mt;

    start_receive();

	boost::system::error_code err;
	err.assign(0, boost::system::generic_category());
	handle_next_ping(err);
}

void monitor_pinger2::handle_next_ping(const boost::system::error_code &err)
{
	if ((!err) && (err != boost::asio::error::operation_aborted))
	{
		if (current_ping_index < pmt->result_size())
		{
			monitor_result mr;
			pmt->get_result_record(current_ping_index, mr);
			mr.curr_serial_request_count++;
			mr.total_ping_request_count++;
			mr.curr_tick_request = monitor_total::get_current_hires_tick();
			pmt->set_result_record(current_ping_index, mr);

			unsigned short sequence_number = static_cast<unsigned short>(mr.curr_serial_request_count & 0xffff);
			send_icmp_to(mr.addr, sequence_number);

			current_ping_index++;
			timer_.expires_at(time_sent_ + boost::posix_time::microseconds(2));
			timer_.async_wait(boost::bind(&monitor_pinger2::handle_next_ping, this, _1));

			if (current_ping_index == 1)
			{
				ios_ptr->reset();
				ios_ptr->run();
			}
		}
		else
		{
			timer_.expires_at(time_sent_ + boost::posix_time::seconds(1));
			timer_.async_wait(boost::bind(&monitor_pinger2::handle_analyze, this, _1));
		}
	}
}

void monitor_pinger2::handle_analyze(const boost::system::error_code &err)
{
	timer_.cancel();
	socket_.cancel();
	ios_ptr->stop();
}

void monitor_pinger2::send_icmp_to(const addrip_v4& addr, unsigned short sequence_number)
{
	boost::asio::ip::address_v4 target_ip(addr.m_addr);
	boost::asio::ip::icmp::endpoint target_endpoint(target_ip, 0);

	std::string body("HELLO HELLO HELLO HELLO.");

	// Create an ICMP header for an echo request.
	icmp_header echo_request;
	echo_request.type(icmp_header::echo_request);
	echo_request.code(0);
	echo_request.identifier(get_pid());
	echo_request.sequence_number(sequence_number);
	compute_checksum(echo_request, body.begin(), body.end());

	// Encode the request packet.
	boost::asio::streambuf request_buffer;
	std::ostream os(&request_buffer);
	os << echo_request << body;

	// Send the request.
	time_sent_ = boost::posix_time::microsec_clock::universal_time();
	socket_.send_to(request_buffer.data(), target_endpoint);
}

void monitor_pinger2::start_receive()
{
	// Discard any data already in the buffer.
	reply_buffer_.consume(reply_buffer_.size());

	// Wait for a reply. We prepare the buffer to receive up to 64KB.
	socket_.async_receive(reply_buffer_.prepare(65536),
		boost::bind(&monitor_pinger2::handle_receive, this, 
		boost::asio::placeholders::error, 
		boost::asio::placeholders::bytes_transferred));
}

void monitor_pinger2::handle_receive(const boost::system::error_code& e, std::size_t length)
{
	if (e)
	{
		start_receive();
	}

	// The actual number of bytes received is committed to the buffer so that we
	// can extract it using a std::istream object.
	reply_buffer_.commit(length);

	// Decode the reply packet.
	std::istream is(&reply_buffer_);
	ipv4_header ipv4_hdr;
	icmp_header icmp_hdr;
	is >> ipv4_hdr >> icmp_hdr;

	// We can receive all ICMP packets received by the host, so we need to
	// filter out only the echo replies that match the our identifier and
	// expected sequence number.
	if (is && icmp_hdr.type() == icmp_header::echo_reply && icmp_hdr.identifier() == get_pid())
	{
		addrip_v4 a(ipv4_hdr.source_address().to_ulong());
		utm::monitor_index_container::iterator iter = pmt->results_index.find(a);
		if (iter != pmt->results_index.end())
		{
//			unsigned short sequence_number = static_cast<unsigned short>(icmp_hdr.sequence_number());
//			if (sequence_number == icmp_hdr.sequence_number())
			{
				unsigned int index = iter->second;
				monitor_result mr;
				pmt->get_result_record(index, mr);
				if (mr.addr == a)
				{
					mr.last_ping_reply.now();
					mr.curr_serial_reply_count++;
					mr.total_ping_reply_count++;
					mr.curr_tick_reply = monitor_total::get_current_hires_tick();
					pmt->set_result_record(index, mr);
				}
			}
		}
	}

	start_receive();
}

}