#ifndef _UTM_PKT_QUEUE_H
#define _UTM_PKT_QUEUE_H

#pragma once
#include <utm.h>

#include <filter_extra.h>

#define PKTQUEUE_STORED 0
#define PKTQUEUE_NOSPACE 1

#include <boost/thread/mutex.hpp>

namespace utm {

struct pkt_additional_data
{
	HANDLE hAdapterHandle;
	unsigned int nic_alias;
};

class pkt_queue
{
public:
	static const char this_class_name[];

	pkt_queue();
	pkt_queue(const pkt_queue& rhs);
	~pkt_queue();

	pkt_queue& operator=(const pkt_queue& rhs);

	void init(uint32_t maxpackets, size_t packetbuf_size);
	int put_packet(const unsigned char* pkt, size_t packetbuf_size, const pkt_additional_data* ppkt_data);
	bool get_packet(unsigned char* pkt, size_t packetbuf_size, pkt_additional_data* ppkt_data);

	size_t get_packetbuf_size() const { return packetbuf_size; };
	void get_stat(uint64_t& pktput_counter, uint64_t& pktget_counter, uint32_t& current_freeslot) const;

public:
	// variables required for traffic shaper algorithm
	int ticks_left;
	int adjust_speed_value;
	uint32_t adjust_speed_seqnum;

private:
	unsigned char* get_pktposptr(uint32_t pktpos) const;
	size_t get_datasize() const { return (maxpackets * (packetbuf_size + sizeof(pkt_additional_data))); };

	uint64_t pktput_counter;
	uint64_t pktget_counter;
	uint32_t current_freeslot;

	uint32_t maxpackets;
	size_t packetbuf_size;
	unsigned char* data;
	
	boost::mutex guard;

public:
#ifdef UTM_DEBUG
	static void test_all();
#endif
};

}

#endif // _UTM_PKT_QUEUE_H