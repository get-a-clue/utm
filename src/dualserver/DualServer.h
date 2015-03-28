/**************************************************************************
*   Copyright (C) 2005 by Achal Dhir                                      *
*   achaldhir@gmail.com                                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
//This file defines all structures and constants
//for both DHCP and DNS Servers

#pragma once

#ifndef _DUALSERVER_H
#define _DUALSERVER_H

#ifdef NODUALSERVER
#else

#define MYBYTE unsigned char
#define MYWORD unsigned short
#define MYDWORD unsigned int

#ifdef _MSC_VER
#define strcasecmp _stricmp
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#endif

#include <string>
#include <vector>
#include <map>
using namespace std;

#include <addrmac.h>
#include <addrip_v4.h>
#include "configdhcp_statrecord.h"

typedef std::vector<utm::configdhcp_statrecord> statrec_container;
typedef std::vector<utm::configdhcp_statrecord>::iterator statrec_iterator;

#ifndef PRG_NAME

#include "DnsWildRecord.h"

#else

#include <DnsWildRecord.h>

#endif

#ifndef _CALLBACKLOGMESS_
#define _CALLBACKLOGMESS_
typedef void (*pCallbackLogMess)(char *, int);
#endif

namespace utm {

#define MAX_CONFIG_SECTION 16380
#define MAX_INI_STRING 8192

#define MAX_SERVERS 125
#define MAX_RANGES 125

#define my_inet_addr inet_addr

#define RCODE_NOERROR 		0
#define RCODE_FORMATERROR	1
#define RCODE_SERVERFAIL	2
#define RCODE_NAMEERROR		3
#define RCODE_NOTIMPL 		4
#define RCODE_REFUSED 		5
#define RCODE_YXDOMAIN 		6
#define RCODE_YXRRSET 		7
#define RCODE_NXRRSET 		8
#define RCODE_NOTAUTH 		9
#define RCODE_NOTZONE 		10

#define OPCODE_STANDARD_QUERY	0
#define OPCODE_INVERSE_QUERY	1
#define OPCODE_SRVR_STAT_REQ	2
#define OPCODE_NOTIFY	4
#define OPCODE_DYNAMIC_UPDATE	5

#ifndef DNS_TYPE_A
#define DNS_TYPE_A		1
#endif

#ifndef DNS_TYPE_NS
#define DNS_TYPE_NS		2
#endif

#ifndef DNS_TYPE_MD
#define DNS_TYPE_MD		3
#endif

#ifndef DNS_TYPE_MF
#define DNS_TYPE_MF		4
#endif

#ifndef DNS_TYPE_CNAME
#define DNS_TYPE_CNAME	5
#endif

#ifndef DNS_TYPE_SOA
#define DNS_TYPE_SOA	6
#endif

#ifndef DNS_TYPE_MB
#define DNS_TYPE_MB		7
#endif

#ifndef DNS_TYPE_MG
#define DNS_TYPE_MG		8
#endif

#ifndef DNS_TYPE_MR
#define DNS_TYPE_MR		9
#endif

#ifndef DNS_TYPE_NULL
#define DNS_TYPE_NULL	10
#endif

#ifndef DNS_TYPE_WKS
#define DNS_TYPE_WKS	11
#endif

#ifndef DNS_TYPE_PTR
#define DNS_TYPE_PTR	12
#endif

#ifndef DNS_TYPE_HINFO
#define DNS_TYPE_HINFO	13
#endif

#ifndef DNS_TYPE_MINFO
#define DNS_TYPE_MINFO	14
#endif

#ifndef DNS_TYPE_MX
#define DNS_TYPE_MX		15
#endif

#ifndef DNS_TYPE_TXT
#define DNS_TYPE_TXT	16
#endif

#ifndef DNS_TYPE_AAAA
#define DNS_TYPE_AAAA	28
#endif

#ifndef DNS_TYPE_IXFR
#define DNS_TYPE_IXFR	251
#endif

#ifndef DNS_TYPE_AXFR
#define DNS_TYPE_AXFR	252
#endif

#ifndef DNS_TYPE_MAILB
#define DNS_TYPE_MAILB	253
#endif

#ifndef DNS_TYPE_MAILA
#define DNS_TYPE_MAILA	254
#endif

#ifndef DNS_TYPE_ANY
#define DNS_TYPE_ANY	255
#endif

#ifndef DNS_CLASS_IN
#define DNS_CLASS_IN	1
#endif

#ifndef DNS_CLASS_CS
#define DNS_CLASS_CS	2
#endif

#ifndef DNS_CLASS_CH
#define DNS_CLASS_CH	3
#endif

#ifndef DNS_CLASS_HS
#define DNS_CLASS_HS	4
#endif

#ifndef DNS_CLASS_NONE
#define DNS_CLASS_NONE	254
#endif

#ifndef DNS_CLASS_ANY
#define DNS_CLASS_ANY	255
#endif

#define IPPORT_DNS 53

typedef map<string, string> String2String;

struct dnsHeader
{
	unsigned xid :16;	//query identification number
	/* byte boundry */
	unsigned rd: 1;		//recursion desired
	unsigned tc: 1;		//truncated message
	unsigned aa: 1;		//authoritive answer
	unsigned opcode: 4;	//option code
	unsigned qr: 1;		//response flag
	/* byte boundry */
	unsigned rcode :4;	//response code
	unsigned cd: 1;		//checking disabled by resolver
	unsigned at: 1;		//authentic data from named
	unsigned unused :1;	//unused
	unsigned ra: 1;		//recursion available
	/* byte boundry */
	unsigned qdcount :16;	//number of question entries
	unsigned ancount :16;	//number of answer entries
	unsigned nscount :16;	//number of authority entries
	unsigned adcount :16;	//number of additional entries
};

