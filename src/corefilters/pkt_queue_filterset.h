#ifndef _UTM_PKT_QUEUE_FILTERSET_H
#define _UTM_PKT_QUEUE_FILTERSET_H

#pragma once
#include <utm.h>

#include <pkt_queue.h>
#include <filterset.h>
#include <ubase_test.h>
#include <idset.h>

#include <vector>
#include <memory>
#include <boost/thread/mutex.hpp>

#define MAX_FILTERQUEUE_PACKETS 200
#define MAX_FILTERSETQUEUE_PACKETS 1000
#define MAX_FILTERQUEUES 2
#define DIRECTION_SENT 0
#define DIRECTION_RECV 1

namespace utm {

struct pkt_queue_filter
{
	pkt_queue_filter() : filterid(0) { };

	void init(unsigned int maxpackets, size_t packetbuf_size)
	{
		for (unsigned int i = 0; i < MAX_FILTERQUEUES; i++)	{ q[i].init(maxpackets, packetbuf_size); };
	}

	unsigned int filterid;
	pkt_queue q[MAX_FILTERQUEUES];
};

typedef std::vector<pkt_queue_filter> pkt_queue_filterset_container;

class pkt_queue_filterset
{
public:
	static const char this_class_name[];

	pkt_queue_filterset();
	~pkt_queue_filterset();

	void init(const filterset& fs, size_t packetbuf_size);
	int put_packet(unsigned int filter_
		, unsigned int qnumber, const unsigned char* pkt, size_t packetbuf_size, const pkt_additional_data* ppkt_data);
	bool get_packet(unsigned int filter_seq_id, unsigned int qnumber, unsigned char* pkt, size_t packetbuf_size, pkt_additional_data* ppkt_data);

	size_t get_queue_qty() const;
	void get_queue_stat(unsigned int filter_seq_id, unsigned int qnumber, uint64_t& pktput_counter, uint64_t& pktget_counter, uint32_t& current_freeslot) const;
	void get_queue_state(unsigned int filter_seq_id, unsigned int qnumber, int& ticks_left, int& adjust_speed_value, uint32_t& adjust_speed_seqnum) const;
	void set_queue_state(unsigned int filter_seq_id, unsigned int qnumber, int ticks_left, int adjust_speed_value, uint32_t adjust_speed_seqnum);

private:
	void init(size_t packetbuf_size);
	idvector_container filterids;
	std::unique_ptr<pkt_queue_filterset_container> pq;
	boost::mutex guard;

public:
#ifdef UTM_DEBUG
	static void test_all();
#endif
};

}

#endif // _UTM_PKT_QUEUE_FILTERSET_H
