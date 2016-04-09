#include "StdAfx.h"
#include "filterset.h"

#include <ubase_test.h>
#include <addrtablemap_v4.h>
#include <ufs.h>
#include <utime.h>

#include <filteragent.h>

namespace utm {

const char filterset::this_class_name[] = "filterset";

filterset::filterset(void)
{
}

filterset::~filterset(void)
{
}

void filterset::copy_counters(const filterset& fs)
{
	fdata.copy(fs.fdata);

	for (auto iter = filters.items.begin(); iter != filters.items.end(); ++iter)
	{
		filter2* pf_src = dynamic_cast<filter2 *>(iter->get());
		filter2* pf_dst = dynamic_cast<filter2 *>(fs.filters.findptr_by_id(pf_src->get_id()));
		if (pf_dst != NULL)
		{
			pf_src->copy_filter_counters(*pf_dst);
		}
	}
}

void filterset::reset_all_counters()
{
	for (auto iter = filters.items.begin(); iter != filters.items.end(); ++iter)
	{
		filter2* pf = dynamic_cast<filter2 *>(iter->get());
		pf->reset_filter_counters(true);
	}
}

void filterset::reset_on_schedule(bool reset_history, std::string& filterids)
{
	utime now(true);
	standard_timeset stimeset(now);

	idlist zeroed_filterids;
	for (auto iter = filters.items.begin(); iter != filters.items.end(); ++iter)
	{
		filter2* pf = dynamic_cast<filter2 *>(iter->get());
		if (pf->reset_on_schedule(stimeset, reset_history))
		{
			zeroed_filterids.ids.push_back(pf->get_id());
		}
	}
	filterids = zeroed_filterids.to_string();
}

void filterset::prepare_proc_usage()
{
	is_proc_used = false;
	for (auto iter = filters.items.begin(); iter != filters.items.end(); ++iter)
	{
		filter2* pf = dynamic_cast<filter2 *>(iter->get());
		for (auto riter = pf->rules.items.begin(); riter != pf->rules.items.end(); ++riter)
		{
			rule* pr = dynamic_cast<rule *>(riter->get());
			if ((pr->src_type == RULE_PROCNAME) || (pr->src_type == RULE_PROCUSER) ||
				(pr->dst_type == RULE_PROCNAME) || (pr->dst_type == RULE_PROCUSER))
			{
				is_proc_used = true;
				break;
			}
		}

		if (is_proc_used)
			break;
	}
}

void filterset::prepare_shaper_usage()
{
	is_shaper_used = false;
	for (auto iter = filters.items.begin(); iter != filters.items.end(); ++iter)
	{
		filter2* pf = dynamic_cast<filter2 *>(iter->get());
		if (pf->get_actual_speed() > 0)
		{
			is_shaper_used = true;
			break;
		}
	}
}

bool filterset::is_addrtable_used(unsigned int atkey) const
{
	for (auto iter = filters.items.begin(); iter != filters.items.end(); ++iter)
	{
		filter2* pf = dynamic_cast<filter2 *>(iter->get());
		for (auto iru = pf->rules.items.begin(); iru != pf->rules.items.end(); ++iru)
		{
			rule* pr = dynamic_cast<rule *>(iru->get());
			if ((pr->src_type == RULE_ADDRGRP) && (pr->src_atkey == atkey))
			{
				return true;
			};

			if ((pr->dst_type == RULE_ADDRGRP) && (pr->dst_atkey == atkey))
			{
				return true;
			};
		};
	};

	return false;
}

void filterset::process_traffic_limit_flags(const gstring& flagfolder)
{
	utm::utime now(true);
	std::string nowstr = now.to_string();

	bool flag_traffic_limit;
	bool flag_traffic_warn;

	for (auto iter = filters.items.begin(); iter != filters.items.end(); ++iter)
	{
		filter2* pf = dynamic_cast<filter2 *>(iter->get());
		flag_traffic_limit = pf->is_traffic_limit_exceeded();
		flag_traffic_warn = pf->is_traffic_limitwarn_exceeded();

		if (!flag_traffic_limit)
		{
			// Clear flag 
			pf->m_wTrafficLimitFlags &= 0xFFFE;
		}
		else
		{
			if (pf->m_wTrafficLimitFlags & 0x0001)
			{
				// Flag is already created.
				// Do nothing.
			}
			else
			{
				// Create flag

				std::string s = std::string("limit_") + boost::lexical_cast<std::string>(pf->get_id());
				utm::gstring flagfile = ufs::make_full_filepath(flagfolder, utm::gstring(s));

				std::fstream f;
				f.open(flagfile.c_str(), std::fstream::out);
				if (f)
				{
					f << nowstr << " " << pf->cnt_sent.get_cnt() << "/" << pf->cnt_recv.get_cnt();
					f.close();
				}

				pf->m_wTrafficLimitFlags |= 0x0001;
			};
		}

		if (!flag_traffic_warn)
		{
			// Clear flag

			pf->m_wTrafficLimitFlags &= 0xFFFD;
		}
		else
		{
			if (pf->m_wTrafficLimitFlags & 0x0002)
			{
				// Flag is already created.
				// Do nothing.
			}
			else
			{
				// Create flag

				std::string s = std::string("warn_") + boost::lexical_cast<std::string>(pf->get_id());
				utm::gstring flagfile = ufs::make_full_filepath(flagfolder, utm::gstring(s));

				std::fstream f;
				f.open(flagfile.c_str(), std::fstream::out);
				if (f)
				{
					f << nowstr << " " << pf->cnt_sent.get_cnt() << "/" << pf->cnt_recv.get_cnt();
					f.close();
				}

				pf->m_wTrafficLimitFlags |= 0x0002;
			};					
		}
	}
}

void filterset::select_users_by_filter_id(unsigned int filter_id, std::map<unsigned int, gstring>& selected_users) const
{
	for (auto uiter = users.items.begin(); uiter != users.items.end(); ++uiter)
	{
		fsuser* pu = dynamic_cast<fsuser *>(uiter->get());
		if (pu->fids.ids.end() != pu->fids.ids.find(filter_id))
		{
			selected_users.insert(std::make_pair(pu->get_id(), pu->get_name()));
		}
	}
}

void filterset::make_user_agentinfo_as_xml(const utime& cutime, const fsuser& user, std::ostringstream& res) const
{
	unsigned int total_points = fdata.get_total_points();
	unsigned int next_point = fdata.get_next_point();

	res << "<User>\r\n" << "<N>" << user.get_name().getutf8() << "</N>";
	res << "<K>" << get_kilobytes() << "</K>";
	res << "<M>" << get_megabytes() << "</M>\r\n";

	if (!user.is_user_enabled(cutime))
	{
		res << "<Dis>1</Dis>";
	}
	else
	{
		bool found, finish;
		size_t idcount = 0;

		for (auto iter = filters.items.begin(); iter != filters.items.end(); ++iter)
		{
			filter2* pf = dynamic_cast<filter2 *>(iter->get());
			found = false;
			finish = false;

			if (user.m_nType == USER_ACCESS_SPECIFIED_FILTERS)
			{
				found = user.fids.check_id(pf->get_id());
				if (found)
				{
					idcount++;
					finish = (idcount == user.fids.size());
				}
			}
			else if (user.m_nType == USER_ACCESS_ALL_FILTERS)
			{
				found = true;
			}

			if (found)
			{
				std::string xml;
				utm::filteragent fa;
				fa.create(total_points, next_point, 10, *pf, xml);
				res << xml;
			}

			if (finish)
			{
				break;
			}
		};
	}

	res << "</User>\r\n";
}

lat_as_string_container filterset::get_lat() const
{
	lat_as_string_container retval;

	typedef std::map<utm::addrip_v4, utm::addrip_v4> latmap;
	const latmap& lm = table_lat.itemsref();

	for(latmap::const_iterator iter = lm.begin(); iter != lm.end(); ++iter)
	{
		std::string latrec = iter->first.to_string() + "," + iter->second.to_string();
		retval.push_back(latrec);
	}

	return retval;
}

void filterset::parse_lat_string(const char *lat_string)
{
	if (lat_string == NULL)
		return;

	const char *p = strchr(lat_string, ',');
	if (p == NULL)
		return;

	std::string str1(lat_string, p - lat_string);

	utm::addrip_v4 addr_begin(str1.c_str());
	utm::addrip_v4 addr_end(p + 1);

	table_lat.AddAddrPair(addr_begin, addr_end, false);
}

ubase* filterset::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, FILTER_XMLTAG_FILTER) == 0)
	{
		u = filters.init_and_get_temp_item(new filter2());
		return u;
	}

	if (strcmp(keyname, ADDRTABLEMAP_XMLTAG_MAT) == 0)
	{
		u = table_mat.init_and_get_temp_item(new addrtablemaprec<addrtable_v4>());
		return u;
	}

	if (strcmp(keyname, FSUSER_XMLTAG_ROOT) == 0)
	{
		u = users.init_and_get_temp_item(new fsuser());
		return u;
	}

	if (strcmp(keyname, URLFILTER_XMLTAG_ROOT) == 0)
	{
		u = urlfilters.init_and_get_temp_item(new urlfilter());
		return u;
	}

	if (strcmp(keyname, PROCNICKNAME_XMLTAG_ROOT) == 0)
	{
		u = procs.init_and_get_temp_item(new procnickname());
		return u;
	}

	if (strcmp(keyname, MONRANGE_XMLTAG_ROOT) == 0)
	{
		u = ranges.init_and_get_temp_item(new monitor_range());
		return u;
	}

	return u;
}