/*
struct dnsHeader
{
	unsigned xid :16;	// query identification number
	unsigned qr: 1;		// response flag
	unsigned opcode: 4;	// purpose of message
	unsigned aa: 1;		// authoritive answer
	unsigned tc: 1;		// truncated message
	unsigned rd: 1;		// recursion desired
	// byte boundry 	// fields in fourth byte
	unsigned ra: 1;		// recursion available
	unsigned unused :1;	// unused bits (MBZ as of 4.9.3a3)
	unsigned at: 1;		// authentic data from named
	unsigned cd: 1;		// checking disabled by resolver
	unsigned rcode :4;	// response code
	// byte boundry 	// remaining bytes
	union {
		struct {
			MYWORD qdcount;
			MYWORD ancount;
			MYWORD nscount;
			MYWORD adcount;
		};
		struct {
			MYWORD zcount;
			MYWORD prcount;
			MYWORD ucount;
			MYWORD arcount;
		};
	};
};
*/

struct dnsPacket
{
	struct dnsHeader header;
	char data;
};

struct data6
{
	char zone[256];
	MYWORD zLen;
	MYDWORD dns[2];
	MYBYTE currentDNS;
	MYBYTE lastDNS;
};

struct data16
{
	char wildcard[256];
	MYDWORD ip;
};

struct data7 //cache
{
	char *mapname;
	time_t expiry;
	union
	{
		struct
		{
			MYBYTE dataType;
			MYBYTE sockInd;
			MYBYTE dnsIndex;
			MYBYTE isZone;
		};
		struct
		{
			unsigned no_route: 1;
			unsigned fixed: 1;
			unsigned bitmask: 5;
			unsigned local: 1;
			char rangeInd;
			MYWORD dhcpInd;
		};
	};
	union
	{
		int bytes;
		SOCKADDR_IN *addr;
		MYBYTE *options;
		MYDWORD serial;
		MYDWORD source;
	};
	union
	{
		MYBYTE *response;
		char *hostname;
		char *query;
		MYDWORD ip;
	};
};

typedef multimap<string, data7*> hostMap;
typedef multimap<time_t, data7*> expiryMap;

struct data5 //dns request
{
	dnsPacket *dnsp;
	char raw[2048];
	char temp[2048];
	char *dp;
	char query[256];
	char cname[256];
	char mapname[256];
	data7 cache;
	hostMap::iterator iterBegin;
	SOCKET sock;
	SOCKADDR_IN addr;
	SOCKADDR_IN remote;
	socklen_t sockLen;
	linger ling;
	int bytes;
	MYWORD qLen;
	MYWORD qtype;
	MYWORD qclass;
	MYBYTE dnType;
	MYBYTE sockInd;
	MYBYTE dnsInd;
};

enum
{
	NONE,
	LOCAL_A,
	LOCAL_PTR_AUTH,
	LOCAL_PTR_NAUTH,
	LOCALHOST_A,
	LOCALHOST_PTR,
	SERVER_A,
	SERVER_PTR_AUTH,
	SERVER_A_NAUTH,
	SERVER_PTR_NAUTH,
	LOCAL_CNAME,
	EXT_CNAME,
	STATIC_A_AUTH,
	STATIC_PTR_AUTH,
	STATIC_A_NAUTH,
	STATIC_PTR_NAUTH,
	ZONE,
	NS,
	SOA,
	AXFR,
	CACHED,
	NON_CACHED,
	QUEUE,
	DNS_CHECK,
	//QUERY_TYPE
	DNTYPE_A_EXT,
	DNTYPE_A_BARE,
	DNTYPE_A_LOCAL,
	DNTYPE_A_ZONE,
	DNTYPE_P_EXT,
	DNTYPE_P_LOCAL,
	DNTYPE_P_ZONE,
	DNTYPE_SUBZONE
};

struct data12 //dns range
{
	MYDWORD rangeStart;
	MYDWORD rangeEnd;
};

