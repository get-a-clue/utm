#include "stdafx.h"
#include "monitor_total.h"

#include <utimestamp.h>
#include <ubase_test.h>

#include "arp_table.h"
#include "macvendors.h"

namespace utm {

const char monitor_total::this_class_name[] = "monitor_total";
boost::function<std::int64_t ()> monitor_total::get_current_hires_tick = monitor_total::get_fake_tick;
std::int64_t monitor_total::fake_tick = 0;

monitor_total::monitor_total(void)
{
	add_event = NULL;
	init_count = 0;
}

monitor_total::~monitor_total(void)
{
}

void monitor_total::clear()
{
	boost::mutex::scoped_lock lock(guard);
	results.clear();
	results_index.clear();
	ranges.clear();
	maccomments.clear();
	last_reverse_lookup_tick = 0;
}

void monitor_total::safe_copy_to(monitor_total& duplicate) const
{
	boost::mutex::scoped_lock lock(guard);
	duplicate.results = results;
	duplicate.results_index = results_index;
	duplicate.ranges = ranges;
	duplicate.maccomments = maccomments;
	duplicate.last_reverse_lookup_tick = last_reverse_lookup_tick;
}

size_t monitor_total::result_size() const
{
	boost::mutex::scoped_lock lock(guard);
	return results.size();
}

size_t monitor_total::maccomments_size() const
{
	boost::mutex::scoped_lock lock(guard);
	return maccomments.size();
}

int monitor_total::get_init_count() const
{
	return init_count;
}

void monitor_total::init_from_ranges(const monitor_range_list& ra)
{
	boost::mutex::scoped_lock lock(guard);

	init_count++;
	monitor_result_container prev_results = results;

	ranges = ra;
	results.clear();
	results_index.clear();
	maccomments.clear();
	host2resolve.clear();

	unsigned int i = 0;
	for (auto iter_range = ranges.items.begin(); iter_range != ranges.items.end(); ++iter_range)
	{
		monitor_range* mr = dynamic_cast<monitor_range *>(iter_range->get());

		if (!(mr->active))
			continue;

		unsigned int start_addr = mr->start_addr.m_addr;
		unsigned int end_addr = mr->end_addr.m_addr;

		for (unsigned int u = start_addr; u <= end_addr; u++, i++)
		{
			monitor_result r;

			r.id = i;
			r.range_id = mr->id;
			r.addr = addrip_v4(u);

			for (monitor_result_container::iterator iter_prev = prev_results.begin(); iter_prev != prev_results.end(); ++iter_prev)
			{
				if (iter_prev->addr.m_addr == u)
				{
					r.status = iter_prev->status;
					r.last_ping_reply = iter_prev->last_ping_reply;
					r.mac = iter_prev->mac;
					r.hostname = iter_prev->hostname;

					break;
				}
			}

			monitor_detail_list& dli = mr->details;
			for (auto iter_detal = dli.items.begin(); iter_detal != dli.items.end(); ++iter_detal)
			{
				monitor_detail_record *md = dynamic_cast<monitor_detail_record *>(iter_detal->get());

				if (md->ip.m_addr == u)
				{
					r.descr = md->comment;
					r.flags = md->flags;

					break;
				}
			}

			results.push_back(r);
			results_index.insert(std::make_pair(addrip_v4(u), i));

			if (mr->reverse_lookup)
			{
				dnsserver4addr da;
				da.addr = r.addr;
				da.dns = mr->dnsserver;
				host2resolve.push_back(da);
			}
		}

		for (auto iter_comment = mr->details.items.begin(); iter_comment != mr->details.items.end(); ++iter_comment)
		{
			monitor_detail_record *md = dynamic_cast<monitor_detail_record *>(iter_comment->get());
			if ((md->ip.is_zero()) && (!md->mac.is_zero()) && (!md->comment.empty()))
			{
				maccomments.insert(std::make_pair(md->mac, md->comment));
			}
		}
	}
}

bool monitor_total::get_result_record(size_t index, monitor_result& mr) const
{
	boost::mutex::scoped_lock lock(guard);
	size_t total_size = results.size();
	if (index >= total_size)
	{
		return false;
	}

	mr = results[index];
	return true;
}

void monitor_total::set_result_record(size_t index, const monitor_result& mr)
{
	boost::mutex::scoped_lock lock(guard);
	size_t total_size = results.size();
	if (index < total_size)
		results[index] = mr;
}

void monitor_total::set_reverse_hostname(const addrip_v4& addr, const char *hostname)
{
	boost::mutex::scoped_lock lock(guard);

	monitor_result_container::iterator iter;
	for (iter = results.begin(); iter != results.end(); ++iter)
	{
		if (iter->addr == addr.m_addr)
		{
			iter->hostname.assign(hostname);
			break;
		}
	}
}

void monitor_total::extract_minidump(monitor_minidump& mini, __int64 nRemoteReverseLookupTick)
{
	boost::mutex::scoped_lock lock(guard);

	monitor_result_container::iterator iter;
	for (iter = results.begin(); iter != results.end(); ++iter)
	{
		monitor_miniresult mmr;
		mmr.nRangeId = iter->range_id;
		mmr.nStatus = iter->status;
		mmr.lastReplyDt = iter->last_ping_reply.to_uint();
		mmr.nRrCounter = (unsigned short)(iter->last_serial_request_count & 0xff); mmr.nRrCounter = mmr.nRrCounter << 8;
		mmr.nRrCounter += (unsigned short)(iter->last_serial_reply_count & 0xff);
		memcpy(mmr.mac, iter->mac.get(), sizeof(mmr.mac));
		mini.dumpdata.insert(std::make_pair(iter->addr.m_addr, mmr));

//		if (nRemoteReverseLookupTick <  last_reverse_lookup_tick)
		{
			if (!iter->hostname.empty())
				mini.hostdata.insert(std::make_pair(iter->addr.m_addr, iter->hostname));
		}
	}
}

void monitor_total::apply_minidump(const monitor_minidump& mini)
{
	boost::mutex::scoped_lock lock(guard);

	monitor_miniresult_container::const_iterator iter_dump;
	monitor_ip2host_container::const_iterator iter_host;

	monitor_result_container::iterator iter;
	for (iter = results.begin(); iter != results.end(); ++iter)
	{
		iter_dump = mini.dumpdata.find(iter->addr.m_addr);
		if (iter_dump != mini.dumpdata.end())
		{
			const monitor_miniresult& mmi = iter_dump->second;
			iter->range_id = mmi.nRangeId;
			iter->status = mmi.nStatus;
			iter->last_ping_reply = utm::utimestamp(mmi.lastReplyDt);
			iter->last_serial_request_count = static_cast<unsigned int>(((mmi.nRrCounter >> 8) & 0xFF));
			iter->last_serial_reply_count = static_cast<unsigned int>(mmi.nRrCounter & 0xFF);
			iter->mac.set(mmi.mac);
		}

		iter_host = mini.hostdata.find(iter->addr.m_addr);
		if (iter_host != mini.hostdata.end())
		{
			iter->hostname = iter_host->second;
		}
	}
}

void monitor_total::format_monitoring_message(const monitor_result& r, const macvendors& mv, gstring& msg) const
{
	msg.assign(_U("IP "));
	msg.append(r.addr.to_string());
	msg.append(_U(" is "));
	msg.append(monitor_result::get_ping_status_str(r.status));

	switch(r.action_type)
	{
		case 1:	msg.append(_U(" (+mac);"));	break;
		case 2:	msg.append(_U(" (-mac);"));	break;
		default: msg.append(_U(";")); break;
	}

	append_monitoring_message(msg, _U("Hostname "), r.hostname);

	bool maccomment_found = false;
	if (!r.mac.is_zero())
	{
		addrmac_comment_container::const_iterator iter = maccomments.find(r.mac);
		if (iter != maccomments.end())
		{
			append_monitoring_message(msg, _U("Description "), iter->second);
			maccomment_found = true;
		}
	}

	if (!maccomment_found)
	{
		append_monitoring_message(msg, _U("Description "), r.descr);
	}

	if (!r.mac.is_zero())
	{
		append_monitoring_message(msg, _U("Mac "), gstring(r.mac.to_string()));

		const char *macvendor = mv.find_vendor(r.mac.get());
		if (macvendor[0] != 0)
		{
			append_monitoring_message(msg, _U("Vendor "), gstring(macvendor));
		}
	}
}

void monitor_total::parse_monitoring_message(const gstring& msg, utm::addrip_v4& ip, utm::gstring& state, utm::gstring& flag, utm::gstring& descr, utm::gstring& hostname, utm::addrmac& mac)
{
#ifdef _UNICODE
	std::vector<std::wstring> tokens;
#else
	std::vector<std::string> tokens;
#endif
	utm::stringtools::split(tokens, msg.c_str(), _U(';'));

	const static utm::gstring delim(" ");
	size_t t0 = tokens[0].find(delim.c_str(), 0) + 1;
	size_t t1 = tokens[0].find(delim.c_str(), t0) + 1;
	size_t t2 = tokens[0].find(delim.c_str(), t1) + 1;
	size_t t3 = tokens[0].find(delim.c_str(), t2) + 1;

	std::string s0 = utm::gstring(tokens[0]).getmb();

	if ((t3 == std::string::npos) || (t3 == 0))
	{
		state.assign(tokens[0].c_str() + t2);
	}
	else
	{
		state = s0.substr(t2, t3 - t2 - 1);
		flag = s0.substr(t3);
	}

	ip.from_string(s0.substr(t0, t1 - t0));

	static const utm::gstring param_mac("Mac ");
	static const utm::gstring param_hostname("Hostname ");
	static const utm::gstring param_descr("Description ");

	size_t tsize = tokens.size();
	for (size_t i = 1; i < tsize; i++)
	{
		size_t t = tokens[i].find(delim.c_str(), 0) + 1;

#ifdef _UNICODE
		if (wcsncmp(tokens[i].c_str(), param_mac.c_str(), param_mac.size()) == 0)
#else
		if (strncmp(tokens[i].c_str(), param_mac.c_str(), param_mac.size()) == 0)
#endif
		{
			mac.from_string(utm::gstring(tokens[i].c_str() + t).getmb());
			continue;
		}

#ifdef _UNICODE
		if (wcsncmp(tokens[i].c_str(), param_hostname.c_str(), param_hostname.size()) == 0)
#else
		if (strncmp(tokens[i].c_str(), param_hostname.c_str(), param_hostname.size()) == 0)
#endif
		{
			hostname.assign(tokens[i].c_str() + t);
			continue;
		}

#ifdef _UNICODE
		if (wcsncmp(tokens[i].c_str(), param_descr.c_str(), param_descr.size()) == 0)
#else
		if (strncmp(tokens[i].c_str(), param_descr.c_str(), param_descr.size()) == 0)
#endif
		{
			descr.assign(tokens[i].c_str() + t);
			continue;
		}
	}

	return;
}

void monitor_total::analyze(const macvendors& mv)
{
	arp_table arp;
	arp.fill();

	monitor_range mrange;

	boost::mutex::scoped_lock lock(guard);

	monitor_result_container::iterator iter;
	for (iter = results.begin(); iter != results.end(); ++iter)
	{
		if (iter->flags & MONITOR_FLAG_EXCLUDE)
			continue;

		unsigned int rangetype = 0;
		if (mrange.id != iter->range_id)
		{
			ubase *u = ranges.findptr_by_id(iter->range_id);
			if (u != NULL)
			{
				monitor_range* mr = dynamic_cast<monitor_range *>(u);
				if (mr != NULL)
				{
					rangetype = mr->rangetype;
					mrange = *mr;
				}
			}
		}
		else
		{
			rangetype = mrange.rangetype;
		}

		unsigned int prev_status = iter->status;
		monitor_result::analyze_and_set_new_status(*iter, rangetype > 0);
		unsigned int new_status = iter->status;
		bool status_changed = prev_status != new_status;
		bool no_message = (prev_status == PING_STATUS_UNKNOWN) && (iter->status == PING_STATUS_OFFLINE);

		gstring monitoring_msg;

		if (status_changed && !no_message)
		{
			iter->action_type = 0;
			format_monitoring_message(*iter, mv, monitoring_msg);
			// добавить событие в очередь для БД
		}

		addrip_v4 ip(iter->addr);
		addrmac mac_from_arp;
		bool macfound = arp.findmac(ip, mac_from_arp);

		if (macfound)
		{
			if (iter->mac != mac_from_arp)
			{
				// Мак изменился
#ifdef _DEBUG
				if (add_event != NULL) 
				{
					char tmp[MAXLEN_MSG];
					sprintf_s(tmp, MAXLEN_MSG, "Mac found for %s, mac=%s", ip.to_string().c_str(), mac_from_arp.to_string().c_str());
					add_event(gstring(tmp), true);
				}
#endif
				iter->action_type = 1;
				iter->mac = mac_from_arp;
				format_monitoring_message(*iter, mv, monitoring_msg);
				// добавить событие в очередь для БД
			}
		}
		else
		{
			if (!iter->mac.is_zero())
			{
#ifdef _DEBUG
				if (add_event != NULL)
				{
					char tmp[MAXLEN_MSG];
					sprintf_s(tmp, MAXLEN_MSG, "Mac removed for %s (was=%s)", ip.to_string().c_str(), iter->mac.to_string().c_str());
					add_event(gstring(tmp), true);
				}
#endif

				iter->action_type = 2;
				format_monitoring_message(*iter, mv, monitoring_msg);
				// добавить событие в очередь для БД
			}
		}
		iter->mac = mac_from_arp;

		iter->last_serial_request_count = iter->curr_serial_request_count;
		iter->last_serial_reply_count = iter->curr_serial_reply_count;
		iter->curr_serial_request_count = 0;
		iter->curr_serial_reply_count = 0;

		if ((!monitoring_msg.empty()) && (add_event != NULL))
		{
			add_event(monitoring_msg, false);
//			host_state_changes.push_back(*iter);
		}
	}
}


void monitor_total::test_all()
{
	test_case::classname.assign(this_class_name);

	std::int64_t t1 = monitor_total::get_current_hires_tick();
	std::int64_t t2 = monitor_total::get_current_hires_tick();

	monitor_total mt;
	{
		monitor_range range;
		range.test_fillparams(1);

		monitor_range_list mrl;
		mrl.add_element(range);

		mt.init_from_ranges(mrl);
	}

	TEST_CASE_CHECK(mt.result_size(), size_t(255));
	TEST_CASE_CHECK(mt.maccomments_size(), size_t(3));

	{
		monitor_result r1;
		r1.addr.from_string("192.168.1.7");
		r1.status = PING_STATUS_OFFLINE;
		r1.mac.from_string("61:62:63:64:65:66");
		macvendors mv;

		r1.action_type = 0;
		{
			gstring s1;
			mt.format_monitoring_message(r1, mv, s1);
			TEST_CASE_CHECK(s1 == "IP 192.168.1.7 is Offline;Mac 61:62:63:64:65:66;", bool(true));
			addrip_v4 ip;
			addrmac mac;
			gstring state, flag, descr, hostname;
			mt.parse_monitoring_message(s1, ip, state, flag, descr, hostname, mac);
			TEST_CASE_CHECK(ip.to_string(), std::string("192.168.1.7"));
			TEST_CASE_CHECK(state == "Offline", bool(true));
			TEST_CASE_CHECK(flag == "", bool(true));
			TEST_CASE_CHECK(mac.to_string(), std::string("61:62:63:64:65:66"));
		}

		r1.action_type = 1;
		{
			gstring s1;
			mt.format_monitoring_message(r1, mv, s1);
			TEST_CASE_CHECK(s1 == "IP 192.168.1.7 is Offline (+mac);Mac 61:62:63:64:65:66;", bool(true));
			addrip_v4 ip;
			addrmac mac;
			gstring state, flag, descr, hostname;
			mt.parse_monitoring_message(s1, ip, state, flag, descr, hostname, mac);
			TEST_CASE_CHECK(ip.to_string(), std::string("192.168.1.7"));
			TEST_CASE_CHECK(state == "Offline", bool(true));
			TEST_CASE_CHECK(flag == "(+mac)", bool(true));
			TEST_CASE_CHECK(mac.to_string(), std::string("61:62:63:64:65:66"));
		}

		r1.action_type = 2;
		{
			gstring s1;
			mt.format_monitoring_message(r1, mv, s1);
			TEST_CASE_CHECK(s1 == "IP 192.168.1.7 is Offline (-mac);Mac 61:62:63:64:65:66;", bool(true));
			addrip_v4 ip;
			addrmac mac;
			gstring state, flag, descr, hostname;
			mt.parse_monitoring_message(s1, ip, state, flag, descr, hostname, mac);
			TEST_CASE_CHECK(ip.to_string(), std::string("192.168.1.7"));
			TEST_CASE_CHECK(state == "Offline", bool(true));
			TEST_CASE_CHECK(flag == "(-mac)", bool(true));
			TEST_CASE_CHECK(mac.to_string(), std::string("61:62:63:64:65:66"));
		}
	}

	return;
}

}