#include "StdAfx.h"
#include "filter2.h"

#include "hosttable.h"
#include "hostresolver.h"

#include <utime.h>
#include <addrtablemap.h>
#include <addrtablemap_v4.h>
#include <ubase_test.h>

namespace utm {

const char filter2::this_class_name[] = "filter2";

filter2::filter2(void)
{
	last_reset_ts.clear();
	seq_id = 0;
}

filter2::~filter2(void)
{
}

void filter2::copy_filter_counters(const filter2& f)
{
	cnt_sent.copy(f.cnt_sent);
	cnt_recv.copy(f.cnt_recv);
	seq_id = f.seq_id;
	last_reset_ts = f.last_reset_ts;
}

void filter2::reset_filter_counters(bool reset_history)
{
	cnt_sent.reset(reset_history);
	cnt_recv.reset(reset_history);
}

bool filter2::reset_on_schedule(const standard_timeset& stimeset, bool reset_history)
{
	utimestamp tss(unsigned int(0));

	if (m_nResetCnt == RESETCNT_MONTHLY)
	{
		tss.ts = stimeset.start_of_month;
	}
	else if (m_nResetCnt == RESETCNT_DAILY)
	{
		tss.ts = stimeset.start_of_day;
	}
	else if (m_nResetCnt == RESETCNT_HOURLY)
	{
		tss.ts = stimeset.start_of_hour;
	}
	else if (m_nResetCnt == RESETCNT_SUNDAY)
	{
		tss.ts = stimeset.start_of_lastdayweek[0];
	}
	else if (m_nResetCnt == RESETCNT_MONDAY)
	{
		tss.ts = stimeset.start_of_lastdayweek[1];
	}
	else if (m_nResetCnt == RESETCNT_TUESDAY)
	{
		tss.ts = stimeset.start_of_lastdayweek[2];
	}
	else if (m_nResetCnt == RESETCNT_WEDNESDAY)
	{
		tss.ts = stimeset.start_of_lastdayweek[3];
	}
	else if (m_nResetCnt == RESETCNT_THURSDAY)
	{
		tss.ts = stimeset.start_of_lastdayweek[4];
	}
	else if (m_nResetCnt == RESETCNT_FRIDAY)
	{
		tss.ts = stimeset.start_of_lastdayweek[5];
	}
	else if (m_nResetCnt == RESETCNT_SATURDAY)
	{
		tss.ts = stimeset.start_of_lastdayweek[6];
	}

	if (tss.ts > 0)
	{
		if (last_reset_ts < tss)
		{
			reset_filter_counters(reset_history);
			last_reset_ts = stimeset.time_point;
			return true;
		}
	}

	return false;
}

bool filter2::is_traffic_limit_exceeded(int mbytes, bool check_warning_level) const
{
	std::int64_t a = m_nTrafficLimitMb * mbytes;

	if (!check_warning_level)
	{
		if ((m_nTrafficLimitMb == 0) && (!m_bRevers))
			return false;
	}
	else
	{
		if (m_bRevers) return false;
		if (m_nTrafficLimitMb == 0) return false;
		if (m_wTrafficLimitWarnLevel <= 0) return false;
		if (m_wTrafficLimitWarnLevel > 100)	return false;

		a = a * m_wTrafficLimitWarnLevel;
		a = a / 100;
	}

	if (!m_bRevers)
	{
		switch(m_nTrafficLimitType)
		{
			case TRAFFICLIMIT_SENT_PLUS_RECV:	if ((cnt_sent.get_cnt() + cnt_recv.get_cnt()) >= a) return true;
												break;

			case TRAFFICLIMIT_SENT:				if (cnt_sent.get_cnt() >= a) return true;
												break;

			case TRAFFICLIMIT_RECV:				if (cnt_recv.get_cnt() >= a) return true;
												break;

			default:							if ((cnt_sent.get_cnt() >= a) || (cnt_recv.get_cnt() >= a)) return true;
												break;
		};
	}
	else
	{
		if ((m_nTrafficLimitType > 0) && ((cnt_sent.get_cnt() < 0) || (cnt_recv.get_cnt() < 0)))
			return true;
	};

	return false;
}

bool filter2::is_traffic_limitwarn_exceeded(int mbytes) const
{
	if (m_bRevers) return false;
	if (m_nTrafficLimitMb == 0) return false;
	if (m_wTrafficLimitWarnLevel <= 0) return false;
	if (m_wTrafficLimitWarnLevel > 100)	return false;

	std::int64_t a = m_nTrafficLimitMb * mbytes;
	a = a * m_wTrafficLimitWarnLevel;
	a = a / 100;

	switch(m_nTrafficLimitType)
	{
		case TRAFFICLIMIT_SENT_PLUS_RECV:
											if ((cnt_sent.get_cnt() + cnt_recv.get_cnt()) >= a) return true;
											break;

		case TRAFFICLIMIT_SENT:
											if (cnt_sent.get_cnt() >= a) return true;
											break;

		case TRAFFICLIMIT_RECV:
											if (cnt_recv.get_cnt() >= a) return true;
											break;

		default:
											if ((cnt_sent.get_cnt() >= a) || (cnt_recv.get_cnt() >= a)) return true;
											break;
	};

	return false;
}

int filter2::get_actual_speed() const
{
	if (m_nTrafficLimitAction != TRAFFICLIMIT_SPEED2)
		return m_nSpeed;

	if (is_traffic_limit_exceeded())
		return m_nSpeed2;

	return m_nSpeed;
}

void filter2::match_filter(const match_filter_input& data, match_filter_result& result, bool clear_result_before)
{
	if (clear_result_before)
		result.clear();

	match_filter(data, result);
}

void filter2::match_filter(const match_filter_input& data, match_filter_result& result)
{
    if (m_bDisable)
		return;

	{
		// Check for Work Hours for the filter
		unsigned int bit = 1;
		bit = bit << data.lt->tm_hour;

		int week_day = (data.lt->tm_wday <= 0) ? 6 : (data.lt->tm_wday - 1);
		if ((m_nWorkHours[week_day] & bit) == 0)
			return;

		// Check for Work Days for the filter

		unsigned int current_day = (data.lt->tm_year + 1900)*10000 + (data.lt->tm_mon + 1)*100 + data.lt->tm_mday;
		if ((m_nStartDay > current_day) || (m_nEndDay < current_day))
			return;
	}

	bool filter_match = false;
	int rule_number = 1;

//	addrtablemaprec_v4 *src_at, *dst_at;
	utm::addrtablemaprec<utm::addrtable_v4> *src_at, *dst_at;

	unsigned char *psrc_mac, *pdst_mac;
	unsigned int src_ip, dst_ip;
	utm::addrip_v4 addr_src_ip, addr_dst_ip;
	unsigned short src_port, dst_port;
	int curdir;

	bool is_match, is_ftp_data_match, is_spi_match;
	__int64 rule_lim;
	__int64 lim;

	utm::ip_header* ip = data.ip;
	utm::addrip_v4 a;

	const unsigned char* cond_mac_data;
	unsigned int isrc_ip, isrc_mask, idst_ip, idst_mask;
	utm::rule* prule;

	rule_container::iterator iter;
	for (iter = rules.items.begin(); iter != rules.items.end(); ++iter)
	{
        prule = &(*iter);

		isrc_ip = prule->src_ip.m_addr;
		isrc_mask = prule->src_mask.m_addr;
		idst_ip = prule->dst_ip.m_addr;
		idst_mask = prule->dst_mask.m_addr;

		if ((prule->natuse == NATUSE_YES) && (data.bWithNat == false))
			continue;

		if ((prule->natuse == NATUSE_NO) && (data.bWithNat == true))
			continue;

		if ((prule->nicalias == NICALIAS_PUBLIC) || (prule->nicalias == NICALIAS_PRIVATE) || (prule->nicalias == NICALIAS_NETFLOW))
		{
			if (prule->nicalias != data.nNicAlias)
				continue;
		};

		src_at = NULL;
		dst_at = NULL;

		if (data.mat != NULL)
		{

			if ((prule->src_type == RULE_ADDRGRP) || (prule->src_type == RULE_ADDRGRP_NO))
			{
				src_at = data.mat->findptr_by_id(prule->src_atkey);
//				src_at_prule = data.mat->addrtables.find(prule->src_atkey);
//				if (src_at_prule != data.mat->addrtables.end())
//					src_at_found = true;
			};

			if ((prule->dst_type == RULE_ADDRGRP) || (prule->dst_type == RULE_ADDRGRP_NO))
			{
				dst_at = data.mat->findptr_by_id(prule->dst_atkey);
//				dst_at_prule = data.mat->addrtables.find(prule->dst_atkey);
//				if (dst_at_prule != data.mat->addrtables.end())
//					dst_at_found = true;
			};
		}

		is_ftp_data_match = false;
		is_spi_match = false;

		for (curdir = 0; curdir < 2; curdir++)
		{
			// ѕеременна€ is_match - результат проверки на соответствие пакета правилу
			// ≈сли переменна€ is_match станет false, это означает что правило не соответствует пакету.

			is_match = true;

			if (curdir == DIRECTION_FORWARD)
			{
				psrc_mac = &ip->src_mac[0];
				pdst_mac = &ip->dst_mac[0];
				src_ip = ip->src_ip_addr.m_addr;
				dst_ip = ip->dst_ip_addr.m_addr;
				src_port = ip->src_port;
				dst_port = ip->dst_port;
			};

			if (curdir == DIRECTION_BACKWARD)
			{
				if ((prule->mirrored == DIRECTION_ONEWAY) || (prule->pkt_options & PKTOPT_TCPSYN) || (prule->pkt_options & PKTOPT_ICMPECHOREQUEST))
				{
					curdir++;
					break;
				};

				psrc_mac = &ip->dst_mac[0];
				pdst_mac = &ip->src_mac[0];
				src_ip = ip->dst_ip_addr.m_addr;
				dst_ip = ip->src_ip_addr.m_addr;
				src_port = ip->dst_port;
				dst_port = ip->src_port;
			};

			addr_src_ip.m_addr = src_ip;
			addr_dst_ip.m_addr = dst_ip;

			// Check for data channel in FTP protocol

			if ((prule->proto == 6) && (prule->dst_port == 21) && (prule->pkt_options & PKTOPT_FTP))
			{
				if ((src_port != 21) && (dst_port != 21))
				{
/*
					StateStor sis;

					sis.src_ip = src_ip;
					sis.src_port = src_port;
					sis.dst_ip = dst_ip;
					sis.dst_port = dst_port;
					sis.proto = 6;
					sis.type = STATE_STOR_TYPE_FTP;

					if ((*prule).SsFindPkt(&sis, data.nUptime, tcp_flags))
					{
						is_ftp_data_match = true;
						break;
					};
*/
				};
			};

			if ((prule->proto == 6) && (prule->src_port == 21) && (prule->pkt_options & PKTOPT_FTP))
			{
				if ((src_port != 21) && (dst_port != 21))
				{
/*
					StateStor sis;

					sis.src_ip = src_ip;
					sis.src_port = src_port;
					sis.dst_ip = dst_ip;
					sis.dst_port = dst_port;
					sis.proto = 6;
					sis.type = STATE_STOR_TYPE_FTP;

					if ((*prule).SsFindPkt(&sis, data.nUptime, tcp_flags))
					{
						is_ftp_data_match = true;
						break;
					};
*/
				};
			};

			if (curdir == DIRECTION_BACKWARD)
			{
				if (prule->mirrored & DIRECTION_STATEFUL)
				{
/*
					StateStor sis;

					sis.src_ip = src_ip;
					sis.src_port = src_port;
					sis.dst_ip = dst_ip;
					sis.dst_port = dst_port;

					sis.proto = ip->proto;
					sis.type = STATE_STOR_TYPE_NORMAL;

					if ((*prule).SsFindPkt(&sis, data.nUptime, tcp_flags))
					{
						is_spi_match = true;
					}
					else
					{
						// Packet is not matched in the current rule

						curdir++;
					};

					break;
*/
				};
			};

			// ѕровер€ем адрес источника

			switch (prule->src_type)
			{
				case RULE_MYIP:
					is_match = (curdir == DIRECTION_FORWARD) ? (data.nPreCheckAddrTables & CHECKADDR_MYPC_SRC) != 0 : (data.nPreCheckAddrTables & CHECKADDR_MYPC_DST) != 0;
					break;

				case RULE_IP:
					is_match = (src_ip & isrc_mask) == (isrc_ip & isrc_mask);
					break;

				case RULE_RANGE:
					is_match = (src_ip >= isrc_ip) && (src_ip <= isrc_mask);
					break;

				case RULE_LAN:
					is_match = (curdir == DIRECTION_FORWARD) ? (data.nPreCheckAddrTables & CHECKADDR_LAT_SRC) != 0 : (data.nPreCheckAddrTables & CHECKADDR_LAT_DST) != 0;
					break;

				case RULE_WAN:
					is_match = (curdir == DIRECTION_FORWARD) ? (data.nPreCheckAddrTables & CHECKADDR_LAT_SRC) == 0 : (data.nPreCheckAddrTables & CHECKADDR_LAT_DST) == 0;
					break;

				case RULE_MAC:
					is_match = prule->src_mac.is_equal(psrc_mac);
					break;

				case RULE_HOST:
					is_match = (data.host2ip == NULL) ? false : data.host2ip->checkaddr(prule->src_host, addr_src_ip);
					break;

				case RULE_ADDRGRP:
					is_match = (src_at == NULL) ? false : src_at->addrtable.CheckAddrRange(addr_src_ip);
					break;

				case RULE_ADDRGRP_NO:
					is_match = (src_at == NULL) ? false : !(src_at->addrtable.CheckAddrRange(addr_src_ip));
					break;

				case RULE_INCOMING:
					is_match = data.nPacketDirection == PACKET_DIRECTION_INCOMING;
					break;

				case RULE_OUTGOING:
					is_match = data.nPacketDirection != PACKET_DIRECTION_OUTGOING;
					break;

				case RULE_USER:
					is_match = data.user_conn->check_user_and_ip(prule->src_uid, addr_src_ip, data.uptime);
                    break;

				case RULE_USER_ANY:
					is_match = data.user_conn->check_for_any_ip(addr_src_ip, data.uptime);
					break;

				case RULE_PROCNAME:

					if (curdir == DIRECTION_FORWARD)
					{
						if (data.dwProcNickIdSrc == 0)
							is_match = false;
						else
							is_match = !(data.dwProcNickIdSrc != prule->src_procnick);
					}
					else
					{
						if (data.dwProcNickIdDst == 0)
							is_match = false;
						else
							is_match = !(data.dwProcNickIdDst != prule->src_procnick);
					}
					break;

				case RULE_PROCUSER:

					if (curdir == DIRECTION_FORWARD)
					{
						if (data.uidSrc == 0)
							is_match = false;
						else
						{
							is_match = !(data.uidSrc != prule->src_uid);
						}
					}
					else
					{
						if (data.uidDst == 0)
							is_match = false;
						else
						{
							is_match = !(data.uidDst != prule->src_uid);
						}
					}
					break;

				default:
					is_match = false;
					break;
			};

			if (!is_match)
				continue;			// check backward direction or exit

			// ѕровер€ем адрес назначени€

			switch (prule->dst_type)
			{
				case RULE_MYIP:
					is_match = (curdir == DIRECTION_FORWARD) ? (data.nPreCheckAddrTables & CHECKADDR_MYPC_DST) != 0 : (data.nPreCheckAddrTables & CHECKADDR_MYPC_SRC) != 0;
					break;

				case RULE_IP:
					is_match = (dst_ip & idst_mask) == (idst_ip & idst_mask);
					break;

				case RULE_RANGE:
					is_match = (dst_ip >= idst_ip) && (dst_ip <= idst_mask);
					break;

				case RULE_LAN:
					is_match = (curdir == DIRECTION_FORWARD) ? (data.nPreCheckAddrTables & CHECKADDR_LAT_DST) != 0 : (data.nPreCheckAddrTables & CHECKADDR_LAT_SRC) != 0;
					break;

				case RULE_WAN:
					is_match = (curdir == DIRECTION_FORWARD) ? (data.nPreCheckAddrTables & CHECKADDR_LAT_DST) == 0 : (data.nPreCheckAddrTables & CHECKADDR_LAT_SRC) == 0;
					break;

				case RULE_MAC:
					is_match = prule->dst_mac.is_equal(pdst_mac);
					break;

				case RULE_HOST:
					is_match = (data.host2ip == NULL) ? false : data.host2ip->checkaddr(prule->dst_host, addr_dst_ip);
					break;

				case RULE_ADDRGRP:
					is_match = (dst_at == NULL) ? false : dst_at->addrtable.CheckAddrRange(addr_dst_ip);
					break;

				case RULE_ADDRGRP_NO:
					is_match = (dst_at == NULL) ? false : !(dst_at->addrtable.CheckAddrRange(addr_dst_ip));
					break;

				case RULE_INCOMING:
					is_match = !(data.nPacketDirection != PACKET_DIRECTION_INCOMING);
					break;

				case RULE_OUTGOING:
					is_match = !(data.nPacketDirection != PACKET_DIRECTION_OUTGOING);
					break;

                case RULE_USER:
					is_match = data.user_conn->check_user_and_ip(prule->dst_uid, addr_dst_ip, data.uptime);
                    break;

				case RULE_USER_ANY:
					is_match = data.user_conn->check_for_any_ip(addr_dst_ip, data.uptime);
					break;

				case RULE_PROCNAME:
					if (curdir == DIRECTION_FORWARD)
					{
						if (data.dwProcNickIdDst == 0)
							is_match = false;
						else
							is_match = !(data.dwProcNickIdDst != prule->dst_procnick);
					}
					else
					{
						if (data.dwProcNickIdSrc == 0)
							is_match = false;
						else
							is_match = !(data.dwProcNickIdSrc != prule->dst_procnick);
					}
					break;

				case RULE_PROCUSER:
					if (curdir == DIRECTION_FORWARD)
					{
						if (data.uidDst == 0)
							is_match = false;
						else
							is_match = !(data.uidDst != prule->dst_uid);
					}
					else
					{
						if (data.uidSrc == 0)
							is_match = false;
						else
							is_match = !(data.uidSrc != prule->dst_uid);
					}
					break;

				
				default:
					is_match = false;
					break;
			};

			if (!is_match)
				continue;			// check backward direction or exit

			// IP-адреса в правило попали, теперь провер€ем протоколы и порты
			if ((prule->proto > 0))
			{
				// ѕроверка, совпадает ли тип протокола пришедшего пакета
				// и типа протокола указанного в правиле
				
				if (prule->proto != ip->proto)
					continue;

				if ((ip->proto == 6) || (ip->proto == 17))
				{
					// We will check ports for TCP and UDP protocol types

					switch(prule->src_port_type)
					{
						case PORT_ANY:	
									break;

						case PORT_EQUAL:
									is_match = (prule->src_port) == src_port;
									break;

						case PORT_GREATER:
									is_match = src_port > (prule->src_port);
									break;

						case PORT_LESS:
									is_match = src_port < (prule->src_port);
									break;

						case PORT_NOTEQUAL:
									is_match = src_port != (prule->src_port);
									break;

						case PORT_BETWEEN:
									is_match = (src_port >= (prule->src_port)) && (src_port <= (prule->src_port_to));
									break;

						case PORT_NOTBETWEEN:
									is_match = (src_port <= (prule->src_port)) || (src_port >= (prule->src_port_to));
									break;

						case PORT_EQUAL_OR:
									is_match = (src_port == (prule->src_port)) || (src_port == (prule->src_port_to));
									break;
					};

					switch(prule->dst_port_type)
					{
						case PORT_ANY:	
									break;

						case PORT_EQUAL:
									is_match = (prule->dst_port) == dst_port;
									break;

						case PORT_GREATER:
									is_match = dst_port > (prule->dst_port);
									break;

						case PORT_LESS:
									is_match = dst_port < (prule->dst_port);
									break;

						case PORT_NOTEQUAL:
									is_match = dst_port != (prule->dst_port);
									break;

						case PORT_BETWEEN:
									is_match = (dst_port >= (prule->dst_port)) && (dst_port<=(prule->dst_port_to));
									break;

						case PORT_NOTBETWEEN:
									is_match = (dst_port <= (prule->dst_port)) || (dst_port >= (prule->dst_port_to));
									break;

						case PORT_EQUAL_OR:
									is_match = (dst_port == (prule->dst_port)) || (dst_port == (prule->dst_port_to));
									break;
					};
				};
			};

			if (!is_match)
				continue;

			// Pkt options (ICMP)

			if (ip->proto == 1)
			{
				if ((prule->pkt_options & PKTOPT_ICMPECHOREQUEST) && (curdir == DIRECTION_FORWARD))
				{
					is_match = ((ip->flags & 0x0800) == 0x0800);
				};

				if ((prule->pkt_options & PKTOPT_ICMPTTLEXCEEDED) && (curdir == DIRECTION_FORWARD))
				{
					is_match = ((ip->flags & 0x0b00) == 0x0b00);
				};
			};

			// Pkt options (TCP)

			if ((prule->pkt_options & PKTOPT_TCPSYN) && (ip->proto == 6))
			{
				if (curdir == DIRECTION_FORWARD)
				{
					is_match = (0x0002 & ip->flags) && ((0x0010 & ip->flags) == 0);
				};
			};

			if (!is_match)
				continue;

			// Condition

			rule_lim = prule->condition_limit;
			lim = (data.mbytes)*rule_lim;
	
			switch (prule->condition_type)
			{
				case COND_SENT_LESS:
								is_match = cnt_sent.get_cnt() < lim;
								break;

				case COND_SENT_GREATER:
								is_match = cnt_sent.get_cnt() > lim;
								break;

				case COND_RECV_LESS:
								is_match = cnt_recv.get_cnt() < lim;
								break;

				case COND_RECV_GREATER:
								is_match = cnt_recv.get_cnt() > lim;
								break;

				case COND_SENT_OR_RECV_LESS:
								is_match = (cnt_sent.get_cnt() < lim) || (cnt_recv.get_cnt() < lim);
								break;

				case COND_SENT_OR_RECV_GREATER:
								is_match = (cnt_sent.get_cnt() > lim) || (cnt_recv.get_cnt() > lim);
								break;

				case COND_SENT_AND_RECV_LESS:
								is_match = (cnt_sent.get_cnt() < lim) && (cnt_recv.get_cnt() < lim);
								break;

				case COND_SENT_AND_RECV_GREATER:
								is_match = (cnt_sent.get_cnt() > lim) && (cnt_recv.get_cnt() > lim);
								break;

				case COND_SENT_PLUS_RECV_LESS:
								is_match = (cnt_sent.get_cnt() + cnt_recv.get_cnt()) < lim;
								break;

				case COND_SENT_PLUS_RECV_GREATER:
								is_match = (cnt_sent.get_cnt() + cnt_recv.get_cnt()) > lim;
								break;
			};

			if (!is_match)
				continue;

			// Check day of week

			switch(data.lt->tm_wday)
			{
				case 1:		is_match = (prule->wday & WDAY_MON) != 0;
							break;

				case 2:		is_match = (prule->wday & WDAY_TUE) != 0;
							break;

				case 3:		is_match = (prule->wday & WDAY_WED) != 0;
							break;

				case 4:		is_match = (prule->wday & WDAY_THU) != 0;
							break;

				case 5:		is_match = (prule->wday & WDAY_FRI) != 0;
							break;

				case 6:		is_match = (prule->wday & WDAY_SAT) != 0;
							break;

				case 0:		is_match = (prule->wday & WDAY_SUN) != 0;
							break;
			};

			if (!is_match)
				continue;

			if ((data.lt->tm_hour) < prule->time_from)
				continue;

			if ((data.lt->tm_hour) > prule->time_to)
				continue;

			// Condition MAC

			cond_mac_data = prule->cond_mac_data.get();
			switch (prule->cond_mac_type)
			{
				case COND_MAC_SRC_EQUAL:
							if (data.nNicAlias == NICALIAS_NETFLOW)
							{
								is_match = memcmp(psrc_mac + 2, &cond_mac_data[2], 4) == 0;
							}
							else
							{
								is_match = memcmp(psrc_mac, &cond_mac_data[0], 6) == 0;
							}
							break;

				case COND_MAC_SRC_NOTEQUAL:
							is_match = !(memcmp(psrc_mac, &cond_mac_data[0], 6) == 0);
							break;

				case COND_MAC_DST_EQUAL:
							if (data.nNicAlias == NICALIAS_NETFLOW)
							{
								is_match = memcmp(pdst_mac + 2, &cond_mac_data[2], 4) == 0;
							}
							else
							{
								is_match = memcmp(pdst_mac, &cond_mac_data[0], 6) == 0;
							}
							break;

				case COND_MAC_DST_NOTEQUAL:
							is_match = !(memcmp(pdst_mac, &cond_mac_data[0], 6) == 0);
							break;

				default:
							break;
			};

			if (!is_match)
				continue;

			switch (prule->prevfilter_type)
			{
				case PREVFILTER_NOTMATCHED:
							is_match = (data.nPrevFilter == 0);
							break;

				case PREVFILTER_MATCHED:
							is_match = (data.nPrevFilter == 1);
							break;

				default:
							break;
			};

			if (!is_match)
				continue;

			break;				// This will be a match
		};

		//
		// Match !!!!!
		//

		if ((curdir == DIRECTION_FORWARD) || (curdir == DIRECTION_BACKWARD))
		{
			if (!is_ftp_data_match)
			{
				if ((prule->mirrored & DIRECTION_STATEFUL) && (curdir == DIRECTION_FORWARD))
				{
/*
					StateStor sis;

					sis.src_ip = ip->src_ip;
					sis.dst_ip = ip->dst_ip;
					sis.src_port = ip->src_port;
					sis.dst_port = ip->dst_port;

					sis.proto = ip->proto;
					sis.type = STATE_STOR_TYPE_NORMAL;

					(*prule).SsPutPkt(&sis, data.nUptime, tcp_flags);
*/
				};
			};

			filter_match = true;
			result.filter_match_result = true;
			result.direction = curdir;
			result.action = prule->action;
			result.rule_no = rule_number;
			result.filter_id = m_id;
			result.filter_ptr = this;
			result.is_rwrfwd = (prule->rwr_fwd > 0);
				
			// match - update corresponding counter

			if (data.nModifyCounter == MODIFY_COUNTER_NO)
			{
				if ((prule->action == ACTION_COUNT) || (prule->action == ACTION_COUNTPASS))
				{
					// Modify counters

					if ((m_nTrafficLimitAction == TRAFFICLIMIT_BLOCK) && (is_traffic_limit_exceeded(data.mbytes)))
					{
						result.action = ACTION_DENY;
						result.is_limit_exceeded = true;
						return;
					};
				};
			};

			if (data.nModifyCounter == MODIFY_COUNTER_YES)
			{
				// If rule action is about to change counters...

				if ((prule->action == ACTION_COUNT) || (prule->action == ACTION_COUNTPASS))
				{
					// Modify counters

					if (curdir == DIRECTION_FORWARD)
					{
						if (!m_bRevers)
						{
							cnt_sent.add_cnt(ip->length);
						}
						else
						{
							__int64 ip_length = ip->length;
							cnt_sent.add_cnt(-ip_length);
						};
					}
					else
					{
						if (!m_bRevers)
						{
							cnt_recv.add_cnt(ip->length);
						}
						else
						{
							__int64 ip_length = ip->length;
							cnt_recv.add_cnt(-ip_length);
						}
					};

					// Put packet into Packet collector

/* TODO
					if (m_nPktLogDest != LOGPKT_DISABLED)
					{
						if (m_bInitDone == false)
							InitCollector();

						PutPacket(ip, curdir);
					};
*/
					if (m_nTrafficLimitAction == TRAFFICLIMIT_BLOCK)
					{
						if (is_traffic_limit_exceeded(data.mbytes))
						{
							result.action = ACTION_DENY;
							result.is_limit_exceeded = true;
							result.filter_match_result = true;
							return;
						};
					}
				};
/* TODO
				if ((!is_ftp_data_match) && (ip->ftpdata_mode != FTPMODE_NONE) && (prule->pkt_options & PKTOPT_FTP))
				{
					StateStor sis;

					if (ip->ftpdata_mode == FTPMODE_ACTIVE)
					{
						// Construct temp rule for ftp active data connection

						if (prule->dst_port == 21)
						{
							sis.src_ip = ip->ftpdata_ip;
							sis.src_port = ip->ftpdata_port;
							sis.dst_ip = ip->dst_ip;
							sis.dst_port = 20;
							sis.proto = 6;
							sis.type = STATE_STOR_TYPE_FTP;
						};

						if (prule->src_port == 21)
						{
							sis.src_ip = ip->dst_ip;
							sis.src_port = 20;
							sis.dst_ip = ip->ftpdata_ip;
							sis.dst_port = ip->ftpdata_port;
							sis.proto = 6;
							sis.type = STATE_STOR_TYPE_FTP;
						};
					};

					if (ip->ftpdata_mode == FTPMODE_PASSIVE)
					{
						// Construct temp rule for ftp passive data connection

						if (prule->dst_port == 21)
						{
							sis.src_ip = ip->dst_ip;
							sis.src_port = 0;
							sis.dst_ip = ip->ftpdata_ip;
							sis.dst_port = ip->ftpdata_port;
							sis.proto = 6;
							sis.type = STATE_STOR_TYPE_FTP;
						};

						if (prule->src_port == 21)
						{
							sis.src_ip = ip->ftpdata_ip;
							sis.src_port = ip->ftpdata_port;
							sis.dst_ip = ip->dst_ip;
							sis.dst_port = 0;
							sis.proto = 6;
							sis.type = STATE_STOR_TYPE_FTP;
						};
					};

					(*prule).SsPutPkt(&sis, data.nUptime, tcp_flags);
				};
*/
			};

			break;
		};

		rule_number++;
	};

	if (filter_match && m_bBlocked)
	{
		result.action = ACTION_DENY;
	}
}

void filter2::test_match_filter()
{
	tm lt = utm::utime(2010, 05, 30, 12, 59, 12).to_tm();

	addrtable_v4 at;
	at.AddAddrPair(addrip_v4("172.30.0.0"), addrip_v4("172.30.255.255"), false);
	at.AddAddrPair(addrip_v4("172.31.0.0"), addrip_v4("172.31.255.255"), false);
	addrtablemaprec_v4 mp(at);

	addrtablemap_v4 addrmap;
	addrmap.add_element(mp);

	test_case::classname.assign(this_class_name);

	{
		test_case::testcase_num = 1;

		ip_header iphdr;
		iphdr.test_fill_packet(0);

		match_filter_input input;
		input.ip = &iphdr;
		input.mat = &addrmap;
		input.nModifyCounter = MODIFY_COUNTER_YES;
		input.nPrevFilter = 0;
		input.bWithNat = false;
		input.lt = &lt;
		input.nPreCheckAddrTables = CHECKADDR_MYPC_SRC;
		input.mbytes = 1048576;

		match_filter_result result;

		rule r1(RULE_MYIP, "0.0.0.0", "255.255.255.255", RULE_IP, "10.0.0.180", "255.255.255.255");

		filter2 f1;
		f1.rule_add(r1);

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, true);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(60));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(0));

		input.nModifyCounter = MODIFY_COUNTER_NO;

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, true);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(60));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(0));

		input.nModifyCounter = MODIFY_COUNTER_YES;

		input.nPreCheckAddrTables = 0;

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, false);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(60));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(0));

		input.nPreCheckAddrTables = CHECKADDR_MYPC_SRC;

		f1.rules_clear();
		rule r2(RULE_MYIP, "0.0.0.0", "255.255.255.255", RULE_IP, "10.0.0.181", "255.255.255.255");
		f1.rule_add(r2);

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, false);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(60));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(0));

		f1.rules_clear();
		rule r3(RULE_MYIP, "0.0.0.0", "255.255.255.255", RULE_IP, "10.0.0.181", "255.255.255.0");
		f1.rule_add(r3);
		input.nPreCheckAddrTables = CHECKADDR_MYPC_DST;
		iphdr.swap_src_dst();

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, true);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(60));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(60));

		rule r4(RULE_MYIP, "0.0.0.0", "255.255.255.255", RULE_IP, "0.0.0.0", "0.0.0.0");
		f1.rule_add(r4);
		input.nPreCheckAddrTables = CHECKADDR_MYPC_SRC;
		iphdr.swap_src_dst();

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, true);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(120));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(60));

		// RULE_LAN, RULE_WAN

		f1.rules_clear();
		rule r5(RULE_LAN, "255.255.255.255", "255.255.255.255", RULE_IP, "0.0.0.0", "0.0.0.0");
		f1.rule_add(r5);
		input.nPreCheckAddrTables = CHECKADDR_LAT_SRC;

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, true);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(180));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(60));

		f1.rules_clear();
		rule r6(RULE_LAN, "255.255.255.255", "255.255.255.255", RULE_WAN, "255.255.255.255", "255.255.255.255");
		f1.rule_add(r6);
		input.nPreCheckAddrTables = CHECKADDR_LAT_SRC;

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, true);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(240));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(60));

		// RULE_MAC

		f1.rules_clear();
		rule r7(RULE_MAC, "255.255.255.255", "255.255.255.255", RULE_MAC, "255.255.255.255", "255.255.255.255");
		r7.src_mac.from_string("FF:FE:FD:FC:FB:FA");
		r7.dst_mac.from_string("00:01:02:03:04:05");
		f1.rule_add(r7);
		input.nPreCheckAddrTables = 0;

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, true);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(240));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(120));

		r7.src_mac.from_string("FF:FE:FD:FC:FB:F0");
		f1.rules_clear();
		f1.rule_add(r7);

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, false);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(240));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(120));

		// RULE_RANGE

		f1.rules_clear();
		rule r8(RULE_RANGE, "192.168.1.1", "192.168.1.3", RULE_RANGE, "10.0.0.1", "10.0.0.255");
		f1.rule_add(r8);
		input.nPreCheckAddrTables = 0;

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, true);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(300));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(120));

		f1.rules_clear();
		r8.src_ip.from_string("192.168.1.3");
		f1.rule_add(r8);

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, false);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(300));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(120));

		// RULE_ADDRGRP

		f1.rules_clear();
		rule r9(RULE_ADDRGRP, "192.168.1.1", "192.168.1.3", RULE_RANGE, "10.0.0.1", "10.0.0.255");
		r9.src_atkey = 11;
		f1.rule_add(r9);
		input.nPreCheckAddrTables = 0;

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, false);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(300));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(120));

		addrtablemaprec_v4 amr;
		amr.set_id(11);
		addrmap.add_element(amr);
		addrmap.AddAddrPair(11, addrip_v4("192.168.1.1"), addrip_v4("192.168.1.2"));

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, true);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(360));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(120));

		// RULE_ADDRGRP_NO

		f1.rules_clear();
		rule r10(RULE_ADDRGRP_NO, "192.168.1.1", "192.168.1.3", RULE_RANGE, "10.0.0.1", "10.0.0.255");
		r10.src_atkey = 11;
		f1.rule_add(r10);
		input.nPreCheckAddrTables = 0;

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, false);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(360));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(120));

		// RULE_HOST

		f1.rules_clear();
		rule r11(RULE_IP, "0.0.0.0", "0.0.0.0", RULE_HOST, "0.0.0.0", "0.0.0.0");
		r11.src_host.set_host("bla-bla-bla.com");
		r11.dst_host.set_host("localhost");
		f1.rule_add(r11);
		
		hosttable ht;
		ht.add_host(TESTDOMAIN_UTM_NAME);
		ht.add_host("localhost");
		ht.refresh_hosttable();
		input.host2ip = &ht;

		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, false);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(360));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(120));

		input.ip->dst_ip_addr = utm::addrip_v4("127.0.0.1");
		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, true);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(420));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(120));

		input.ip->dst_ip_addr = utm::addrip_v4("127.0.0.2");
		f1.match_filter(input, result, true);
		TEST_CASE_CHECK(result.filter_match_result, false);
		TEST_CASE_CHECK(f1.cnt_sent.get_cnt(), __int64(420));
		TEST_CASE_CHECK(f1.cnt_recv.get_cnt(), __int64(120));

	}
}

}
