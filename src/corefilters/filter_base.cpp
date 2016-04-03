// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "filter_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char filter_base::this_class_name[] = "filter_base";

filter_base::filter_base()
{
    clear();
}

filter_base::filter_base(const filter_base& rhs)
{
    operator=(rhs);
}

filter_base::filter_base(const filter_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

filter_base::~filter_base()
{
}

filter_base& filter_base::operator=(const filter_base& rhs)
{
    m_id = rhs.m_id;
    m_szFilterName = rhs.m_szFilterName;
    m_color = rhs.m_color;
    m_bRevers = rhs.m_bRevers;
    m_bBlocked = rhs.m_bBlocked;
    m_bDisable = rhs.m_bDisable;
    m_bVpn = rhs.m_bVpn;
    m_bExcludeCntReports = rhs.m_bExcludeCntReports;
    m_bExcludeUrlFiltering = rhs.m_bExcludeUrlFiltering;
    m_nResetCnt = rhs.m_nResetCnt;
    memcpy(&m_nWorkHours, &rhs.m_nWorkHours, sizeof(m_nWorkHours));
    m_nStartDay = rhs.m_nStartDay;
    m_nEndDay = rhs.m_nEndDay;
    m_szEmail = rhs.m_szEmail;
    m_szComment = rhs.m_szComment;
    m_nRewMask = rhs.m_nRewMask;
    m_dwRewSrcIp = rhs.m_dwRewSrcIp;
    m_dwRewDstIp = rhs.m_dwRewDstIp;
    m_wRewSrcPort = rhs.m_wRewSrcPort;
    m_wRewDstPort = rhs.m_wRewDstPort;
    rules = rhs.rules;
    m_nTrafficLimitMb = rhs.m_nTrafficLimitMb;
    m_nTrafficLimitType = rhs.m_nTrafficLimitType;
    m_nTrafficLimitAction = rhs.m_nTrafficLimitAction;
    m_wTrafficLimitWarnLevel = rhs.m_wTrafficLimitWarnLevel;
    m_wTrafficLimitFlags = rhs.m_wTrafficLimitFlags;
    m_nPktLogDest = rhs.m_nPktLogDest;
    m_nPktLogOpt = rhs.m_nPktLogOpt;
    m_szPktLogFilename = rhs.m_szPktLogFilename;
    m_szPktLogTable = rhs.m_szPktLogTable;
    m_nPktLogSqlSyntax = rhs.m_nPktLogSqlSyntax;
    m_nPktLogMaxColSize = rhs.m_nPktLogMaxColSize;
    m_nPktLogAlertColUsage = rhs.m_nPktLogAlertColUsage;
    m_nHhLogDest = rhs.m_nHhLogDest;
    m_szHhLogFilename = rhs.m_szHhLogFilename;
    m_szHhLogTable = rhs.m_szHhLogTable;
    m_nMaster = rhs.m_nMaster;
    m_nSpeed = rhs.m_nSpeed;
    m_nSpeed2 = rhs.m_nSpeed2;
    m_nSpeedMaster = rhs.m_nSpeedMaster;
    m_nMasterFilterId = rhs.m_nMasterFilterId;
    m_nMasterLevelSpeed = rhs.m_nMasterLevelSpeed;
    m_nMasterOptions = rhs.m_nMasterOptions;
    return *this;
}

bool filter_base::operator==(const filter_base& rhs) const
{
    return equals(&rhs);
}

bool filter_base::equals(const ubase* rhs) const
{
    if (rhs == NULL)
    {
       return false;
    }

    const filter_base* other = dynamic_cast<const filter_base*>(rhs);

    if (!(m_id == other->m_id)) return false;
    if (!(m_szFilterName == other->m_szFilterName)) return false;
    if (!(m_color == other->m_color)) return false;
    if (!(m_bRevers == other->m_bRevers)) return false;
    if (!(m_bBlocked == other->m_bBlocked)) return false;
    if (!(m_bDisable == other->m_bDisable)) return false;
    if (!(m_bVpn == other->m_bVpn)) return false;
    if (!(m_bExcludeCntReports == other->m_bExcludeCntReports)) return false;
    if (!(m_bExcludeUrlFiltering == other->m_bExcludeUrlFiltering)) return false;
    if (!(m_nResetCnt == other->m_nResetCnt)) return false;
    if ((memcmp(&m_nWorkHours, &other->m_nWorkHours, sizeof(m_nWorkHours))!=0)) return false;
    if (!(m_nStartDay == other->m_nStartDay)) return false;
    if (!(m_nEndDay == other->m_nEndDay)) return false;
    if (!(m_szEmail == other->m_szEmail)) return false;
    if (!(m_szComment == other->m_szComment)) return false;
    if (!(m_nRewMask == other->m_nRewMask)) return false;
    if (!(m_dwRewSrcIp == other->m_dwRewSrcIp)) return false;
    if (!(m_dwRewDstIp == other->m_dwRewDstIp)) return false;
    if (!(m_wRewSrcPort == other->m_wRewSrcPort)) return false;
    if (!(m_wRewDstPort == other->m_wRewDstPort)) return false;
    if (!(rules == other->rules)) return false;
    if (!(m_nTrafficLimitMb == other->m_nTrafficLimitMb)) return false;
    if (!(m_nTrafficLimitType == other->m_nTrafficLimitType)) return false;
    if (!(m_nTrafficLimitAction == other->m_nTrafficLimitAction)) return false;
    if (!(m_wTrafficLimitWarnLevel == other->m_wTrafficLimitWarnLevel)) return false;
    if (!(m_wTrafficLimitFlags == other->m_wTrafficLimitFlags)) return false;
    if (!(m_nPktLogDest == other->m_nPktLogDest)) return false;
    if (!(m_nPktLogOpt == other->m_nPktLogOpt)) return false;
    if (!(m_szPktLogFilename == other->m_szPktLogFilename)) return false;
    if (!(m_szPktLogTable == other->m_szPktLogTable)) return false;
    if (!(m_nPktLogSqlSyntax == other->m_nPktLogSqlSyntax)) return false;
    if (!(m_nPktLogMaxColSize == other->m_nPktLogMaxColSize)) return false;
    if (!(m_nPktLogAlertColUsage == other->m_nPktLogAlertColUsage)) return false;
    if (!(m_nHhLogDest == other->m_nHhLogDest)) return false;
    if (!(m_szHhLogFilename == other->m_szHhLogFilename)) return false;
    if (!(m_szHhLogTable == other->m_szHhLogTable)) return false;
    if (!(m_nMaster == other->m_nMaster)) return false;
    if (!(m_nSpeed == other->m_nSpeed)) return false;
    if (!(m_nSpeed2 == other->m_nSpeed2)) return false;
    if (!(m_nSpeedMaster == other->m_nSpeedMaster)) return false;
    if (!(m_nMasterFilterId == other->m_nMasterFilterId)) return false;
    if (!(m_nMasterLevelSpeed == other->m_nMasterLevelSpeed)) return false;
    if (!(m_nMasterOptions == other->m_nMasterOptions)) return false;

    return true;
}

void filter_base::clear()
{
    m_id = 0;
    m_szFilterName.clear();
    m_color = 255;
    m_bRevers = false;
    m_bBlocked = false;
    m_bDisable = false;
    m_bVpn = false;
    m_bExcludeCntReports = false;
    m_bExcludeUrlFiltering = false;
    m_nResetCnt = RESETCNT_MONTHLY;
    memset(&m_nWorkHours, 0xFF, sizeof(m_nWorkHours));
    m_nStartDay = 20000101;
    m_nEndDay = 20991231;
    m_szEmail.clear();
    m_szComment.clear();
    m_nRewMask = 0;
    m_dwRewSrcIp.clear();
    m_dwRewDstIp.clear();
    m_wRewSrcPort = 0;
    m_wRewDstPort = 0;
    rules.clear();
    m_nTrafficLimitMb = 0;
    m_nTrafficLimitType = 0;
    m_nTrafficLimitAction = 0;
    m_wTrafficLimitWarnLevel = 80;
    m_wTrafficLimitFlags = 0;
    m_nPktLogDest = LOGPKT_DISABLED;
    m_nPktLogOpt = 0;
    m_szPktLogFilename.clear();
    m_szPktLogTable.clear();
    m_nPktLogSqlSyntax = LOGPKT_SQLADDR_CHAR;
    m_nPktLogMaxColSize = LOGPKT_MAXCOLSIZE;
    m_nPktLogAlertColUsage = 100;
    m_nHhLogDest = LOGHH_DISABLED;
    m_szHhLogFilename.clear();
    m_szHhLogTable.clear();
    m_nMaster = 0;
    m_nSpeed = 0;
    m_nSpeed2 = 0;
    m_nSpeedMaster = 0;
    m_nMasterFilterId = 0;
    m_nMasterLevelSpeed = 0;
    m_nMasterOptions = 0;
}

void filter_base::xml_create()
{
    filter_base orig;

    xml_append_root( FILTER_XMLTAG_FILTER);
    add_class_name();
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("Id", m_id, orig.m_id);
    xml_append_node("Name", m_szFilterName, orig.m_szFilterName);
    xml_append_node("Color", m_color, orig.m_color);
    xml_append_node("Revers", m_bRevers, orig.m_bRevers);
    xml_append_node("Blocked", m_bBlocked, orig.m_bBlocked);
    xml_append_node("Disable", m_bDisable, orig.m_bDisable);
    xml_append_node("ParseVpn", m_bVpn, orig.m_bVpn);
    xml_append_node("ExcludeCnt", m_bExcludeCntReports, orig.m_bExcludeCntReports);
    xml_append_node("ExcludeUrlFiltering", m_bExcludeUrlFiltering, orig.m_bExcludeUrlFiltering);
    xml_append_node("ResetCnt", m_nResetCnt, orig.m_nResetCnt);
    xml_append_node("WorkHours", get_workhours_str());
    xml_append_node("StartDay", m_nStartDay, orig.m_nStartDay);
    xml_append_node("EndDay", m_nEndDay, orig.m_nEndDay);
    xml_append_node("Email", m_szEmail, orig.m_szEmail);
    xml_append_node("Comment", m_szComment, orig.m_szComment);
    xml_append_node("RewriteMask", m_nRewMask, orig.m_nRewMask);
    xml_append_node("RewriteSrcIp", m_dwRewSrcIp, orig.m_dwRewSrcIp);
    xml_append_node("RewriteDstIp", m_dwRewDstIp, orig.m_dwRewDstIp);
    xml_append_node("RewriteSrcPort", m_wRewSrcPort, orig.m_wRewSrcPort);
    xml_append_node("RewriteDstPort", m_wRewDstPort, orig.m_wRewDstPort);
    xml_append_node("RuleNT", get_ruleset());
    xml_append_node("TrafficLimitMb", m_nTrafficLimitMb, orig.m_nTrafficLimitMb);
    xml_append_node("TrafficLimitType", m_nTrafficLimitType, orig.m_nTrafficLimitType);
    xml_append_node("TrafficLimitAction", m_nTrafficLimitAction, orig.m_nTrafficLimitAction);
    xml_append_node("TrafficLimitWarnLevel", m_wTrafficLimitWarnLevel, orig.m_wTrafficLimitWarnLevel);
    xml_append_node("TrafficLimitFlags", m_wTrafficLimitFlags, orig.m_wTrafficLimitFlags);
    xml_append_node("PackLogDest", m_nPktLogDest, orig.m_nPktLogDest);
    xml_append_node("PackLogOpt", m_nPktLogOpt, orig.m_nPktLogOpt);
    xml_append_node("PackLogFilenameTemplate", m_szPktLogFilename, orig.m_szPktLogFilename);
    xml_append_node("PackLogDbTable", m_szPktLogTable, orig.m_szPktLogTable);
    xml_append_node("PackLogSQLSyntax", m_nPktLogSqlSyntax, orig.m_nPktLogSqlSyntax);
    xml_append_node("PackLogMaxColSize", m_nPktLogMaxColSize, orig.m_nPktLogMaxColSize);
    xml_append_node("PackLogAlertColUsage", m_nPktLogAlertColUsage, orig.m_nPktLogAlertColUsage);
    xml_append_node("HhLogDest", m_nHhLogDest, orig.m_nHhLogDest);
    xml_append_node("HhLogFilenameTemplate", m_szHhLogFilename, orig.m_szHhLogFilename);
    xml_append_node("HhLogDbTable", m_szHhLogTable, orig.m_szHhLogTable);
    xml_append_node("Master", m_nMaster, orig.m_nMaster);
    xml_append_node("SpeedKiloBytes", m_nSpeed, orig.m_nSpeed);
    xml_append_node("SpeedKiloBytes2", m_nSpeed2, orig.m_nSpeed2);
    xml_append_node("SpeedMaster", m_nSpeedMaster, orig.m_nSpeedMaster);
    xml_append_node("MasterFid", m_nMasterFilterId, orig.m_nMasterFilterId);
    xml_append_node("MasterLevel", m_nMasterLevelSpeed, orig.m_nMasterLevelSpeed);
    xml_append_node("MasterOpt", m_nMasterOptions, orig.m_nMasterOptions);
}

void filter_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "Id", keyvalue, m_id)) return;
    if (xml_check_value(keyname, "Name", keyvalue, m_szFilterName)) return;
    if (xml_check_value(keyname, "Color", keyvalue, m_color)) return;
    if (xml_check_value(keyname, "Revers", keyvalue, m_bRevers)) return;
    if (xml_check_value(keyname, "Blocked", keyvalue, m_bBlocked)) return;
    if (xml_check_value(keyname, "Disable", keyvalue, m_bDisable)) return;
    if (xml_check_value(keyname, "ParseVpn", keyvalue, m_bVpn)) return;
    if (xml_check_value(keyname, "ExcludeCnt", keyvalue, m_bExcludeCntReports)) return;
    if (xml_check_value(keyname, "ExcludeUrlFiltering", keyvalue, m_bExcludeUrlFiltering)) return;
    if (xml_check_value(keyname, "ResetCnt", keyvalue, m_nResetCnt)) return;
    if (xml_check_value(keyname, "WorkHours", keyvalue, boost::bind(&filter_base::parse_workhours_string,this,_1))) return;
    if (xml_check_value(keyname, "StartDay", keyvalue, m_nStartDay)) return;
    if (xml_check_value(keyname, "EndDay", keyvalue, m_nEndDay)) return;
    if (xml_check_value(keyname, "Email", keyvalue, m_szEmail)) return;
    if (xml_check_value(keyname, "Comment", keyvalue, m_szComment)) return;
    if (xml_check_value(keyname, "RewriteMask", keyvalue, m_nRewMask)) return;
    if (xml_check_value(keyname, "RewriteSrcIp", keyvalue, m_dwRewSrcIp)) return;
    if (xml_check_value(keyname, "RewriteDstIp", keyvalue, m_dwRewDstIp)) return;
    if (xml_check_value(keyname, "RewriteSrcPort", keyvalue, m_wRewSrcPort)) return;
    if (xml_check_value(keyname, "RewriteDstPort", keyvalue, m_wRewDstPort)) return;
    if (xml_check_value(keyname, "RuleNT", keyvalue, boost::bind(&filter_base::parse_rule_string,this,_1))) return;
    if (xml_check_value(keyname, "TrafficLimitMb", keyvalue, m_nTrafficLimitMb)) return;
    if (xml_check_value(keyname, "TrafficLimitType", keyvalue, m_nTrafficLimitType)) return;
    if (xml_check_value(keyname, "TrafficLimitAction", keyvalue, m_nTrafficLimitAction)) return;
    if (xml_check_value(keyname, "TrafficLimitWarnLevel", keyvalue, m_wTrafficLimitWarnLevel)) return;
    if (xml_check_value(keyname, "TrafficLimitFlags", keyvalue, m_wTrafficLimitFlags)) return;
    if (xml_check_value(keyname, "PackLogDest", keyvalue, m_nPktLogDest)) return;
    if (xml_check_value(keyname, "PackLogOpt", keyvalue, m_nPktLogOpt)) return;
    if (xml_check_value(keyname, "PackLogFilenameTemplate", keyvalue, m_szPktLogFilename)) return;
    if (xml_check_value(keyname, "PackLogDbTable", keyvalue, m_szPktLogTable)) return;
    if (xml_check_value(keyname, "PackLogSQLSyntax", keyvalue, m_nPktLogSqlSyntax)) return;
    if (xml_check_value(keyname, "PackLogMaxColSize", keyvalue, m_nPktLogMaxColSize)) return;
    if (xml_check_value(keyname, "PackLogAlertColUsage", keyvalue, m_nPktLogAlertColUsage)) return;
    if (xml_check_value(keyname, "HhLogDest", keyvalue, m_nHhLogDest)) return;
    if (xml_check_value(keyname, "HhLogFilenameTemplate", keyvalue, m_szHhLogFilename)) return;
    if (xml_check_value(keyname, "HhLogDbTable", keyvalue, m_szHhLogTable)) return;
    if (xml_check_value(keyname, "Master", keyvalue, m_nMaster)) return;
    if (xml_check_value(keyname, "SpeedKiloBytes", keyvalue, m_nSpeed)) return;
    if (xml_check_value(keyname, "SpeedKiloBytes2", keyvalue, m_nSpeed2)) return;
    if (xml_check_value(keyname, "SpeedMaster", keyvalue, m_nSpeedMaster)) return;
    if (xml_check_value(keyname, "MasterFid", keyvalue, m_nMasterFilterId)) return;
    if (xml_check_value(keyname, "MasterLevel", keyvalue, m_nMasterLevelSpeed)) return;
    if (xml_check_value(keyname, "MasterOpt", keyvalue, m_nMasterOptions)) return;
}



#ifdef UTM_WIN

LONG filter_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG filter_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_READ, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}
#endif // UTM_WIN

}
