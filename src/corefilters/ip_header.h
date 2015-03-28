#ifndef UTM_IP_HEADER_H
#define UTM_IP_HEADER_H

#pragma once
#include <utm.h>

#ifdef UTM_WIN
#include <Windows.h>
#include <tchar.h>
#endif

#ifndef FTPMODE_NONE
#define FTPMODE_NONE 0
#endif

#ifndef FTPMODE_PASSIVE
#define FTPMODE_PASSIVE 1
#endif

#ifndef FTPMODE_ACTIVE
#define FTPMODE_ACTIVE 2
#endif

#include <addrip_v4.h>

namespace utm {

struct ip_header
{
public:
	ip_header(void);
	~ip_header(void);

	unsigned char			src_mac[6];
	unsigned char			dst_mac[6];
	unsigned char			ip_hl;				// IP header Length (in 32-bits word)
	unsigned char			tcp_hl;				// TCP header Lenght (in 32-bits word)
	unsigned short			version;			// IP version
	unsigned int			length;				// Total length of packet
	unsigned short			identification;
	addrip_v4				src_ip_addr;
	addrip_v4				dst_ip_addr;

	unsigned short			proto;				// Protocol: 6-TCP, 17-UDP, 1-ICMP and etc
	unsigned char			tos;
	unsigned char			ttl;

	unsigned int			seq_num;			// Sequence number (only for TCP)
	unsigned int			ack_num;			// Ack number (only for TCP)
	unsigned short			flags;				// Data offset + Flags
	unsigned short			window;
	unsigned short			src_port;			// Source port (only for TCP or UDP)
	unsigned short			dst_port;			// Destination port (only for TCP or UDP)

	unsigned int			ftpdata_ip;			// IP address for data connection during FTP session
	unsigned short			ftpdata_port;		// TCP port for data connection during FTP session
	unsigned short			ftpdata_mode;		// mode for FTP data connection (passive or active)

	bool parse_raw_buffer(const unsigned char *ippkt);
	void swap_src_dst();
	void compare(const ip_header& rhs, std::string& result);

	void test_fill_packet(int packet_num);
};

}

#endif // UTM_IP_HEADER_H