struct dns_rr
{
	char *name;
	MYWORD type, _class;
	MYDWORD ttl;
	MYWORD rdlength;
	char *rdata;
	union {
		struct
		{
			long address;
		} a;
		struct
		{
			char *cname;
		} cname;
		struct
		{
			char *cpu, *os;
		} hinfo;
		struct
		{
			char *madname;
		} mb;
		struct
		{
			char *madname;
		} md;
		struct
		{
			char *madname;
		} mf;
		struct
		{
			char *mgmname;
		} mg;
		struct
		{
			char *rmailbx, *emailbx;
		} minfo;
		struct
		{
			char *newname;
		} mr;
		struct
		{
			int preference;
			char *exchange;
		} mx;
		struct
		{
			char *nsdname;
		} ns;
		struct
		{
			char *data;
		} null;
		struct
		{
			char *ptrdname;
		} ptr;
		struct
		{
			char *mname, *rname;
			unsigned serial, refresh, retry, expire, minimum;
		} soa;
		struct
		{
			char **txt_data;
		} txt;
		struct
		{
			int address;
			MYBYTE protocol;
			int bitmapsize;
			char *bitmap;
		} wks;
	} data;
};

struct data11 //mx
{
	char hostname[256];
	MYWORD pref;
};

struct ConnType
{
	SOCKET sock;
	SOCKADDR_IN addr;
	MYDWORD server;
	MYWORD port;
	bool loaded;
	bool ready;
};

#define BOOTP_REQUEST  1
#define BOOTP_REPLY    2

#define DHCP_MESS_NONE       0
#define DHCP_MESS_DISCOVER   1
#define DHCP_MESS_OFFER      2
#define DHCP_MESS_REQUEST	 3
#define DHCP_MESS_DECLINE	 4
#define DHCP_MESS_ACK		 5
#define DHCP_MESS_NAK		 6
#define DHCP_MESS_RELEASE    7
#define DHCP_MESS_INFORM	 8


// DHCP OPTIONS
#define DHCP_OPTION_PAD						0
#define DHCP_OPTION_NETMASK          		1
#define DHCP_OPTION_TIMEOFFSET       		2
#define DHCP_OPTION_ROUTER           		3
#define DHCP_OPTION_TIMESERVER       		4
#define DHCP_OPTION_NAMESERVER       		5
#define DHCP_OPTION_DNS              		6
#define DHCP_OPTION_LOGSERVER        		7
#define DHCP_OPTION_COOKIESERVER     		8
#define DHCP_OPTION_LPRSERVER        		9
#define DHCP_OPTION_IMPRESSSERVER    		10
#define DHCP_OPTION_RESLOCSERVER     		11
#define DHCP_OPTION_HOSTNAME         		12
#define DHCP_OPTION_BOOTFILESIZE     		13
#define DHCP_OPTION_MERITDUMP        		14
#define DHCP_OPTION_DOMAINNAME       		15
#define DHCP_OPTION_SWAPSERVER       		16
#define DHCP_OPTION_ROOTPATH         		17
#define DHCP_OPTION_EXTSPATH         		18
#define DHCP_OPTION_IPFORWARD        		19
#define DHCP_OPTION_NONLOCALSR       		20
#define DHCP_OPTION_POLICYFILTER     		21
#define DHCP_OPTION_MAXREASSEMBLE    		22
#define DHCP_OPTION_IPTTL            		23
#define DHCP_OPTION_PATHMTUAGING     		24
#define DHCP_OPTION_PATHMTUPLATEAU   		25
#define DHCP_OPTION_INTERFACEMTU     		26
#define DHCP_OPTION_SUBNETSLOCAL     		27
#define DHCP_OPTION_BCASTADDRESS     		28
#define DHCP_OPTION_MASKDISCOVERY    		29
#define DHCP_OPTION_MASKSUPPLIER     		30
#define DHCP_OPTION_ROUTERDISCOVERY  		31
#define DHCP_OPTION_ROUTERSOLIC      		32
#define DHCP_OPTION_STATICROUTE      		33
#define DHCP_OPTION_TRAILERENCAPS    		34
#define DHCP_OPTION_ARPTIMEOUT       		35
#define DHCP_OPTION_ETHERNETENCAPS   		36
#define DHCP_OPTION_TCPTTL           		37
#define DHCP_OPTION_TCPKEEPALIVEINT  		38
#define DHCP_OPTION_TCPKEEPALIVEGRBG 		39
#define DHCP_OPTION_NISDOMAIN        		40
#define DHCP_OPTION_NISSERVERS       		41
#define DHCP_OPTION_NTPSERVERS       		42
#define DHCP_OPTION_VENDORSPECIFIC   		43
#define DHCP_OPTION_NETBIOSNAMESERV  		44
#define DHCP_OPTION_NETBIOSDGDIST    		45
#define DHCP_OPTION_NETBIOSNODETYPE  		46
#define DHCP_OPTION_NETBIOSSCOPE     		47
#define DHCP_OPTION_X11FONTS         		48
#define DHCP_OPTION_X11DISPLAYMNGR   		49
#define DHCP_OPTION_REQUESTEDIPADDR  		50
#define DHCP_OPTION_IPADDRLEASE      		51
#define DHCP_OPTION_OVERLOAD         		52
#define DHCP_OPTION_MESSAGETYPE      		53
#define DHCP_OPTION_SERVERID         		54
#define DHCP_OPTION_PARAMREQLIST     		55
#define DHCP_OPTION_MESSAGE          		56
#define DHCP_OPTION_MAXDHCPMSGSIZE   		57
#define DHCP_OPTION_RENEWALTIME      		58
#define DHCP_OPTION_REBINDINGTIME    		59
#define DHCP_OPTION_VENDORCLASSID    		60
#define DHCP_OPTION_CLIENTID         		61
#define DHCP_OPTION_NISPLUSDOMAIN    		64
#define DHCP_OPTION_NISPLUSSERVERS   		65
#define DHCP_OPTION_TFTPSERVER       		66
#define DHCP_OPTION_BOOTFILE         		67
#define DHCP_OPTION_MOBILEIPHOME     		68
#define DHCP_OPTION_SMTPSERVER       		69
#define DHCP_OPTION_POP3SERVER       		70
#define DHCP_OPTION_NNTPSERVER       		71
#define DHCP_OPTION_WWWSERVER        		72
#define DHCP_OPTION_FINGERSERVER     		73
#define DHCP_OPTION_IRCSERVER        		74
#define DHCP_OPTION_STSERVER         		75
#define DHCP_OPTION_STDASERVER       		76
#define DHCP_OPTION_USERCLASS        		77
#define DHCP_OPTION_SLPDIRAGENT      		78
#define DHCP_OPTION_SLPDIRSCOPE      		79
#define DHCP_OPTION_CLIENTFQDN       		81
#define DHCP_OPTION_RELAYAGENTINFO     		82
#define DHCP_OPTION_NDSSERVERS       		85
#define DHCP_OPTION_NDSTREENAME      		86
#define DHCP_OPTION_NDSCONTEXT		 		87
#define DHCP_OPTION_AUTHENTICATION			90
#define DHCP_OPTION_CLIENTSYSTEM			93
#define DHCP_OPTION_CLIENTNDI				94
#define DHCP_OPTION_LDAP					95
#define DHCP_OPTION_UUID_GUID				97
#define DHCP_OPTION_USER_AUTH				98
#define DHCP_OPTION_NETINFOADDRESS			112
#define DHCP_OPTION_NETINFOTAG				113
#define DHCP_OPTION_URL						114
#define DHCP_OPTION_AUTO_CONFIG				116
#define DHCP_OPTION_NAMESERVICESEARCH		117
#define DHCP_OPTION_SUBNETSELECTION			118
#define DHCP_OPTION_DOMAINSEARCH			119
#define DHCP_OPTION_SIPSERVERSDHCP			120
#define DHCP_OPTION_CLASSLESSSTATICROUTE	121
#define DHCP_OPTION_CCC						122
#define DHCP_OPTION_GEOCONF					123
#define DHCP_OPTION_V_IVENDORCLASS			124
#define DHCP_OPTION_V_IVENDOR_SPECIFIC		125
#define DHCP_OPTION_TFPTSERVERIPADDRESS		128
#define DHCP_OPTION_CALLSERVERIPADDRESS		129
#define DHCP_OPTION_DISCRIMINATIONSTRING	130
#define DHCP_OPTION_REMOTESTATISTICSSERVER	131
#define DHCP_OPTION_802_1PVLANID			132
#define DHCP_OPTION_802_1QL2PRIORITY		133
#define DHCP_OPTION_DIFFSERVCODEPOINT		134
#define DHCP_OPTION_HTTPPROXYFORPHONE_SPEC	135
#define DHCP_OPTION_SERIAL					253
#define DHCP_OPTION_NEXTSERVER				254
#define DHCP_OPTION_END						255

