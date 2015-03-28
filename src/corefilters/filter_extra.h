#ifndef _UTM_FILTER_EXTRA_H
#define _UTM_FILTER_EXTRA_H

#pragma once

#include "ip_header.h"
#include "hosttable.h"
#include "users_connected.h"

#include <addrtablemap_v4.h>

#define MODIFY_COUNTER_NO 1
#define MODIFY_COUNTER_YES 0

#define DIRECTION_FORWARD 0
#define DIRECTION_BACKWARD 1

#define PACKET_DIRECTION_PASSIVE 0
#define PACKET_DIRECTION_INCOMING 1
#define PACKET_DIRECTION_OUTGOING 2

#define REWRITE_DIRECTION 3
#define REWRITE_DIRECTION_NEG 0xFFFFFFFC
#define REWRITE_SRCIP 4
#define REWRITE_DSTIP 8
#define REWRITE_SRCPORT 16
#define REWRITE_DSTPORT 32
#define REWRITE_SRCMAT 64
#define REWRITE_DSTMAT 128

#define REWRITE_DIRECTMATCH 0
#define REWRITE_MIRROREDMATCH 1

#define PACKET_UNMODIFIED 0
#define PACKET_WITHNAT 1

namespace utm {

class filter2;

struct match_filter_result
{
	match_filter_result()
	{
		clear();
	}

	// filter-level results...
	bool filter_match_result;
	bool is_limit_exceeded;
	bool is_rwrfwd;
	unsigned short ip_length;
	int  direction;
	int  action;
	int  rule_no;
	unsigned int filter_id;

	// filterset-level results...
	int filterset_match_result;
	filter2* filterset_match_ptr;

	// filter-level result
	filter2* filter_ptr;

	void clear()
	{
		memset(this, 0, sizeof(match_filter_result));
	}
};

struct match_filter_input
{
	match_filter_input()
	{
		clear();
	}

	bool proc_search_done;
	bool bWithNat;

	utm::ip_header* ip;
	utm::addrtablemap_v4* mat;
	utm::hosttable* host2ip;
	utm::users_connected* user_conn;
	UINT nModifyCounter;
	int nPrevFilter;
	struct tm* lt;

	unsigned int dwProcNickIdSrc;
	unsigned int dwProcNickIdDst;
	int uidSrc;
	int uidDst;
	unsigned int nNicAlias;
	unsigned int nPacketDirection;
	unsigned int nPreCheckAddrTables;
	utm::utimestamp uptime;
	int mbytes;
	HANDLE adapter_handle;

	void clear()
	{
		memset(this, 0, sizeof(match_filter_input));
	}
};

}

#endif // _UTM_FILTER_EXTRA_H