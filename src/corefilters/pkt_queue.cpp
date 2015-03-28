#include "stdafx.h"
#include "pkt_queue.h"

#include <ubase_test.h>

namespace utm {

const char pkt_queue::this_class_name[] = "pkt_queue";

pkt_queue::pkt_queue() : pktput_counter(0), pktget_counter(0), current_freeslot(0), 
	ticks_left(0), adjust_speed_value(0), adjust_speed_seqnum(0),
	maxpackets(0), packetbuf_size(0), data(NULL)
{
}

pkt_queue::pkt_queue(const pkt_queue& rhs)
{
	operator=(rhs);
}

pkt_queue::~pkt_queue()
{
	if (data != NULL)
		delete[] data;
}

pkt_queue& pkt_queue::operator=(const pkt_queue& rhs)
{
	pktput_counter = rhs.pktput_counter;
	pktget_counter = rhs.pktget_counter;
	current_freeslot = rhs.current_freeslot;
	ticks_left = rhs.ticks_left;
	adjust_speed_value = rhs.adjust_speed_value;
	adjust_speed_seqnum = rhs.adjust_speed_seqnum;
	maxpackets = rhs.maxpackets;
	packetbuf_size = rhs.packetbuf_size;
	data = new unsigned char[get_datasize()];
	memcpy(data, rhs.data, get_datasize());

	return *this;
}

void pkt_queue::init(size_t maxpackets, size_t packetbuf_size)
{
	this->maxpackets = maxpackets;
	this->packetbuf_size = packetbuf_size;

	data = new unsigned char[get_datasize()];
}

int pkt_queue::put_packet(const unsigned char* pkt, size_t packetbuf_size, const pkt_additional_data* ppkt_data)
{
	boost::mutex::scoped_lock lock(guard);

	uint32_t qsize = static_cast<uint32_t>(pktput_counter - pktget_counter);
	if (qsize >= maxpackets)
		return PKTQUEUE_NOSPACE;

	unsigned char* ptr = get_pktposptr(current_freeslot);
	if (ptr == NULL)
		return PKTQUEUE_NOSPACE;

	memcpy(ptr, pkt, packetbuf_size);

	if (ppkt_data != NULL)
		memcpy(ptr + packetbuf_size, ppkt_data, sizeof(pkt_additional_data));

	pktput_counter++;
	current_freeslot++;
	if (current_freeslot >= maxpackets)
		current_freeslot = 0;

	return PKTQUEUE_STORED;
}

bool pkt_queue::get_packet(unsigned char* pkt, size_t packetbuf_size, pkt_additional_data* ppkt_data)
{
	boost::mutex::scoped_lock lock(guard);

	uint32_t qsize = static_cast<uint32_t>(pktput_counter - pktget_counter);
	if (qsize == 0)
		return false;

	if (ticks_left > 0)
	{
		ticks_left--;
		return false;
	};

	uint32_t lastpos = ((maxpackets + current_freeslot - qsize) % maxpackets);
	unsigned char* ptr = get_pktposptr(lastpos);
	if (ptr == NULL)
		return false;

	memcpy(pkt, ptr, packetbuf_size);
	memcpy(ppkt_data, ptr + packetbuf_size, sizeof(pkt_additional_data));

	pktget_counter++;
	return true;
}

unsigned char* pkt_queue::get_pktposptr(uint32_t pktpos) const
{
	if (pktpos >= maxpackets)
		return NULL;

	if (data == NULL)
		return NULL;

	return (data + (pktpos * (packetbuf_size + sizeof(pkt_additional_data))));
}

void pkt_queue::get_stat(uint64_t& pktput_counter, uint64_t& pktget_counter, uint32_t& current_freeslot) const
{
	pktput_counter = this->pktput_counter;
	pktget_counter = this->pktget_counter;
	current_freeslot = this->current_freeslot;
}

#ifdef UTM_DEBUG
void pkt_queue::test_all()
{
	test_report tr(this_class_name);

	static const size_t MAXPKT = 5;
	static const size_t PKTBUFSIZE = 1514;

	pkt_queue pq;
	pq.init(MAXPKT, PKTBUFSIZE);

	unsigned char pktbuf[PKTBUFSIZE];
	pkt_additional_data data;

	for (size_t i = 0; i < (MAXPKT * 2); i++)
	{
		memset(&pktbuf[0], 0, PKTBUFSIZE);
		pktbuf[0] = static_cast<unsigned char>(i);

		int res = pq.put_packet(&pktbuf[0], PKTBUFSIZE, &data);
		
		if (i < MAXPKT)
		{
			TEST_CASE_CHECK(res, int(PKTQUEUE_STORED));
		}
		else
		{
			TEST_CASE_CHECK(res, int(PKTQUEUE_NOSPACE));
		}
	}

	{
		uint64_t pktput_counter = 0;
		uint64_t pktget_counter = 0;
		uint32_t current_freeslot = 0;
		pq.get_stat(pktput_counter, pktget_counter, current_freeslot);

		TEST_CASE_CHECK(pktput_counter, uint64_t(MAXPKT));
		TEST_CASE_CHECK(pktget_counter, uint64_t(0));
		TEST_CASE_CHECK(current_freeslot, uint32_t(0));
	}

	for (size_t j = 0; j < MAXPKT; j++)
	{
		memset(&pktbuf[0], 0, PKTBUFSIZE);
		bool res = pq.get_packet(&pktbuf[0], PKTBUFSIZE, &data);

		TEST_CASE_CHECK(res, true);
		TEST_CASE_CHECK(pktbuf[0], static_cast<unsigned char>(j));
		TEST_CASE_CHECK(pktbuf[1], unsigned char(0));
	}

	bool res = pq.get_packet(&pktbuf[0], PKTBUFSIZE, &data);
	TEST_CASE_CHECK(res, false);

	{
		uint64_t pktput_counter = 0;
		uint64_t pktget_counter = 0;
		uint32_t current_freeslot = 0;
		pq.get_stat(pktput_counter, pktget_counter, current_freeslot);

		TEST_CASE_CHECK(pktput_counter, uint64_t(MAXPKT));
		TEST_CASE_CHECK(pktget_counter, uint64_t(MAXPKT));
		TEST_CASE_CHECK(current_freeslot, uint32_t(0));
	}
}
#endif

}