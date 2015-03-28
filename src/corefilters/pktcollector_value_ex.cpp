#include "stdafx.h"
#include "pktcollector_value_ex.h"

namespace utm {

pktcollector_value_ex::pktcollector_value_ex()
{
	flags |= HTTP_FLAG;
	sent_packets = 0;
	recv_packets = 0;
	request_type = 11;
	end_type = 1;
	method = HTTP_METHOD_UNKNOWN;
}


pktcollector_value_ex::~pktcollector_value_ex()
{
}

const char* pktcollector_value_ex::get_methodname() const
{
	switch (method)
	{
		case HTTP_METHOD_GET: return "GET";
		case HTTP_METHOD_POST: return "POST";
		case HTTP_METHOD_HEAD: return "HEAD";
		case HTTP_METHOD_PUT: return "PUT";
		case HTTP_METHOD_OPTIONS: return "OPTIONS";
	}

	return "UNKNOWN";
}

bool pktcollector_value_ex::process_raw_data(const ip_header& iphdr, int direction, unsigned int now, const unsigned char* rawdata, size_t rawdata_len)
{
	bool retval = false;

	if (iphdr.flags & 0x0001)
	{
		flags |= TCP_FLAG_FIN;
	}

	if (direction == 0)
	{
		sent_packets++;
		if ((flags & FIRST_PACKET_DIRECTION_BIT) == 0)
		{
			if (sent_packets == 3)
			{
				// detect HTTP request here
				first_http_header_packet(rawdata, rawdata_len); 
				if ((flags & IS_HTTP_REQUEST) == IS_HTTP_REQUEST)
				{
					retval = true;
				}
			}
			else if (sent_packets > 3)
			{
				if (((flags & IS_HTTP_REQUEST) == IS_HTTP_REQUEST) &&
					((flags & IS_HTTP_REQUEST_FINISH) == 0))
				{
					next_http_header_packet(rawdata, rawdata_len);
					retval = true;
				}
			}
			else
			{
				retval = true;
			}
		}
		else
		{
			retval = true;
		}
	}
	else
	{
		if ((sent_packets == 0) && (recv_packets == 0))
		{
			flags |= FIRST_PACKET_DIRECTION_BIT;
		}

		recv_packets++;
		if ((flags & FIRST_PACKET_DIRECTION_BIT) == FIRST_PACKET_DIRECTION_BIT)
		{
			if (recv_packets == 3)
			{
				// detect HTTP request here
				first_http_header_packet(rawdata, rawdata_len);
				if ((flags & IS_HTTP_REQUEST) == IS_HTTP_REQUEST)
				{
					retval = true;
				}
			}
			else if (recv_packets > 3)
			{
				if (((flags & IS_HTTP_REQUEST) == IS_HTTP_REQUEST) &&
					((flags & IS_HTTP_REQUEST_FINISH) == 0)) 
				{
					next_http_header_packet(rawdata, rawdata_len);
					retval = true;
				}
			}
			else 
			{
				retval = true;
			}
		}
		else
		{
			retval = true;
		}
	}

	return retval;
}


void pktcollector_value_ex::first_http_header_packet(const unsigned char *rawdata, size_t rawdata_len)
{
	if (is_http_request_begin(rawdata, rawdata_len))
	{
		flags |= IS_HTTP_REQUEST;
		size_t endpos = lookup_http_request_end(rawdata, rawdata_len);
		size_t btc = endpos == std::string::npos ? rawdata_len : endpos;
		raw.clear();
		raw.append((const char*)rawdata, btc);

		if (endpos != std::string::npos)
		{
			flags |= IS_HTTP_REQUEST_FINISH;
			lookup_hostname();
		}
	}
}

void pktcollector_value_ex::next_http_header_packet(const unsigned char *rawdata, size_t rawdata_len)
{
	size_t sz = raw.size();
	if (sz >= 8192)
	{
		return;
	}

	size_t cap = raw.capacity();
	if ((sz + rawdata_len) > cap)
	{
		raw.reserve(sz + rawdata_len);
	}

	const char *pt = (const char *)rawdata;
	const char *p;
	char c;
	for (size_t t = 0; t < rawdata_len; t++, pt++)
	{
		c = *pt;
		raw.push_back(c);
		sz++;
		if ((c == '\r') || (c == '\n'))
		{
			p = raw.c_str();
			if (((*(p + sz - 4)) == '\r') &&
				((*(p + sz - 3)) == '\n') &&
				((*(p + sz - 2)) == '\r') &&
				((*(p + sz - 1)) == '\n'))
			{
				flags |= IS_HTTP_REQUEST_FINISH;
				lookup_hostname();
				break;
			}
		}
	}
}

bool pktcollector_value_ex::is_http_request_begin(const unsigned char* rawdata, size_t rawdata_len)
{
	if ((rawdata == NULL) || (rawdata_len < 8))
	{
		return false;
	}

	const char *p = (const char*)(rawdata);

	if (strncmp(p, "GET ", 4) == 0)
	{
		method = HTTP_METHOD_GET;
		return true;
	}

	if (strncmp(p, "POST ", 5) == 0)
	{
		method = HTTP_METHOD_POST;
		return true;
	}

	if (strncmp(p, "PUT ", 4) == 0)
	{
		method = HTTP_METHOD_PUT;
		return true;
	}

	if (strncmp(p, "HEAD ", 5) == 0)
	{
		method = HTTP_METHOD_HEAD;
		return true;
	}

	return false;
}

size_t pktcollector_value_ex::lookup_http_request_end(const unsigned char* rawdata, size_t rawdata_len)
{
	size_t r = 0;
	const char *p = (const char*)(rawdata);

	while ((r + 3) < rawdata_len)
	{
		if (*p == '\r')
		{
			if ((*(p + 1) == '\n') && (*(p + 2) == '\r') && (*(p + 3) == '\n'))
			{
				return r + 3;
			}
		}

		r++;
		p++;
	}

	return std::string::npos;
}

void pktcollector_value_ex::lookup_hostname()
{
	size_t r = 0;
	const char *p = raw.c_str();
	size_t rawlen = raw.size();

	const char *host = NULL;
	size_t hostlen = 0;

	while ((r + 8) < rawlen)
	{
		if (*p == '\r')
		{
			if (host == NULL)
			{
				if ((*(p + 1) == '\n') && (*(p + 2) == 'H') && (*(p + 3) == 'o') && (*(p + 4) == 's') && (*(p + 5) == 't') && (*(p + 6) == ':') && (*(p + 7) == ' '))
				{
					host = p + 8;
				}
			}
			else
			{
				if (*(p + 1) == '\n')
				{
					hostlen = p - host;
					break;
				}
			}
		}

		r++;
		p++;
	}

	if ((host != NULL) && (hostlen > 0))
	{
		hostname.assign(host, hostlen);
	}
}


}