//#define DHCP_VENDORDATA_SIZE		 272
//#define DHCP_VENDORDATA_SIZE		 64
//#define DHCP_VENDORDATA_SIZE		 784
//#define DHCP_PACKET_SIZE			1024
//#define DHCP_MIN_SIZE				 44
//#define DHCP_MAX_CLIENTS			 254
#define IPPORT_DHCPS   67
#define IPPORT_DHCPC   68
#define VM_STANFORD  0x5354414EUL
#define VM_RFC1048   0x63825363UL

typedef map<string, data7*> dhcpMap;

struct data3
{
	MYBYTE opt_code;
	MYBYTE size;
	MYBYTE value[256];
};

struct dhcp_header
{
	MYBYTE bp_op;
	MYBYTE bp_htype;
	MYBYTE bp_hlen;
	MYBYTE bp_hops;
	MYDWORD bp_xid;
	MYWORD bp_secs;
	MYBYTE bp_broadcast;
	MYBYTE bp_spare;
	MYDWORD bp_ciaddr;
	MYDWORD bp_yiaddr;
	MYDWORD bp_siaddr;
	MYDWORD bp_giaddr;
	MYBYTE bp_chaddr[16];
	char bp_sname[64];
	MYBYTE bp_file[128];
	MYBYTE bp_magic_num[4];
};

struct dhcp_packet
{
	dhcp_header header;
	MYBYTE vend_data[1024 - sizeof(dhcp_header)];
};

struct data13 //dhcp range
{
	MYBYTE rangeSetInd;
	MYDWORD rangeStart;
	MYDWORD rangeEnd;
	MYDWORD mask;
	MYBYTE *options;
	time_t *expiry;
	data7 **dhcpEntry;
};

