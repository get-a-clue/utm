#include "StdAfx.h"
#include "monitor_pinger.h"
/*
#include "arp_table.h"
#include "rawping.h"

#include <ubase_test.h>

namespace utm {

monitor_pinger::monitor_pinger(void)
{
}


monitor_pinger::~monitor_pinger(void)
{
}

void monitor_pinger::main(monitor_total& mt, std::list<monitor_result>& host_state_changes, const macvendors& mv, SOCKET s, unsigned short pid, bool lookup_arp)
{
	// Одна итерация сериала пингования.
	// Последовательное пингование всех хостов.

	for (range_state_container::iterator seqiter = seqs.begin(); seqiter != seqs.end(); ++seqiter)
	{
		seqiter->second.seq_no = 0;
		seqiter->second.delay_no = 0;
		seqiter->second.serial++;
	}

	monitor_range_container::iterator iter_range;
	for (iter_range = mt.ranges.items.begin(); iter_range != mt.ranges.items.end(); ++iter_range)
	{
		unsigned int range_id = iter_range->id;
		range_state_container::iterator seqiter = seqs.find(range_id);
		if (seqiter == seqs.end())
		{
			range_state rs;
			seqs.insert(std::make_pair(range_id, rs));
		}
	}

	// Последовательно обходим все диапазоны адресов, назначенные для пингования
	
	size_t next_index_position = 0;
	for (iter_range = mt.ranges.items.begin(); iter_range != mt.ranges.items.end(); ++iter_range)
	{
		unsigned int range_id = iter_range->id;
		
		if (!iter_range->active)
			continue;

		range_state_container::iterator seqiter = seqs.find(range_id);
		if (seqiter == seqs.end())
			continue; // Невозможный случай, но все-же проверяем...

		unsigned int start_addr = iter_range->start_addr.m_addr;
		unsigned int end_addr = iter_range->end_addr.m_addr;

		size_t start_index = next_index_position;
		size_t end_index = next_index_position + (end_addr - start_addr);
		next_index_position = end_index + 1;
		
		if (start_addr > end_addr)
			continue;

		for (unsigned int q = seqiter->second.seq_no; q < iter_range->attempts; q++)
		{
			// Сериалов пингов выполняется.
			// Последовательно отсылаем пинги на каждый адрес.

#ifdef _DEBUG
			{
				char tmp[MAXLEN_MSG];
				sprintf_s(tmp, MAXLEN_MSG, "New Ping serial for RangeId=%d, Serial=%d, Attempts=%d, Current_Attempt=%d", iter_range->id, seqiter->second.serial, iter_range->attempts, seqiter->second.seq_no);
				mt.add_event(gstring(tmp), true);
			}
#endif

			unsigned int u = start_addr;
			for(size_t k = start_index; k <= end_index; k++, u++)
			{
				monitor_result mr;
				mt.get_result_record(k, mr);

				if (mr.flags & MONITOR_FLAG_EXCLUDE)
					continue;

				mr.serial = seqiter->second.serial;

				if (seqiter->second.seq_no == 0)
				{
					// Если сброшен счетчик пингов внутри сериала, значит это новый сериал пингования
					// поэтому инициализируем свойства для текущего адреса в списке результатов пингования.

					mr.serial_attempts = iter_range->attempts;
					mr.curr_serial_request_count = 0;
					mr.curr_serial_reply_count = 0;
				}

				// В списке результатов пингования сохраняем значения для последнего отосланного пинга

				std::int64_t tick = monitor_total::get_current_hires_tick();
				mr.curr_tick_request = tick;
				mr.curr_seq_request = LOWORD(seqiter->second.seq_no);

				// Отсылка пинга

				BYTE serial_byte = LOBYTE(LOWORD(seqiter->second.serial));
				WORD ident = MAKEWORD(serial_byte, LOBYTE(LOWORD(seqiter->second.seq_no)));

				mr.curr_serial_request_count++;
				mr.total_ping_request++;
				mt.set_result_record(k, mr);

				if (s != 0)
				{
					addrip_v4 current_addr(u);
					rawping::SendPing(s, pid, ident, current_addr.to_string().c_str(), tick);
				}
//				Sleep(dwDelayBetweenPing);
			}

			seqiter->second.seq_no++;
		}

		{
			// Сериал пингов закончился. Выполняем анализ результатов пингов.

#ifdef _DEBUG
			{
				char tmp[MAXLEN_MSG];
				sprintf_s(tmp, MAXLEN_MSG, "Analyzing results for RangeId=%d", iter_range->id);
				mt.add_event(gstring(tmp), true);
			}
#endif

			arp_table arp;
	
			if (lookup_arp)
				arp.fill();

			for (size_t m = start_index; m <= end_index; m++)
			{
				monitor_result mr;
				mt.get_result_record(m, mr);

				if (mr.flags & MONITOR_FLAG_EXCLUDE)
					continue;

				unsigned int prev_status = mr.status;
				monitor_result::analyze_and_set_new_status(mr, (iter_range->rangetype > 0));
				bool status_changed = prev_status != mr.status;
				bool no_message = (prev_status == monitor_result::PING_STATUS_UNKNOWN) && (mr.status == monitor_result::PING_STATUS_OFFLINE);

				gstring monitoring_msg;

				if (status_changed && !no_message)
				{
					mr.action_type = 0;
					mt.format_monitoring_message(mr, mv, monitoring_msg);
					// добавить событие в очередь для БД
				}

				addrip_v4 ip(mr.addr);
				addrmac mac_from_arp;
				bool macfound = arp.findmac(ip, mac_from_arp);

				if (macfound)
				{
					if ((!(mr.mac == mac_from_arp)) && (status_changed))
					{
						// Мак изменился и измененился статус хоста
#ifdef _DEBUG
						{
							char tmp[MAXLEN_MSG];
							sprintf_s(tmp, MAXLEN_MSG, "New mac with change status for %s, mac=%s", ip.to_string().c_str(), mac_from_arp.to_string().c_str());
							mt.add_event(gstring(tmp), true);
						}
#endif
						mr.action_type = 1;
						mr.mac = mac_from_arp;
						mt.format_monitoring_message(mr, mv, monitoring_msg);
						// добавить событие в очередь для БД
					}
				}
				else
				{
					if (!mr.mac.is_zero())
					{
#ifdef _DEBUG
						{
							char tmp[MAXLEN_MSG];
							sprintf_s(tmp, MAXLEN_MSG, "Remove mac for %s", ip.to_string().c_str());
							mt.add_event(gstring(tmp), true);
						}
#endif

						mr.action_type = 2;
						mt.format_monitoring_message(mr, mv, monitoring_msg);
						// добавить событие в очередь для БД
					}
				}
				mr.mac = mac_from_arp;

				mr.last_serial_request_count = mr.curr_serial_request_count;
				mr.last_serial_reply_count = mr.curr_serial_reply_count;

				mt.set_result_record(m, mr);

				if (!monitoring_msg.empty())
				{
					mt.add_event(monitoring_msg, false);
					host_state_changes.push_back(mr);
				}
			}

			seqiter->second.seq_no++;
		}

		if (seqiter->second.seq_no > (unsigned int)iter_range->attempts)
		{
			// Сериал пингов завершился.
			// Выдерживаем задержку до начала следующего сериала пингования.

			seqiter->second.delay_no++;

			if (seqiter->second.delay_no >= (UINT)iter_range->poll_interval)
			{
				// Задержка окончилась, начинаем новый сериал пингов

//				seqiter->second.serial++;
				seqiter->second.seq_no = 0;
				seqiter->second.delay_no = 0;
			}
		}
	}
}


void monitor_pinger::test_all()
{
	utm::monitor_range range;
	range.test_fillparams(1);

	utm::monitor_range_list rangelist;
	rangelist.add_element(range);

	utm::monitor_total total;
	total.init_from_ranges(rangelist);
	total.add_event = monitor_pinger::test_print_event;

	utm::monitor_pinger pinger;
	utm::macvendors mv;
	mv.load_from_file(gstring("c:\\tmp\\oui.txt"));

	std::list<utm::monitor_result> r1;
	pinger.main(total, r1, mv, 0, 0, true);

	utm::monitor_minidump mini;
	total.extract_minidump(mini, 0);

	TEST_CASE_CHECK(mini.dumpdata.size(), size_t(255));
	monitor_miniresult_container::iterator iter;
	for (iter = mini.dumpdata.begin(); iter != mini.dumpdata.end(); ++iter)
	{
		TEST_CASE_CHECK(iter->second.nStatus, int(monitor_result::PING_STATUS_OFFLINE));
	}

	std::list<utm::monitor_result> r2;
	pinger.main(total, r2, mv, 0, 0, true);

	std::list<utm::monitor_result> r3;
	pinger.main(total, r3, mv, 0, 0, false);

	std::list<utm::monitor_result> r4;
	pinger.main(total, r4, mv, 0, 0, false);

	return;
}

}
*/