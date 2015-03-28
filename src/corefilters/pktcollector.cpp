#include "stdafx.h"
#include "pktcollector.h"

#include <ubase_test.h>

namespace utm {

/*
const char pktcollector::this_class_name[] = "pktcollector";

pktcollector::pktcollector()
{
}

pktcollector::pktcollector(const pktcollector& rhs)
{
	data = rhs.data;
}

pktcollector::~pktcollector()
{
}

pktcollector& pktcollector::operator=(const pktcollector& rhs)
{
	data = rhs.data;
	return *this;
}

void pktcollector::put_packet(const ip_header& iphdr, int direction, unsigned int now, const unsigned char* rawdata, size_t rawdata_len)
{
	boost::mutex::scoped_lock lock(guard);
	put_packet_action(iphdr, direction, now, rawdata, rawdata_len);
}

void pktcollector::put_packet_action(const ip_header& iphdr, int direction, unsigned int now, const unsigned char* rawdata, size_t rawdata_len)
{
	unsigned short src_port = iphdr.src_port;
	unsigned short dst_port = iphdr.dst_port;
	bool has_syn = false;

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

		if ((iphdr.proto == 6) && ((iphdr.flags & 0x0012) == 0x0002))
		{
			if (direction == 0)
			{
				src_port = MAX_UNPRIVELEGED_PORT;
			}
			else
			{
				dst_port = MAX_UNPRIVELEGED_PORT;
			}
			has_syn = true;
		}
	}

	pktcollector_key key;
	key.proto = iphdr.proto;
	key.src_port = (direction == 0) ? src_port : dst_port;
	key.dst_port = (direction == 0) ? dst_port : src_port;
	key.src_addr = (direction == 0) ? iphdr.src_ip : iphdr.dst_ip;
	key.dst_addr = (direction == 0) ? iphdr.dst_ip : iphdr.src_ip;

	auto iter = data.find(key);
	if (iter == data.end())
	{
		if (data.size() < MAX_PACKETS)
		{
			pktcollector_value v(rawdata != NULL);

			if (has_syn)
			{
//				if (rawdata != NULL)
//				{
//					v.hhcol.flags = HAS_SYN_FLAG;
//				}
			}

			v.last_update = now;
			if (direction == 0)
			{
				v.sent_fixed += iphdr.length;
				v.sent_flush += iphdr.length;
//				if (rawdata != NULL)
//				{
//					v.hhcol.sent_packets++;
//				}
			}
			else
			{
				v.recv_fixed += iphdr.length;
				v.recv_flush += iphdr.length;
//				if (rawdata != NULL)
//				{
//					v.hhcol.flags |= FIRST_PACKET_DIRECTION_BIT;
//					v.hhcol.recv_packets++;
//				}
			}

			if (rawdata != NULL)
			{
				v.process_first_raw_data(has_syn, direction);
			}
			data.insert(std::make_pair(key, v));
		}
	}
	else
	{
		pktcollector_value& v = iter->second;
		v.last_update = now;
		if (direction == 0)
		{
			v.sent_fixed += iphdr.length;
			v.sent_flush += iphdr.length;
		}
		else
		{
			v.recv_fixed += iphdr.length;
			v.recv_flush += iphdr.length;
		}

		if (rawdata != NULL)
		{
			v.process_raw_data(iphdr, direction, now, rawdata, rawdata_len);
		}
	}
}

void pktcollector::flush(flush_container& fc, unsigned int now)
{
	boost::mutex::scoped_lock lock(guard);
	flush_action(fc, now);
}

void pktcollector::flush_action(flush_container& fc, unsigned int now)
{
	unsigned int threshold = now - 600;

	for (auto iter = data.begin(); iter != data.end(); )
	{
		pktcollector_value& value = iter->second;

		if ((value.recv_flush > 0) || (value.sent_flush > 0))
		{
			pktcollector_key copy_key(iter->first);
			pktcollector_value copy_value(iter->second);
			fc.push_back(std::make_pair(copy_key, copy_value));

			value.sent_flush = 0;
			value.recv_flush = 0;
		}
		else
		{
			if (value.last_update < threshold)
			{
				iter = data.erase(iter);
				continue;
			}
		}
		++iter;
	}
}
*/

/*
#ifdef UTM_DEBUG
void pktcollector::test_all()
{
	test_report tr(this_class_name);

	pktcollector_value v;
	pktcollector_key pkey;
	pktcollector_key pkey2;

	TEST_CASE_CHECK(false, pkey < pkey2);
	TEST_CASE_CHECK(false, pkey2 < pkey);

	pcdata_container pcdata;
	pcdata.insert(std::make_pair(pkey, v));
	pcdata.insert(std::make_pair(pkey2, v));

	pkey.proto = 6;
	pkey2.proto = 17;

	TEST_CASE_CHECK(true, pkey < pkey2);
	TEST_CASE_CHECK(false, pkey2 < pkey);

	pkey2.proto = 6;
	pkey.dst_port = 80;
	pkey2.dst_port = 8000;

//	TEST_CASE_CHECK(true, pkey < pkey2);
//	TEST_CASE_CHECK(false, pkey2 < pkey);

}
#endif
*/
}