struct data14 //rangeSet
{
	MYBYTE active;
	MYBYTE *macStart[32];
	MYBYTE *macEnd[32];
	MYBYTE macSize[32];
	MYBYTE *vendClass[32];
	MYBYTE vendClassSize[32];
	MYBYTE *userClass[32];
	MYBYTE userClassSize[32];
};

struct data19
{
	SOCKET sock;
	SOCKADDR_IN remote;
	socklen_t sockLen;
	linger ling;
	int bytes;
	char *buffer;
};

struct data20
{
	MYBYTE *options;
	MYDWORD ip;
	MYDWORD mask;
	MYBYTE rangeSetInd;
};

struct data9 //dhcpRequst
{
	MYDWORD lease;
	union
	{
		char raw[sizeof(dhcp_packet)];
		dhcp_packet dhcpp;
	};
	char hostname[256];
	MYDWORD server;
	MYDWORD reqIP;
	int bytes;
	SOCKADDR_IN remote;
	socklen_t sockLen;
	MYWORD messsize;
	MYBYTE *vp;
	data7 *dhcpEntry;
	data3 agentOption;
	data3 clientId;
	MYDWORD serial;
	MYBYTE paramreqlist[256];
	MYBYTE opAdded[256];
	MYBYTE vendClass[256];
	MYBYTE userClass[256];
	MYBYTE vendClassSize;
	MYBYTE userClassSize;
	MYBYTE req_type;
	MYBYTE resp_type;
	MYBYTE sockInd;
};

struct DhcpConnType
{
	SOCKET sock;
	SOCKADDR_IN addr;
	MYDWORD server;
	MYWORD port;
	MYDWORD mask;
	int broadCastVal;
	int broadCastSize;
	int reUseVal;
	int reUseSize;
	int donotRouteVal;
	int donotRouteSize;
	bool loaded;
	bool ready;
};

struct data4
{
	char opName[40];
	MYBYTE opTag;
	MYBYTE opType;
};

struct data15
{
	union
	{
		//MYDWORD ip;
		unsigned ip:32;
		MYBYTE octate[4];
	};
};

struct data8 //client
{
	MYWORD dhcpInd;
	MYBYTE bp_hlen;
	MYBYTE local;
	MYDWORD source;
	MYDWORD ip;
	time_t expiry;
	MYBYTE bp_chaddr[16];
	char hostname[64];
};

struct data1
{
	DhcpConnType dhcpConn[MAX_SERVERS];
	ConnType dhcpReplConn;
	ConnType dnsUdpConn[MAX_SERVERS];
	ConnType forwConn;
	ConnType dnsTcpConn[MAX_SERVERS];
	ConnType httpConn;
	MYDWORD allServers[MAX_SERVERS];
	MYDWORD listenServers[MAX_SERVERS];
	MYDWORD listenMasks[MAX_SERVERS];
	MYDWORD staticServers[MAX_SERVERS];
	MYDWORD staticMasks[MAX_SERVERS];
	MYDWORD dns[MAX_SERVERS];
	SOCKET maxFD;
	MYBYTE currentDNS;
	bool ready;
	bool busy;
};

struct data2
{
	WSADATA wsaData;
	char zone[256];
	MYBYTE zLen;
	char authoritySmall[256];
	char authority[256];
	MYBYTE aLen;
	char ns[256];
	MYDWORD nsIP;
	char servername[128];
	char servername_fqn[256];
	MYDWORD zoneServers[MAX_SERVERS];
	data11 mxServers[2][5];
	MYBYTE mxCount[2];
	MYDWORD mask;
	MYDWORD lease;
	MYDWORD serial1;
	MYDWORD serial2;
	MYDWORD refresh;
	MYDWORD retry;
	MYDWORD expire;
	MYDWORD minimum;
	MYWORD minCache;
	MYWORD maxCache;
	MYDWORD dhcpSize;
	time_t expireTime;
	data6 dnsRoutes[32];
	data16 wildHosts[32];
	data12 dnsRanges[32];
	data13 dhcpRanges[MAX_RANGES];
	data14 rangeSet[MAX_RANGES];
	MYBYTE macArray[MAX_RANGES];
	MYBYTE vendArray[MAX_RANGES];
	MYBYTE userArray[MAX_RANGES];
	MYBYTE hasFilter;
	MYDWORD rangeStart;
	MYDWORD rangeEnd;
	MYBYTE *options;
	MYWORD dhcpIndex;
	FILE *logfile;
	MYDWORD failureCount;
	MYBYTE dhcpLogLevel;
	MYBYTE dnsLogLevel;
	MYBYTE authorized;
	MYBYTE replication;
};

#ifdef _DUALSERVERDEBUG
class __declspec(dllexport) CDualServer
#else
class CDualServer
#endif

