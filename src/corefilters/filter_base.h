// This is auto-generated file. Do not modify it!

#ifndef UTM_FILTER_BASE_H
#define UTM_FILTER_BASE_H

#pragma once
#include <utm.h>

#include <ubase.h>
#include <ubaselist.h>
#include <addrip_v4.h>
#include <addrmac.h>
#include <gstring.h>
#include <rule.h>
#include <rulelist.h>

#define  FILTER_XMLTAG_FILTER "Filter"
#define  LOGPKT_MAXCOLSIZE 1200
#define  LOGPKT_SQLADDR_CHAR 0
#define  LOGPKT_SQLADDR_UINT 1
#define  LOGPKT_SQLADDR_INT 2
#define  LOGPKT_DISABLED 0
#define  LOGPKT_INTOFILE 1
#define  LOGPKT_INTODB 2
#define  LOGHH_DISABLED 0
#define  LOGHH_INTOFILE 1
#define  LOGHH_INTODB 2
#define  LOGHH_SQUID 3
#define  RESETCNT_NEVER 0
#define  RESETCNT_DAILY 1
#define  RESETCNT_MONTHLY 2
#define  RESETCNT_MONDAY 3
#define  RESETCNT_TUESDAY 4
#define  RESETCNT_WEDNESDAY 5
#define  RESETCNT_THURSDAY 6
#define  RESETCNT_FRIDAY 7
#define  RESETCNT_SATURDAY 8
#define  RESETCNT_SUNDAY 9
#define  RESETCNT_HOURLY 10
#define  TRAFFICLIMIT_SENT_OR_RECV 0
#define  TRAFFICLIMIT_SENT_PLUS_RECV 1
#define  TRAFFICLIMIT_SENT 2
#define  TRAFFICLIMIT_RECV 3
#define  TRAFFICLIMIT_REVERS 100
#define  CHECKADDR_MYPC_SRC 1
#define  CHECKADDR_MYPC_DST 2
#define  CHECKADDR_LAT_SRC 4
#define  CHECKADDR_LAT_DST 8
#define  CHECKADDR_MYNET_SRC 16
#define  CHECKADDR_MYNET_DST 32
#define  TRAFFICLIMIT_BLOCK 0
#define  TRAFFICLIMIT_SPEED2 1

namespace utm {

typedef  std::list<std::unique_ptr<utm::ubase>> rule_container;

class filter_base : public ubase
{
public:
    static const char this_class_name[];

public:
    filter_base();
    filter_base(const filter_base& rhs);
    filter_base(const filter_base& rhs, bool make_threadsafe_copyobj);
    virtual ~filter_base();

    filter_base& operator=(const filter_base& rhs);
    virtual bool equals(const ubase* rhs) const;
    bool operator==(const filter_base& rhs) const;

    virtual const char *get_this_class_name() const { return "filter_base"; };

virtual  rule_container& get_ruleset() { return rules.items; };
virtual  std::string get_workhours_str() const { return std::string(""); };
virtual  void parse_rule_string(const char *rule_string) { };
virtual  void parse_workhours_string(const char *workhours_string) { };

public:
    int m_id;
    gstring m_szFilterName;
    unsigned int m_color;
    bool m_bRevers;
    bool m_bBlocked;
    bool m_bDisable;
    bool m_bVpn;
    bool m_bExcludeCntReports;
    bool m_bExcludeUrlFiltering;
    int m_nResetCnt;
    unsigned int m_nWorkHours[7];
    unsigned int m_nStartDay;
    unsigned int m_nEndDay;
    gstring m_szEmail;
    gstring m_szComment;
    int m_nRewMask;
    addrip_v4 m_dwRewSrcIp;
    addrip_v4 m_dwRewDstIp;
    unsigned short m_wRewSrcPort;
    unsigned short m_wRewDstPort;
    rulelist rules;
    __int64 m_nTrafficLimitMb;
    int m_nTrafficLimitType;
    int m_nTrafficLimitAction;
    unsigned short m_wTrafficLimitWarnLevel;
    unsigned short m_wTrafficLimitFlags;
    int m_nPktLogDest;
    int m_nPktLogOpt;
    gstring m_szPktLogFilename;
    gstring m_szPktLogTable;
    int m_nPktLogSqlSyntax;
    int m_nPktLogMaxColSize;
    int m_nPktLogAlertColUsage;
    int m_nHhLogDest;
    gstring m_szHhLogFilename;
    gstring m_szHhLogTable;
    int m_nMaster;
    int m_nSpeed;
    int m_nSpeed2;
    int m_nSpeedMaster;
    int m_nMasterFilterId;
    int m_nMasterLevelSpeed;
    int m_nMasterOptions;


public:
    void clear();
    void xml_create();
    void xml_catch_value(const char *keyname, const char *keyvalue);
    virtual ubase* xml_catch_subnode(const char *tag_name, const char *class_name) { return NULL; };



#ifdef UTM_WIN

    virtual LONG SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
    virtual LONG ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
#endif // UTM_WIN
};

}

#endif // UTM_FILTER_BASE_H
