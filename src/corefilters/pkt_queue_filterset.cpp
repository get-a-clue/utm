#include "stdafx.h"
#include "pkt_queue_filterset.h"

namespace utm {

const char pkt_queue_filterset::this_class_name[] = "pkt_queue_filterset";

pkt_queue_filterset::pkt_queue_filterset()
{
}

pkt_queue_filterset::~pkt_queue_filterset()
{
}

void pkt_queue_filterset::init(size_t packetbuf_size)
{
	boost::mutex::scoped_lock lock(guard);

	pkt_queue_filterset_container* ppq = new pkt_queue_filterset_container();

	pkt_queue_filter pqf;
	ppq->push_back(pqf);
	pkt_queue_filter& refpqf = ppq->at(0);
	refpqf.init(MAX_FILTERSETQUEUE_PACKETS, packetbuf_size);

	pq.reset(ppq);
}

void pkt_queue_filterset::init(const filterset& fs, size_t packetbuf_size)
{
	boost::mutex::scoped_lock lock(guard);

	idvector_container ids;
	for (auto iter = fs.filters.items.begin(); iter != fs.filters.items.end(); ++iter)
	{
		ids.push_back(iter->m_id);
	}

	if (filterids == ids)
	{
		// the filterset was not modified.
		return;
	}

	pkt_queue_filterset_container* ppq = new pkt_queue_filterset_container();

	for (auto iter = fs.filters.items.begin(); iter != fs.filters.items.end(); ++iter)
	{
		pkt_queue_filter pqf;
		ppq->push_back(pqf);
		size_t lastpos = ppq->size() - 1;
		pkt_queue_filter& refpqf = ppq->at(lastpos);

		unsigned int filterid = iter->m_id;

		bool found = false;
		if (pq.get() != NULL)
		{
			for (auto itex = pq.get()->begin(); itex != pq.get()->end(); ++itex)
			{
				if (itex->filterid == filterid)
				{
					refpqf = (*itex);
					found = true;
					break;
				}
			}
		}

		if (!found)
		{
			refpqf.filterid = filterid;
			refpqf.init(MAX_FILTERQUEUE_PACKETS, packetbuf_size);
		}
	}

	filterids = ids;
	pq.reset(ppq);
}

int pkt_queue_filterset::put_packet(unsigned int filter_seq_id, unsigned int qnumber, const unsigned char* pkt, size_t packetbuf_size, const pkt_additional_data* ppkt_data)
{
	if (qnumber >= MAX_FILTERQUEUES)
		throw std::exception("Invalid queue number");

	if (pq.get() == NULL)
	{
		init(packetbuf_size);
	}

	if ((pq.get()->size() > filter_seq_id))
	{
		return pq.get()->at(filter_seq_id).q[qnumber].put_packet(pkt, packetbuf_size, ppkt_data);
	}

	return PKTQUEUE_NOSPACE;
}

bool pkt_queue_filterset::get_packet(unsigned int filter_seq_id, unsigned int qnumber, unsigned char* pkt, size_t packetbuf_size, pkt_additional_data* ppkt_data)
{
	if (qnumber >= MAX_FILTERQUEUES)
		throw std::exception("Invalid queue number");

	bool retval = false;

	{
		boost::mutex::scoped_lock lock(guard);
		if ((pq.get() != NULL) && (pq.get()->size() > filter_seq_id))
		{
			retval = pq.get()->at(filter_seq_id).q[qnumber].get_packet(pkt, packetbuf_size, ppkt_data);
		}
	}

	return retval;
}

void pkt_queue_filterset::get_queue_stat(unsigned int filter_seq_id, unsigned int qnumber, uint64_t& pktput_counter, uint64_t& pktget_counter, uint32_t& current_freeslot) const
{
	if (qnumber >= MAX_FILTERQUEUES)
		throw std::exception("Invalid queue number");

	if (pq.get() != NULL)
	{
		pkt_queue_filter& p = pq.get()->at(filter_seq_id);
		p.q[qnumber].get_stat(pktput_counter, pktget_counter, current_freeslot);
	}
}

void pkt_queue_filterset::get_queue_state(unsigned int filter_seq_id, unsigned int qnumber, int& ticks_left, int& adjust_speed_value, uint32_t& adjust_speed_seqnum) const
{
	if (qnumber >= MAX_FILTERQUEUES)
		throw std::exception("Invalid queue number");

	if (pq.get() != NULL)
	{
		pkt_queue_filter& p = pq.get()->at(filter_seq_id);
		ticks_left = p.q[qnumber].ticks_left;
		adjust_speed_value = p.q[qnumber].adjust_speed_value;
		adjust_speed_seqnum = p.q[qnumber].adjust_speed_seqnum;
	}
}

void pkt_queue_filterset::set_queue_state(unsigned int filter_seq_id, unsigned int qnumber, int ticks_left, int adjust_speed_value, uint32_t adjust_speed_seqnum)
{
	if (qnumber >= MAX_FILTERQUEUES)
		throw std::exception("Invalid queue number");

	if (pq.get() != NULL)
	{
		pkt_queue_filter& p = pq.get()->at(filter_seq_id);
		p.q[qnumber].ticks_left = ticks_left;
		p.q[qnumber].adjust_speed_value = adjust_speed_value;
		p.q[qnumber].adjust_speed_seqnum = adjust_speed_seqnum;
	}
}

size_t pkt_queue_filterset::get_queue_qty() const
{
	if (pq.get() != NULL)
	{
		return pq.get()->size();
	}

	return 0;
}

#ifdef UTM_DEBUG
void pkt_queue_filterset::test_all()
{
	test_report tr(this_class_name);

	pkt_queue_filterset pkf;
	pkf.init(1514);
	TEST_CASE_CHECK(pkf.get_queue_qty(), size_t(1));
}
#endif

}