{

public:
    CDualServer();
    ~CDualServer();


public:

//    HANDLE hStopThreadEvent;
	BOOL m_bLoop;
	TCHAR *m_pszFlagFolder;

	//Global Variables
	bool kRunning;
	HANDLE stopServiceEvent;
	data1 network;
	data1 newNetwork;
	data2 cfig;
	MYBYTE cacheInd;
	MYBYTE newInd;
	hostMap dnsCache[2];
	dhcpMap dhcpCache;
	expiryMap dnsAge[2];
	expiryMap dhcpAge;
	char tempbuff[512];
	char extbuff[512];
	char logBuff[256];
	bool verbatim;
//	char iniFile[_MAX_PATH];
	TCHAR leaFile[_MAX_PATH];
//	char logFile[_MAX_PATH];
//	char htmFile[_MAX_PATH];
//	char lnkFile[_MAX_PATH];
	MYWORD loggingDay;
	char arpa[20];
	BOOL dhcpService;
	BOOL dnsService;
	timeval tv;
	fd_set readfds;
	fd_set writefds;
	HANDLE fEvent;
//HANDLE lEvent;

public:

//Function Prototypes
bool addServer(MYDWORD*, MYDWORD);
bool chkQu(char*);
bool checkMask(MYDWORD);
//bool checkRange(MYBYTE, bool, bool, bool);
bool checkRange(char rangeInd, bool macFound, bool vendFound, bool userFound);
bool detectChange();
bool getSection(const char*, char*, MYBYTE, char*);
bool isInt(char*);
bool isIP(char*);
bool wildcmp(char*, char*);
BOOL openSection(const char *sectionName, MYBYTE serial, std::stringstream *pss);
MYBYTE fromBase64(MYBYTE*, char*);
MYBYTE fromUUE(MYBYTE*, char*);
MYBYTE getBaseValue(MYBYTE);
MYBYTE makeLocal(char*);
MYBYTE makeLocal(MYDWORD);
MYBYTE pIP(void*, MYDWORD);
MYBYTE pULong(void*, MYDWORD);
MYBYTE pUShort(void*, MYWORD);
char getRangeInd(MYDWORD);
char* myTrim(char*, char*);
char* getServerName(char*, MYDWORD);
char* myGetToken(char*, MYBYTE);
char* cloneString(char*);
char* getHexValue(MYBYTE*, char*, MYBYTE*);
char* getResult(data5*);
char* hex2String(char*, MYBYTE*, MYBYTE, char);
char* IP2String(char*, MYDWORD);
char* IP2arpa(char*, MYDWORD);
char* IP62String(char*, MYBYTE*);
char* myUpper(char* string);
char* myLower(char* string);
void parseDhcpOptions(char *sectionBody, const char *sectionName, data20 *optionData);
char* readSection(char*, std::stringstream *pss);
char* toBase64(MYBYTE*, MYBYTE);
char* toUUE(char*, MYBYTE*, MYBYTE);
char* strqtype(char*, MYBYTE);
data7* findEntry(MYBYTE, char*);
data7* findEntry(MYBYTE, char*, MYBYTE);
MYDWORD alad(data9*);
MYDWORD calcMask(MYDWORD, MYDWORD);
MYDWORD chad(data9*);
MYDWORD getClassNetwork(MYDWORD);
MYDWORD getZone(MYBYTE, char*);
MYDWORD getSerial(char*);
MYDWORD resad(data9*);
MYDWORD sdmess(data9*);
MYDWORD sendRepl(data9 *req);
MYDWORD sendRepl(data7 *dhcpEntry);
MYDWORD updateDHCP(data9*);
MYDWORD* findServer(MYDWORD*, MYDWORD);
int getIndex(char, MYDWORD);
void addAuth(data5*);
void addDHCPRange(char*);
void addMacRange(MYBYTE, char*);
void addVendClass(MYBYTE, char*, MYBYTE);
void addUserClass(MYBYTE, char*, MYBYTE);
void addEntry(MYBYTE, data7*);
void addOptions(data9*);
void addRRAny(data5 *req, bool adFlag);
void addRRA(data5*);
void addRRAOne(data5*);
void addRRPtrOne(data5*);
void addRRAd(data5*);
void addRRCNOne(data5*);
void addRRExt(data5*);
void addRRLocalhostA(data5*);
void addRRLocalhostPtr(data5*);
void addRRWildA(data5*, MYDWORD);
void addRRMX(data5*);
void addRRMXOne(data5*, MYBYTE);
void addRRNone(data5*);
void addRRNSA(data5*);
void addRRNSPtr(data5*);
void addRRPtr(data5*);
void addRRServerA(data5*);
void addRRSOA(data5*, MYDWORD);
void addRRSTAOne(data5*);
//void addToCache(MYBYTE, MYBYTE, char*, MYDWORD, time_t, MYBYTE, MYBYTE, MYDWORD);
void addToCache(MYBYTE ind, char *hostname, MYDWORD ip, time_t expiry, MYBYTE aType, MYBYTE pType, MYDWORD serial);
void calcRangeLimits(MYDWORD, MYDWORD, MYDWORD*, MYDWORD*);
void checkSize(MYBYTE);
void checkDNS(MYBYTE);
void closeConn();
void delDnsEntry(MYBYTE, data7*);
void getInterfaces(data1*);
void getSecondary();
void init(void*,int*);
void lockOptions(std::stringstream *pss);
void readOptions(char *p, const char*, data20*);
void logMess(char*, MYBYTE);
void logDHCPMess(char*, MYBYTE);
void logDNSMess(char*, MYBYTE);
void logDNSMess(data5*, char*, MYBYTE);
void logTCPMess(data5*, char*, MYBYTE);
void mySplit(char*, char*, char*, char);
void sendHTTP(void*);
void procTCP(data5*);

void GetActiveLeases(string *ps);
#ifdef _DEBUG
void procHTTP(data19*);
#endif

void pvdata(data9*, data3*);
void recvRepl(data9*);
void checkZone(void*);
void runProg();
void showError(MYDWORD);
void setLeaseExpiry(MYDWORD, time_t);
void setLeaseExpiry(data7*, time_t, bool);
void updateStateFile(void*);
MYWORD fdnmess(data5*);
MYWORD fQu(char*, dnsPacket*, char*);
MYWORD frdnmess(data5*);
MYWORD fUShort(void*);
MYDWORD fULong(void *raw);
MYDWORD fIP(void *raw);
MYWORD gdmess(data9*, MYBYTE);
MYWORD gdnmess(data5*, MYBYTE);
MYWORD listSections(char*, int, char*);
MYWORD recvTcpDnsMess(SOCKET, char*, bool);
MYWORD myTokenize(char*, char*, const char*, bool);
MYWORD pQu(char*, char*);
MYWORD qLen(char*);
MYWORD scanloc(data5*);
MYWORD sdnmess(data5*);
MYWORD sendTCPmess(data5 *req);

public:
    void GetDnsAllowedHosts();
//    void GetDnsHostsRecords();
    void GetDnsForwardingServers();
    void GetDnsWildHosts();

//	void GetDhcpOptions();
//	void GetDhcpRange();

public:

	BOOL m_bUseDnsCache;
	BOOL m_bUseDnsForwarders;
    pCallbackLogMess pclog;
    WORD DnsServerPort;
    int default_loglevel;

	char *pAlexDnsServers;
	char *pAlexDnsAllowedHosts;
	char *pAlexDnsWildHosts;

	char *pAlexDhcpOptions;
	char *pAlexDhcpRange;

//	map<__int64, string> AlexDhcpStaticHosts;
	statrec_container AlexDhcpStaticHosts;
	DWORD *pIpNoListenOn;

	CDnsRecords* m_pWildDns;

public:
    void MySetDnsServerPort(int port);
    void MyServiceDualMain();
    void MyPreInit();

    void MySetDnsServers(char* p);
    void MySetDnsAllowedHosts(char *p);
    void MySetDnsHostsRecords(char *p);
    void MySetDnsWildHosts(char *p);

	void MySetDhcpOptions(char *p);
	void MySetDhcpRange(char *p);

    void MySetCallbackLogMess(pCallbackLogMess cb);
    void MySetDefaultLogLevel(int level);

	char* myCleanToken(char* buff, BYTE index);
};

