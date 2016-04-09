#include "StdAfx.h"
#include "filter_.h"

namespace utm {

const char filter::this_class_name[] = "filter";

filter::filter(void)
{
	clear(true);
}

filter::~filter(void)
{
}

void filter::clear(bool dont_clear_parent)
{
	if (!dont_clear_parent)
		filter_base::clear();

	seq_id = 0;
}

bool filter::is_equal(const filter& f) const
{
	if (seq_id != f.seq_id)
		return false;

	const filter_base* fbase = (const filter_base *)(&f);
	bool retval = filter_base::operator==(*fbase);
	return retval;
}

void filter::rule_add(const rule& r)
{
	rules.add_element(r);
}

void filter::rules_clear()
{
	rules.clear();
}

std::string filter::get_workhours_str() const
{
	bool is_default = true;

	std::ostringstream oss;
	for (int i = 0; i < 7; i++)
	{
		if (m_nWorkHours[i] != 0xFFFFFFFF)
			is_default = false;

		if (i > 0)
			oss << ",";

		oss << std::hex << m_nWorkHours[i];
	}

	std::string retval = is_default ? "" : oss.str();
	return retval;
}

void filter::parse_workhours_string(const char *workhours_string)
{
	if (workhours_string == NULL)
		return;

	if (*workhours_string == 0)
		return;

	sscanf_s(workhours_string, "%x,%x,%x,%x,%x,%x,%x", &m_nWorkHours[0], &m_nWorkHours[1], &m_nWorkHours[2], &m_nWorkHours[3], &m_nWorkHours[4], &m_nWorkHours[5], &m_nWorkHours[6]);
}

ubase* filter::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, RULE_XMLTAG_RULE) == 0)
	{
		u = dynamic_cast<ubase *>(new rule());
		rules.add_element(u);
	}

	return u;
}

void filter::select_diffspeed(unsigned int diff_size, unsigned int start_diffindex, unsigned short* diffsent, unsigned short* diffrecv)  const
{
	if (diff_size > 0)
	{
		unsigned int n, curindex = start_diffindex;
		for (unsigned int i = 0; i < diff_size; i++, curindex++, diffsent++, diffrecv++)
		{
			n = curindex % MAXVALUES;
			*diffsent = cnt_sent.cnt_set[n];
			*diffrecv = cnt_recv.cnt_set[n];
		}
	}
}

void filter::apply_diffspeed(unsigned int diff_size, unsigned int diffindex, unsigned short* diffsent, unsigned short* diffrecv)
{
	if (diff_size > 0)
	{
		unsigned short sent_speed, recv_speed;
		unsigned int n;
		for (unsigned int i = 0; i < diff_size; i++, diffindex++, diffsent++, diffrecv++)
		{
			n = diffindex % MAXVALUES;
			sent_speed = *diffsent;
			recv_speed = *diffrecv;
			cnt_sent.cnt_set[n] = sent_speed;
			cnt_recv.cnt_set[n] = recv_speed;
		}
		cnt_sent.cnt_speed = utm::filtercnt::unpack_speed(sent_speed);
		cnt_recv.cnt_speed = utm::filtercnt::unpack_speed(recv_speed);
	}
}

void filter::get_proto_name(int n, std::string& protoname)
{
	switch(n)
	{
		case 0:		protoname = "IP"; break;
		case 1:		protoname = "ICMP"; break;
		case 6:		protoname = "TCP"; break;
		case 17:	protoname = "UDP"; break;
		case 47:	protoname = "GRE"; break;
		case 50:	protoname = "ESP"; break;
		case 51:	protoname = "AH"; break;
		case 115:	protoname = "L2TP"; break;
		default:	
					char sz[20]; 
					sprintf_s(sz, 20, "%d", n);
					protoname = sz;
					break;
	};
}

#ifdef _UNICODE
void filter::get_proto_name(int n, gstring& protoname)
{
	std::string p;
	get_proto_name(n, p);
	protoname.assign(p);
}
#endif

void filter::test_fillparams(int test_num)
{
	clear();

	m_id = 1;
	m_szFilterName.assign("test filter");
	m_color = 65535;
	m_bRevers = true;
	m_bBlocked = true;
	m_bDisable = true;
	m_bVpn = true;
	m_bExcludeCntReports = true;
	m_bExcludeUrlFiltering = true;
	m_nResetCnt = RESETCNT_DAILY;
	m_nWorkHours[2] = 0x07;
	m_nStartDay = 20100101;
	m_nEndDay = 20190531;
	m_szEmail.assign("bill@gates.com");
	m_szComment.assign("my comment");
	m_nRewMask = 2;
	m_dwRewSrcIp.from_string("192.168.5.6");
	m_dwRewDstIp.from_string("192.168.8.6");
	m_wRewSrcPort = 123;
	m_wRewDstPort = 245;
	m_nTrafficLimitMb = 777;
	m_nTrafficLimitType = 2;
	m_nTrafficLimitAction = 1;
	m_wTrafficLimitWarnLevel = 70;
	m_wTrafficLimitFlags = 1;
	m_nPktLogDest = 1;
	m_nPktLogOpt = 1;
	m_szPktLogFilename.assign("filename000");
	m_szPktLogTable.assign("public");
	m_nPktLogSqlSyntax = 1;
	m_nPktLogMaxColSize = 50;
	m_nPktLogAlertColUsage = 90;
	m_nHhLogDest = 1;
	m_szHhLogFilename.assign("filename001");
	m_szHhLogTable.assign("table2");
	m_nMaster = 1;
	m_nSpeed = 512;
	m_nSpeed2 = 256;
	m_nSpeedMaster = 128;
	m_nMasterFilterId = 10;
	m_nMasterLevelSpeed = 12;
	m_nMasterOptions = 1;

	for (int i = 0; i < 3; i++)
	{
		std::ostringstream oss;
		oss << "rule" << i;

		rule r1;
		r1.test_fillparams(0);
		r1.descr.assign(oss.str());
		rules.add_element(r1);
	}
}

}