void filterset::xml_catch_subnode_finished(const char *keyname)
{
	if (strcmp(keyname, FILTER_XMLTAG_FILTER) == 0)
	{
		filters.commit_temp_item();
		return;
	}

	if (strcmp(keyname, ADDRTABLEMAP_XMLTAG_MAT) == 0)
	{
		table_mat.commit_temp_item();
		return;
	}

	if (strcmp(keyname, FSUSER_XMLTAG_ROOT) == 0)
	{
		users.commit_temp_item();
		return;
	}

	if (strcmp(keyname, URLFILTER_XMLTAG_ROOT) == 0)
	{
		urlfilters.commit_temp_item();
		return;
	}

	if (strcmp(keyname, PROCNICKNAME_XMLTAG_ROOT) == 0)
	{
		procs.commit_temp_item();
		return;
	}

	if (strcmp(keyname, MONRANGE_XMLTAG_ROOT) == 0)
	{
		ranges.commit_temp_item();
		return;
	}
}

void filterset::test_fillparams(int test_num)
{
	test_case::classname.assign(this_class_name);

	clear();

	m_bKeepCounters = false;

	{
		table_lat.AddAddrPair(utm::addrip_v4("192.168.111.1"), utm::addrip_v4("192.168.111.255"), false);
		table_lat.AddAddrPair(utm::addrip_v4("192.168.2.1"), utm::addrip_v4("192.168.2.255"), false);
		table_lat.AddAddrPair(utm::addrip_v4("192.168.31.1"), utm::addrip_v4("192.168.31.255"), false);

		addrtablemaprec_v4 tm(table_lat);
		tm.addrtable.descr = "blabla";
		tm.addrtable.type_id = addrtable<addrip_v4>::ADDR_TABLE_ORIGINAL;
		table_mat.add_element(tm);
	}

	{
		addrtablemaprec_v4 tm;
		tm.addrtable.descr = "blabla - 2";
		tm.addrtable.type_id = addrtable<addrip_v4>::ADDR_TABLE_PPP;
		table_mat.add_element(tm);
	}

	{
		addrtablemaprec_v4 tm;
		tm.addrtable.descr = "blabla - 3";
		tm.addrtable.type_id = addrtable<addrip_v4>::ADDR_TABLE_EXTFILE;
		tm.addrtable.filename = "c:\\program files\\tmeter\\table.txt";
		table_mat.add_element(tm);
	}

	{
		fsuser u1;
		u1.m_szName.assign("Vasya");
		u1.m_szDescr.assign("Pupkin Vasya");
		u1.fids.from_string("1 2 3 4 5 6 7 9999");
		users.add_element(u1);

		fsuser u2;
		u2.m_szName.assign("John");
		u2.m_szDescr.assign("Johm Smith");
		users.add_element(u2);

		TEST_CASE_CHECK(users.size(), size_t(2));
	}

	{
		urlfilter r1;
		r1.name.assign("White list");
		r1.is_any_filter = true;
		r1.parse_urlrules_str("+microsoft.com +google.com");
		urlfilters.add_element(r1);

		urlfilter r2;
		r2.name.assign("Blocked sites");
		r2.fids.from_string("1 2 3");
		r2.parse_urlrules_str("sex.com drugs.com rock-n-roll.com");
		urlfilters.add_element(r2);
		TEST_CASE_CHECK(r2.urlrules.size(), size_t(3));

		TEST_CASE_CHECK(urlfilters.size(), size_t(2));
	}

	{
		procnickname p1;
		p1.nickname.assign("firefox");
		p1.pathexe.assign("C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe");
		procs.add_element(p1);

		TEST_CASE_CHECK(procs.size(), size_t(3));
	}

	{
		monitor_range m1;
		m1.start_addr.from_string("192.168.1.1");
		m1.end_addr.from_string("192.168.1.255");
		m1.active = true;
		m1.comment.assign("Comment 1");
		ranges.add_element(m1);

		monitor_range m2;
		m2.start_addr.from_string("192.168.2.1");
		m2.end_addr.from_string("192.168.2.255");
		m2.active = true;
		m2.comment.assign("Comment 2");
		ranges.add_element(m2);

		TEST_CASE_CHECK(ranges.size(), size_t(2));
	}

	{
		filter2 f1;
		f1.test_fillparams(1);
		filters.add_element(f1);
	}

	return;
}

}