//constants
const char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char send200[] = "HTTP/1.1 200 OK\r\nDate: %s\r\nLast-Modified: %s\r\nContent-Type: text/html\r\nConnection: Close\r\nContent-Length:         \r\n\r\n";
const char line200[] = "<td>%s</td>";
const char sVersion[] = "Dual DHCP DNS Server Version 6.80 Windows Build 6801";
const data4 opData[] =
    {
        {"IP_Addr", 0, 3},
        {"DHCP_Range", 0, 1},
//        {"Authorized", 0, 6},
        {"SubNet_Mask", DHCP_OPTION_NETMASK, 3},
        {"Time_Offset", DHCP_OPTION_TIMEOFFSET, 4},
        {"Router", DHCP_OPTION_ROUTER, 3},
        {"Time_Server", DHCP_OPTION_TIMESERVER, 3},
        {"Name_Server", DHCP_OPTION_NAMESERVER, 3},
        {"DNS_Server", DHCP_OPTION_DNS, 3},
        {"Log_Server", DHCP_OPTION_LOGSERVER, 3},
        {"Cookie_Server", DHCP_OPTION_COOKIESERVER, 3},
        {"LPR_Server", DHCP_OPTION_LPRSERVER, 3},
        {"Impress_Server", DHCP_OPTION_IMPRESSSERVER, 3},
        {"RLP_Server", DHCP_OPTION_RESLOCSERVER, 3},
        {"Hostname", DHCP_OPTION_HOSTNAME, 1},
        {"Boot_File_Size", DHCP_OPTION_BOOTFILESIZE, 5},
//        {"Domain_Name", DHCP_OPTION_DOMAINNAME, 1},
        {"Swap_Server", DHCP_OPTION_SWAPSERVER, 3},
        {"Root_Path", DHCP_OPTION_ROOTPATH, 1},
        {"Extension_Path", DHCP_OPTION_EXTSPATH, 1},
        {"IP_Forward_On/Off", DHCP_OPTION_IPFORWARD, 7},
        {"SrcRte_On/Off", DHCP_OPTION_NONLOCALSR, 7},
        {"Policy_Filter", DHCP_OPTION_POLICYFILTER, 8},
        {"Default_IP_TTL", DHCP_OPTION_IPTTL, 6},
        {"MTU_Timeout", DHCP_OPTION_PATHMTUAGING, 4},
        {"MTU_Plateau", DHCP_OPTION_PATHMTUPLATEAU, 2},
        {"MTU_Interface", DHCP_OPTION_INTERFACEMTU, 5},
        {"All_Subnet_Local", DHCP_OPTION_SUBNETSLOCAL, 7},
        {"Broadcast_Address", DHCP_OPTION_BCASTADDRESS, 3},
        {"Mask_Discovery", DHCP_OPTION_MASKDISCOVERY, 7},
        {"Mask_Supplier", DHCP_OPTION_MASKSUPPLIER, 7},
        {"Router_Discovery", DHCP_OPTION_ROUTERDISCOVERY, 7},
        {"Router_Soli_Address", DHCP_OPTION_ROUTERSOLIC, 3},
        {"Static_Routes", DHCP_OPTION_STATICROUTE, 8},
        {"Trailers", DHCP_OPTION_TRAILERENCAPS, 7},
        {"ARP_Timeout", DHCP_OPTION_ARPTIMEOUT, 4},
        {"Ethernet_Encp", DHCP_OPTION_ETHERNETENCAPS, 7},
        {"Default_TCP_TTL", DHCP_OPTION_TCPTTL, 6},
        {"Keepalive_Time", DHCP_OPTION_TCPKEEPALIVEINT, 4},
        {"Keepalive_Garbage", DHCP_OPTION_TCPKEEPALIVEGRBG, 7},
        {"NIS_Domain", DHCP_OPTION_NISDOMAIN, 1},
        {"NIS_Servers", DHCP_OPTION_NISSERVERS, 3},
        {"NTP_Servers", DHCP_OPTION_NTPSERVERS, 3},
        {"NETBIOS_Name_Srv", DHCP_OPTION_NETBIOSNAMESERV, 3},
        {"NETBIOS_Dist_Srv", DHCP_OPTION_NETBIOSDGDIST, 3},
        {"NETBIOS_Node_Type", DHCP_OPTION_NETBIOSNODETYPE, 6},
        {"NETBIOS_Scope", DHCP_OPTION_NETBIOSSCOPE, 1},
        {"X_Window_Font", DHCP_OPTION_X11FONTS, 3},
        {"X_Window_Manager", DHCP_OPTION_X11DISPLAYMNGR, 3},
        {"Lease_Time", DHCP_OPTION_IPADDRLEASE, 4},
        {"Renewal_Time", DHCP_OPTION_RENEWALTIME, 4},
        {"Rebinding_Time", DHCP_OPTION_REBINDINGTIME, 4},
        {"Netware/IP_Domain", 62, 1},
        {"Netware/IP_Options", 63, 2},
        {"NIS+_Domain_Name", DHCP_OPTION_NISPLUSDOMAIN, 1},
        {"NIS+_Server_Addr", DHCP_OPTION_NISPLUSSERVERS, 3},
        {"TFTP_Server_Name", DHCP_OPTION_TFTPSERVER, 1},
        {"Boot_File", DHCP_OPTION_BOOTFILE, 1},
        {"Home_Agent_Addrs", DHCP_OPTION_MOBILEIPHOME, 3},
        {"SMTP_Server", DHCP_OPTION_SMTPSERVER, 3},
        {"POP3_Server", DHCP_OPTION_POP3SERVER, 3},
        {"NNTP_Server", DHCP_OPTION_NNTPSERVER, 3},
        {"WWW_Server", DHCP_OPTION_WWWSERVER, 3},
        {"Finger_Server", DHCP_OPTION_FINGERSERVER, 3},
        {"IRC_Server", DHCP_OPTION_IRCSERVER, 3},
        {"StreetTalk_Server", DHCP_OPTION_STSERVER, 3},
        {"STDA_Server", DHCP_OPTION_STDASERVER, 3},
        {"NDS_Servers", DHCP_OPTION_NDSSERVERS, 3},
        {"NDS_Tree_Name", DHCP_OPTION_NDSTREENAME, 1},
        {"NDS_Context", 87, 1},
        {"LDAP_URL", DHCP_OPTION_LDAP, 1},
        {"Auto_Configure", DHCP_OPTION_AUTO_CONFIG, 7},
        {"Name_Service_Search", DHCP_OPTION_NAMESERVICESEARCH, 2},
        {"Subnet_Selection", DHCP_OPTION_SUBNETSELECTION, 3},
        //{"DNS_Domain_Search", DHCP_OPTION_DOMAINSEARCH, 1},
        {"TFTP_Phone_Server", DHCP_OPTION_TFPTSERVERIPADDRESS, 3},
        //{"TFTP_Server", DHCP_OPTION_TFPTSERVERIPADDRESS, 3},
        {"Call_Server", DHCP_OPTION_CALLSERVERIPADDRESS, 3},
        {"Discrimination_String", DHCP_OPTION_DISCRIMINATIONSTRING, 1},
        {"RemoteStatisticsServer", DHCP_OPTION_REMOTESTATISTICSSERVER, 3},
        {"Phone_Http_Proxy", DHCP_OPTION_HTTPPROXYFORPHONE_SPEC, 3},
        {"IP_Phone", 176, 1},
        {"Next_Server", DHCP_OPTION_NEXTSERVER, 3}
    };

}

#endif
#endif
