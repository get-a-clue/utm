#include "stdafx.h"
#include "rule_descr.h"

namespace utm {

rule_descr::rule_descr()
{
}


rule_descr::~rule_descr()
{
}

void rule_descr::create_default_descr(const rule& r, const addrtablemap_v4* mat, const userlist* ulist, const procnicknamelist* procs, gstring& descr)
{
	{
		gstring str;
		get_proto_default_descr(r.proto, str);
		descr << str;
	}

	{
		gstring str;
		get_addr_default_descr(mat, ulist, procs, r.src_type, r.src_ip, r.src_mask, r.src_mac, r.src_host.get_host(), r.src_atkey, r.src_uid, r.src_procnick, str);
		descr << _U(", ") << STRLSTR("From") << _U(": ") << str;
	}

	if ((r.proto == 6) || (r.proto == 17))
	{
		gstring str;
		get_port_default_descr(r.src_port_type, r.src_port, r.src_port_to, str);
		descr << str;
	};

	{
		gstring str;
		get_addr_default_descr(mat, ulist, procs, r.dst_type, r.dst_ip, r.dst_mask, r.dst_mac, r.dst_host.get_host(), r.dst_atkey, r.dst_uid, r.dst_procnick, str);
		descr << _U("=> ") << STRLSTR("To") << _U(": ") << str;
	}

	if ((r.proto == 6) || (r.proto == 17))
	{
		gstring str;
		get_port_default_descr(r.dst_port_type, r.dst_port, r.dst_port_to, str);
		descr << str;
	};

	if (r.nicalias != NICALIAS_DEFAULT)
	{
		descr << _U(", ");
		switch (r.nicalias)
		{
			case NICALIAS_PUBLIC:
				descr << STRLSTR("Via Public NIC");
				break;

			case NICALIAS_PRIVATE:
				descr << STRLSTR("Via Private NIC");
				break;

			case NICALIAS_NETFLOW:
				descr << STRLSTR("Via Netflow collector");
				break;
		};
	};

	bool has_condition = false;

	if (r.condition_type != COND_ALWAYS)
	{
		gstring str;
		get_limit_default_descr(r.condition_type, r.condition_limit, str);
		descr << _U(", ") << STRLSTR("Condition") << _U(": ") << str;
		has_condition = true;
	};

	{
		gstring str;
		get_time_default_descr(r.wday, r.time_from, r.time_to, str);
		if (!str.empty())
		{
			descr << _U(", ");
			if (!has_condition)
			{
				descr << STRLSTR("Condition") << _U(": ");
				has_condition = true;
			}
			descr << str;
		}
	}

	{
		gstring str;
		get_mac_default_descr(r.cond_mac_type, r.cond_mac_data, str);
		if (!str.empty())
		{
			descr << _U(", ");
			if (!has_condition)
			{
				descr << STRLSTR("Condition") << _U(": ");
				has_condition = true;
			}
			descr << str;
		};
	}
}

void rule_descr::get_proto_default_descr(int proto, gstring& descr)
{
	if (proto == 0)
	{
		descr << STRLSTR("Any protocol");
		return;
	}

	descr << _U("Proto: ") << uproto::get_name(static_cast<unsigned char>(proto));
}

void rule_descr::get_addr_default_descr(const addrtablemap_v4* mat, const userlist* ulist, const procnicknamelist* procs, 
	unsigned int addr_type, const addrip_v4& ip, const addrip_v4& mask, const addrmac& mac, 
	const char *hostname, unsigned int atkey, unsigned int uid, unsigned int procnick, gstring& descr)
{
	descr.clear();
	switch (addr_type)
	{
		case RULE_MYIP:
			descr << STRLSTR("My Computer");
			break;

		case RULE_IP:
			if (mask.m_addr == 0)
			{
				descr << STRLSTR("Any address");
			}
			else
			if (mask.m_addr == 0xffffffff)
			{
				descr << STRLSTR("Host") << _U(" ") << ip.to_string();
			}
			else
			{
				descr << STRLSTR("Net") << _U(" ");
				descr << ip.to_string() << "/" << mask.to_string();
			};
			break;

		case RULE_LAN:
			descr << _U("LAN");
			break;

		case RULE_WAN:
			descr << _U("WAN");
			break;

		case RULE_MAC:
			char m[(2 * MAX_ETHER_LENGTH) + 1];
			mac.as_hex(m);
			descr << m;
			break;

		case RULE_RANGE:
			descr << STRLSTR("Range") << _U(" ");
			descr << ip.to_string() << _U("-") << mask.to_string();
			break;

		case RULE_HOST:
			descr << STRLSTR("Host") << hostname;
			break;

		case RULE_ADDRGRP:
			if (mat != NULL)
			{
				addrtablemap_v4 *m = const_cast<addrtablemap_v4 *>(mat);
				utm::addrtablemaprec<utm::addrtable_v4>* at = m->findptr_by_id(atkey);
				if (at != NULL)
				{
					descr << STRLSTR("AddrGrp");
					descr << _U(": ") << at->addrtable.GetDescr();
				};
			};
			break;

		case RULE_ADDRGRP_NO:
			if (mat != NULL)
			{
				addrtablemap_v4 *m = const_cast<addrtablemap_v4 *>(mat);
				utm::addrtablemaprec<utm::addrtable_v4>* at = m->findptr_by_id(atkey);
				if (at != NULL)
				{
					descr << STRLSTR("NotAddrGrp");
					descr << _U(": ") << at->addrtable.GetDescr();
				};
			};
			break;

		case RULE_USER:
			get_user_default_descr(true, ulist, uid, descr);
			break;

		case RULE_USER_ANY:
			descr << STRLSTR("Any authenticated remote user by agent");
			break;

		case RULE_PROCNAME:
			get_procname_default_descr(procs, procnick, descr);
			break;

		case RULE_PROCUSER:
			get_user_default_descr(false, ulist, uid, descr);
			break;
	}
}

void rule_descr::get_procname_default_descr(const procnicknamelist* procs, unsigned int procnick, gstring& descr)
{
	descr.clear();
	if ((procs == NULL) || (procs->empty()))
		return;

	for (auto iter = procs->items.begin(); iter != procs->items.end(); ++iter)
	{
		if (procnick == iter->get_id())
		{
			descr << STRLSTR("Local process");
			descr << _U(" \"") << iter->get_name() << _U("\"");
			break;
		}
	}
}

void rule_descr::get_user_default_descr(bool is_user_remote, const userlist* ulist, unsigned int uid, gstring& descr)
{
	descr.clear();
	if ((ulist == NULL) || (ulist->empty()))
		return;

	for (auto iter = ulist->items.begin(); iter != ulist->items.end(); ++iter)
	{
		if (uid == iter->get_id())
		{
			if (is_user_remote)
			{
				descr << STRLSTR("Remote user (authenticated by agent)");
			}
			else
			{
				descr << STRLSTR("Local user (process owner)");
			}

			descr << _U(" \"") << iter->get_name() << _U("\"");
			break;
		}
	}
}

void rule_descr::get_port_default_descr(unsigned int port_type, unsigned short port, unsigned short port_to, gstring& descr)
{
	descr.clear();
	switch (port_type)
	{
		case PORT_ANY:
			descr << _U("All ports");
			break;

		case PORT_EQUAL:
			descr << _U("port=") << port;
			break;

		case PORT_GREATER:
			descr << _U("port>") << port;
			break;

		case PORT_LESS:
			descr << _U("port<") << port;
			break;

		case PORT_NOTEQUAL:
			descr << _U("port!=") << port;
			break;

		case PORT_BETWEEN:
			descr << _U("port in (") << port << _U(",") << port_to << _U(")");
			break;

		case PORT_NOTBETWEEN:
			descr << _U("port out (") << port << _U(",") << port_to << _U(")");
			break;

		case PORT_EQUAL_OR:
			descr << _U("port=(") << port << _U(" or ") << port_to << _U(")");
			break;
	}
}

void rule_descr::get_limit_default_descr(unsigned int condition_type, unsigned int condition_limit, gstring& descr)
{
	char str[32];
	sprintf_s(str, 32, "%u mb", condition_limit);
	gstring lim(str);

	descr.clear();
	switch (condition_type)
	{
		case COND_SENT_LESS:
			descr << STRLSTR("Sent") << _U(" < ") << lim;
			break;

		case COND_SENT_GREATER:
			descr << STRLSTR("Sent") << _U(" > ") << lim;
			break;

		case COND_RECV_LESS:
			descr << STRLSTR("Recv") << _U(" < ") << lim;
			break;

		case COND_RECV_GREATER:
			descr << STRLSTR("Recv") << _U(" > ") << lim;
			break;

		case COND_SENT_OR_RECV_LESS:
			descr << STRLSTR("Recv") << _U(" < ") << lim << _U(" || ") << STRLSTR("Sent") << _U(" < ") << lim;
			break;

		case COND_SENT_OR_RECV_GREATER:
			descr << STRLSTR("Recv") << _U(" > ") << lim << _U(" || ") << STRLSTR("Sent") << _U(" > ") << lim;
			break;

		case COND_SENT_AND_RECV_LESS:
			descr << STRLSTR("Recv") << _U(" < ") << lim << _U(" && ") << STRLSTR("Sent") << _U(" < ") << lim;
			break;

		case COND_SENT_AND_RECV_GREATER:
			descr << STRLSTR("Recv") << _U(" > ") << lim << _U(" && ") << STRLSTR("Sent") << _U(" > ") << lim;
			break;

		case COND_SENT_PLUS_RECV_LESS:
			descr << _U("(") << STRLSTR("Recv") << _U(" + ") << STRLSTR("Sent") << _U(") < ") << lim;
			break;

		case COND_SENT_PLUS_RECV_GREATER:
			descr << _U("(") << STRLSTR("Recv") << _U(" + ") << STRLSTR("Sent") << _U(") > ") << lim;
			break;
	};
};

void rule_descr::get_time_default_descr(unsigned int wday, unsigned short time_from, unsigned short time_to, gstring& descr)
{
	descr.clear();

	if ((wday == WDAY_ALL) && (time_from == 0) && (time_to == 23))
	{
		return;
	};

	char from[16]; sprintf_s(from, 16, " %02d:00 ", time_from);
	char to[16]; sprintf_s(to, 16, " %02d:59", time_to);

	if (wday == WDAY_ALL)
	{
		descr << STRLSTR("From:") << from << STRLSTR("To:") << to;
		return;
	}

	if (wday & WDAY_MON) descr << STRLSTR("Mon") << _U(" ");
	if (wday & WDAY_TUE) descr << STRLSTR("Tue") << _U(" ");
	if (wday & WDAY_WED) descr << STRLSTR("Wed") << _U(" ");
	if (wday & WDAY_THU) descr << STRLSTR("Thu") << _U(" ");
	if (wday & WDAY_FRI) descr << STRLSTR("Fri") << _U(" ");
	if (wday & WDAY_SAT) descr << STRLSTR("Sat") << _U(" ");
	if (wday & WDAY_SUN) descr << STRLSTR("Sun") << _U(" ");

	if ((time_from != 0) || (time_to != 23))
	{
		descr << STRLSTR("From:") << from << STRLSTR("To:") << to;
	};
};

void rule_descr::get_mac_default_descr(unsigned int cond_mac_type, const addrmac& mac, gstring& descr)
{
	descr.clear();

	char m[(2 * MAX_ETHER_LENGTH) + 1];
	mac.as_hex(m);
	switch (cond_mac_type)
	{
		case COND_MAC_SRC_EQUAL:
			descr << STRLSTR("Source") << _U(" = ") << m;
			break;

		case COND_MAC_SRC_NOTEQUAL:
			descr << STRLSTR("Source") << _U(" != ") << m;
			break;

		case COND_MAC_DST_EQUAL:
			descr << STRLSTR("Dest") << _U(" = ") << m;
			break;

		case COND_MAC_DST_NOTEQUAL:
			descr << STRLSTR("Dest") << _U(" != ") << m;
			break;
	};
};

}