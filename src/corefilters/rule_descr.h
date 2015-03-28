#ifndef _UTM_RULE_DESCR_H
#define _UTM_RULE_DESCR_H

#pragma once
#include <utm.h>

#include <addrip_v4.h>
#include <addrmac.h>
#include <gstring.h>
#include <stringtools.h>
#include <rule.h>
#include <userlist.h>
#include <procnicknamelist.h>
#include <addrtablemap_v4.h>
#include <uproto.h>


namespace utm {

class rule_descr
{
public:
	rule_descr();
	~rule_descr();

	static void create_default_descr(const rule& r, const addrtablemap_v4* mat, const userlist* ulist, const procnicknamelist* procs, gstring& descr);
	static void get_proto_default_descr(int proto, gstring& descr);
	static void get_addr_default_descr(const addrtablemap_v4* mat, const userlist* ulist, const procnicknamelist* procs, unsigned int addr_type, const addrip_v4& ip, const addrip_v4& mask, const addrmac& mac, const char *hostname, unsigned int atkey, unsigned int uid, unsigned int procnick, gstring& descr);
	static void get_procname_default_descr(const procnicknamelist* procs, unsigned int procnick, gstring& descr);
	static void get_user_default_descr(bool is_user_remote, const userlist* ulist, unsigned int uid, gstring& descr);
	static void get_port_default_descr(unsigned int port_type, unsigned short port, unsigned short port_to, gstring& descr);

	static void get_limit_default_descr(unsigned int condition_type, unsigned int condition_limit, gstring& descr);
	static void get_time_default_descr(unsigned int wday, unsigned short time_from, unsigned short time_to, gstring& descr);
	static void get_mac_default_descr(unsigned int cond_mac_type, const addrmac& mac, gstring& descr);
};

}

#endif // _UTM_RULE_DESCR_H