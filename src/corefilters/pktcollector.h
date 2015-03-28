#ifndef _PKTCOLLECTOR_H
#define _PKTCOLLECTOR_H

#pragma once

#define MAX_PRIVELEGED_PORT 1023
#define MAX_UNPRIVELEGED_PORT 65535
#define MAX_PACKETS 100000
#define COLLECTOR_FLUSH_TIMEOUT 600

#include <map>
#include <list>
#include <boost/thread/mutex.hpp>

#include <pktcollector_key.h>
#include <pktcollector_value.h>
#include <pktcollector_value_ex.h>
#include <ip_header.h>

namespace utm {

template<class T>
class pktcollector
{
	typedef std::map<pktcollector_key, T> pcdata_container;
	typedef std::pair<pktcollector_key, T> flush_record;
	typedef std::list<flush_record> flush_container;

public:
	pktcollector() { };
	pktcollector(const pktcollector& rhs) 
	{ 
		data = rhs.data;
	};
	
	~pktcollector() { };

	pktcollector& operator=(const pktcollector& rhs) 
	{ 
		data = rhs.data;
		return *this;
	};

	size_t size() const
	{
		boost::mutex::scoped_lock lock(guard);
		return data.size();
	}

	void put_packet(const ip_header& iphdr, int direction, unsigned int now, const unsigned char* rawdata, size_t rawdata_len)
	{
		boost::mutex::scoped_lock lock(guard);
		put_packet_action(iphdr, direction, now, rawdata, rawdata_len);
	}

	void flush(flush_container& fc, unsigned int now)
	{
		boost::mutex::scoped_lock lock(guard);
		flush_action(fc, now);
	}

private:
	void flush_action(flush_container& fc, unsigned int now)
	{
		if (now < COLLECTOR_FLUSH_TIMEOUT)
		{
			now = 0;
		}
		else
		{
			now = now - COLLECTOR_FLUSH_TIMEOUT;
		}

		bool is_http_connection_closed;
		pktcollector_value *pkv;

		for (auto iter = data.begin(); iter != data.end();)
		{
			T& v = iter->second;
			pkv = reinterpret_cast<pktcollector_value *>(&v);
			is_http_connection_closed = ((pkv->flags & TCP_FLAG_FIN) == TCP_FLAG_FIN);

			if (((pkv->flags == 0) && ((pkv->recv_flush > 0) || (pkv->sent_flush > 0))) ||
				(is_http_connection_closed))
			{
				fc.push_back(std::make_pair(iter->first, v));
				pkv->sent_flush = 0;
				pkv->recv_flush = 0;
			}

			if ((pkv->last_update < now) || is_http_connection_closed)
			{
				iter = data.erase(iter);
				continue;
			}
			++iter;
		}
	}

	void put_packet_action(const ip_header& iphdr, int direction, unsigned int now, const unsigned char* rawdata, size_t rawdata_len)
	{
		unsigned short src_port = iphdr.src_port;
		unsigned short dst_port = iphdr.dst_port;
		bool has_syn = ((iphdr.proto == 6) && ((iphdr.flags & 0x0012) == 0x0002));

		if (rawdata == NULL)
		{
			if ((iphdr.proto == 6) || (iphdr.proto == 17))
			{
				if ((src_port <= MAX_PRIVELEGED_PORT) && (dst_port > MAX_PRIVELEGED_PORT))
				{
					dst_port = MAX_UNPRIVELEGED_PORT;
				}
				else if ((src_port > MAX_PRIVELEGED_PORT) && (dst_port <= MAX_PRIVELEGED_PORT))
				{
					src_port = MAX_UNPRIVELEGED_PORT;
				}
			}

			if (has_syn)
			{
				if (direction == 0)
				{
					src_port = MAX_UNPRIVELEGED_PORT;
				}
				else
				{
					dst_port = MAX_UNPRIVELEGED_PORT;
				}
			}
		}

		pktcollector_key key;
		key.proto = iphdr.proto;
		key.src_port = (direction == 0) ? src_port : dst_port;
		key.dst_port = (direction == 0) ? dst_port : src_port;
		key.src_addr = (direction == 0) ? iphdr.src_ip_addr.m_addr : iphdr.dst_ip_addr.m_addr;
		key.dst_addr = (direction == 0) ? iphdr.dst_ip_addr.m_addr : iphdr.src_ip_addr.m_addr;

		auto iter = data.find(key);
		if (iter == data.end())
		{
			if ((data.size() < MAX_PACKETS) && ((rawdata == NULL) || (has_syn && (rawdata != NULL))))
			{
				T vv;
				pktcollector_value *pkv = reinterpret_cast<pktcollector_value *>(&vv);

				pkv->last_update = now;
				if (direction == 0)
				{
					pkv->sent_fixed += iphdr.length;
					pkv->sent_flush += iphdr.length;
				}
				else
				{
					pkv->recv_fixed += iphdr.length;
					pkv->recv_flush += iphdr.length;
				}

				if (rawdata != NULL)
				{
					pktcollector_value_ex *pkvex = reinterpret_cast<pktcollector_value_ex *>(pkv);
					pkvex->process_raw_data(iphdr, direction, now, rawdata, rawdata_len);
				}
				data.insert(std::make_pair(key, vv));
			}
		}
		else
		{
			T& vv = iter->second;
			pktcollector_value *pkv = reinterpret_cast<pktcollector_value *>(&vv);

			pkv->last_update = now;
			if (direction == 0)
			{
				pkv->sent_fixed += iphdr.length;
				pkv->sent_flush += iphdr.length;
			}
			else
			{
				pkv->recv_fixed += iphdr.length;
				pkv->recv_flush += iphdr.length;
			}

			if (rawdata != NULL)
			{
				pktcollector_value_ex *pkvex = reinterpret_cast<pktcollector_value_ex *>(pkv);
				pkvex->process_raw_data(iphdr, direction, now, rawdata, rawdata_len);
			}
		}
	}

	pcdata_container data;
	mutable boost::mutex guard;
};

}

#endif // _PKTCOLLECTOR_H 