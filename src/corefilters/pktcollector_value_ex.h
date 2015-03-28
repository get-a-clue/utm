#ifndef _PKTCOLLECTOR_VALUE_EX_H
#define _PKTCOLLECTOR_VALUE_EX_H

#pragma once

#include "pktcollector_value.h"

#define HTTP_FLAG 1
#define FIRST_PACKET_DIRECTION_BIT 2
#define HAS_SYN_FLAG 4
#define IS_HTTP_REQUEST 8
#define IS_HTTP_REQUEST_FINISH 16
#define TCP_FLAG_FIN 32

#include <cstdint>
#include <string>

#define HTTP_METHOD_UNKNOWN 0
#define HTTP_METHOD_GET 1
#define HTTP_METHOD_POST 2
#define HTTP_METHOD_HEAD 3
#define HTTP_METHOD_PUT 4
#define HTTP_METHOD_OPTIONS 5

namespace utm {

class pktcollector_value_ex : public pktcollector_value
{
public:
	pktcollector_value_ex();
	~pktcollector_value_ex();

	bool process_raw_data(const ip_header& iphdr, int direction, unsigned int now, const unsigned char* rawdata, size_t rawdata_len);
	const char *get_methodname() const;

	void first_http_header_packet(const unsigned char *rawdata, size_t rawdata_len);
	void next_http_header_packet(const unsigned char *rawdata, size_t rawdata_len);
	bool is_http_request_begin(const unsigned char* rawdata, size_t rawdata_len);
	size_t lookup_http_request_end(const unsigned char* rawdata, size_t rawdata_len);
	void lookup_hostname();

	std::uint32_t sent_packets;
	std::uint32_t recv_packets;
	int request_type;
	int end_type;
	int method;
	std::string raw;
	std::string hostname;
//	std::string uri;
};

}

#endif // _PKTCOLLECTOR_VALUE_EX_H