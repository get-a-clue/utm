// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "svcstate_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char svcstate_base::this_class_name[] = "svcstate_base";

svcstate_base::svcstate_base()
{
    clear();
}

svcstate_base::svcstate_base(const svcstate_base& rhs)
{
    operator=(rhs);
}

svcstate_base::svcstate_base(const svcstate_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

svcstate_base::~svcstate_base()
{
}

svcstate_base& svcstate_base::operator=(const svcstate_base& rhs)
{
    max_demo_filters = rhs.max_demo_filters;
    max_filters = rhs.max_filters;
    ver_major = rhs.ver_major;
    ver_minor = rhs.ver_minor;
    ver_build = rhs.ver_build;
    adapters_selected = rhs.adapters_selected;
    startup_time = rhs.startup_time;
    uptime_seconds = rhs.uptime_seconds.load();
    current_time = rhs.current_time;
    memcpy(&current_tm, &rhs.current_tm, sizeof(current_tm));
    total_bytes_captured = rhs.total_bytes_captured.load();
    total_bytes_captured_prev = rhs.total_bytes_captured_prev.load();
    total_bytes_speed = rhs.total_bytes_speed.load();
    total_packets_captured = rhs.total_packets_captured.load();
    total_packets_captured_prev = rhs.total_packets_captured_prev.load();
    total_packets_speed = rhs.total_packets_speed.load();
    total_netflow_packets_captured = rhs.total_netflow_packets_captured.load();
    CaptureStatus = rhs.CaptureStatus;
    license_type = rhs.license_type;
    reg_name = rhs.reg_name;
    hardware_id = rhs.hardware_id;
    expiration_year = rhs.expiration_year;
    expiration_month = rhs.expiration_month;
    expiration_day = rhs.expiration_day;
    trial_days_total = rhs.trial_days_total;
    trial_days_left = rhs.trial_days_left;
    times_after_flush_db = rhs.times_after_flush_db;
    times_after_flush_xml = rhs.times_after_flush_xml;
    times_after_flush_pktlog = rhs.times_after_flush_pktlog;
    revision = rhs.revision;
    fsname = rhs.fsname;
    return *this;
}

bool svcstate_base::operator==(const svcstate_base& rhs) const
{
    if (!(max_demo_filters == rhs.max_demo_filters)) return false;
    if (!(max_filters == rhs.max_filters)) return false;
    if (!(ver_major == rhs.ver_major)) return false;
    if (!(ver_minor == rhs.ver_minor)) return false;
    if (!(ver_build == rhs.ver_build)) return false;
    if (!(adapters_selected == rhs.adapters_selected)) return false;
    if (!(startup_time == rhs.startup_time)) return false;
    if (!(uptime_seconds == rhs.uptime_seconds)) return false;
    if (!(current_time == rhs.current_time)) return false;
    if ((memcmp(&current_tm, &rhs.current_tm, sizeof(current_tm))!=0)) return false;
    if (!(total_bytes_captured == rhs.total_bytes_captured)) return false;
    if (!(total_bytes_captured_prev == rhs.total_bytes_captured_prev)) return false;
    if (!(total_bytes_speed == rhs.total_bytes_speed)) return false;
    if (!(total_packets_captured == rhs.total_packets_captured)) return false;
    if (!(total_packets_captured_prev == rhs.total_packets_captured_prev)) return false;
    if (!(total_packets_speed == rhs.total_packets_speed)) return false;
    if (!(total_netflow_packets_captured == rhs.total_netflow_packets_captured)) return false;
    if (!(CaptureStatus == rhs.CaptureStatus)) return false;
    if (!(license_type == rhs.license_type)) return false;
    if (!(reg_name == rhs.reg_name)) return false;
    if (!(hardware_id == rhs.hardware_id)) return false;
    if (!(expiration_year == rhs.expiration_year)) return false;
    if (!(expiration_month == rhs.expiration_month)) return false;
    if (!(expiration_day == rhs.expiration_day)) return false;
    if (!(trial_days_total == rhs.trial_days_total)) return false;
    if (!(trial_days_left == rhs.trial_days_left)) return false;
    if (!(times_after_flush_db == rhs.times_after_flush_db)) return false;
    if (!(times_after_flush_xml == rhs.times_after_flush_xml)) return false;
    if (!(times_after_flush_pktlog == rhs.times_after_flush_pktlog)) return false;
    if (!(revision == rhs.revision)) return false;
    if (!(fsname == rhs.fsname)) return false;

    return true;
}

void svcstate_base::clear()
{
    max_demo_filters = 0;
    max_filters = 0;
    ver_major = 0;
    ver_minor = 0;
    ver_build = 0;
    adapters_selected = 0;
    startup_time = 0;
    uptime_seconds = 0;
    current_time = 0;
    memset(&current_tm, 0, sizeof(current_tm));
    total_bytes_captured = 0;
    total_bytes_captured_prev = 0;
    total_bytes_speed = 0;
    total_packets_captured = 0;
    total_packets_captured_prev = 0;
    total_packets_speed = 0;
    total_netflow_packets_captured = 0;
    CaptureStatus.set_stopped();
    license_type.clear();
    reg_name.clear();
    hardware_id.clear();
    expiration_year = 0;
    expiration_month = 0;
    expiration_day = 0;
    trial_days_total = 0;
    trial_days_left = 0;
    times_after_flush_db = 0;
    times_after_flush_xml = 0;
    times_after_flush_pktlog = 0;
    revision = 0;
    fsname.clear();
}

void svcstate_base::xml_create()
{
    svcstate_base orig;

    xml_append_root( SVCSTATE_XMLTAG_ROOT);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("MaxDemoFilters", max_demo_filters, orig.max_demo_filters);
    xml_append_node("MaxFilters", max_filters, orig.max_filters);
    xml_append_node("VerMajor", ver_major, orig.ver_major);
    xml_append_node("VerMinor", ver_minor, orig.ver_minor);
    xml_append_node("VerBuild", ver_build, orig.ver_build);
    xml_append_node("Adsel", adapters_selected, orig.adapters_selected);
    xml_append_node("StartupTime", startup_time, orig.startup_time);
    xml_append_node("Uptime", uptime_seconds.load(), orig.uptime_seconds.load());
    xml_append_node("CurrentTime", current_time, orig.current_time);
    xml_append_node("CurrentTm", get_currenttm_str());
    xml_append_node("TotalBytesCaptured", total_bytes_captured.load(), orig.total_bytes_captured.load());
    xml_append_node("TotalBytesCapturedPrev", total_bytes_captured_prev.load(), orig.total_bytes_captured_prev.load());
    xml_append_node("TotalBytesSpeed", total_bytes_speed.load(), orig.total_bytes_speed.load());
    xml_append_node("TotalPacketsCaptured", total_packets_captured.load(), orig.total_packets_captured.load());
    xml_append_node("TotalPacketsCapturedPrev", total_packets_captured_prev.load(), orig.total_packets_captured_prev.load());
    xml_append_node("TotalPacketsSpeed", total_packets_speed.load(), orig.total_packets_speed.load());
    xml_append_node("TotalNetflowPacketsCaptured", total_netflow_packets_captured.load(), orig.total_netflow_packets_captured.load());
    xml_append_node("CaptureStatus", CaptureStatus, orig.CaptureStatus);
    xml_append_node("LicenseType", license_type, orig.license_type);
    xml_append_node("RegName", reg_name, orig.reg_name);
    xml_append_node("HardwareId", hardware_id, orig.hardware_id);
    xml_append_node("ExpYear", expiration_year, orig.expiration_year);
    xml_append_node("ExpMonth", expiration_month, orig.expiration_month);
    xml_append_node("ExpDay", expiration_day, orig.expiration_day);
    xml_append_node("TrialDaysTotal", trial_days_total, orig.trial_days_total);
    xml_append_node("TrialDaysLeft", trial_days_left, orig.trial_days_left);
    xml_append_node("DbTimesAfterFlush", times_after_flush_db, orig.times_after_flush_db);
    xml_append_node("XmlTimesAfterFlush", times_after_flush_xml, orig.times_after_flush_xml);
    xml_append_node("PktLogTimesAfterFlush", times_after_flush_pktlog, orig.times_after_flush_pktlog);
    xml_append_node("FiltersetRevision", revision, orig.revision);
    xml_append_node("FiltersetName", fsname, orig.fsname);
}

void svcstate_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "MaxDemoFilters", keyvalue, max_demo_filters)) return;
    if (xml_check_value(keyname, "MaxFilters", keyvalue, max_filters)) return;
    if (xml_check_value(keyname, "VerMajor", keyvalue, ver_major)) return;
    if (xml_check_value(keyname, "VerMinor", keyvalue, ver_minor)) return;
    if (xml_check_value(keyname, "VerBuild", keyvalue, ver_build)) return;
    if (xml_check_value(keyname, "Adsel", keyvalue, adapters_selected)) return;
    if (xml_check_value(keyname, "StartupTime", keyvalue, startup_time)) return;
    if (xml_check_value(keyname, "Uptime", keyvalue, uptime_seconds)) return;
    if (xml_check_value(keyname, "CurrentTime", keyvalue, current_time)) return;
    if (xml_check_value(keyname, "CurrentTm", keyvalue, boost::bind(&svcstate_base::parse_currenttm_string,this,_1))) return;
    if (xml_check_value(keyname, "TotalBytesCaptured", keyvalue, total_bytes_captured)) return;
    if (xml_check_value(keyname, "TotalBytesCapturedPrev", keyvalue, total_bytes_captured_prev)) return;
    if (xml_check_value(keyname, "TotalBytesSpeed", keyvalue, total_bytes_speed)) return;
    if (xml_check_value(keyname, "TotalPacketsCaptured", keyvalue, total_packets_captured)) return;
    if (xml_check_value(keyname, "TotalPacketsCapturedPrev", keyvalue, total_packets_captured_prev)) return;
    if (xml_check_value(keyname, "TotalPacketsSpeed", keyvalue, total_packets_speed)) return;
    if (xml_check_value(keyname, "TotalNetflowPacketsCaptured", keyvalue, total_netflow_packets_captured)) return;
    if (xml_check_value(keyname, "CaptureStatus", keyvalue, CaptureStatus)) return;
    if (xml_check_value(keyname, "LicenseType", keyvalue, license_type)) return;
    if (xml_check_value(keyname, "RegName", keyvalue, reg_name)) return;
    if (xml_check_value(keyname, "HardwareId", keyvalue, hardware_id)) return;
    if (xml_check_value(keyname, "ExpYear", keyvalue, expiration_year)) return;
    if (xml_check_value(keyname, "ExpMonth", keyvalue, expiration_month)) return;
    if (xml_check_value(keyname, "ExpDay", keyvalue, expiration_day)) return;
    if (xml_check_value(keyname, "TrialDaysTotal", keyvalue, trial_days_total)) return;
    if (xml_check_value(keyname, "TrialDaysLeft", keyvalue, trial_days_left)) return;
    if (xml_check_value(keyname, "DbTimesAfterFlush", keyvalue, times_after_flush_db)) return;
    if (xml_check_value(keyname, "XmlTimesAfterFlush", keyvalue, times_after_flush_xml)) return;
    if (xml_check_value(keyname, "PktLogTimesAfterFlush", keyvalue, times_after_flush_pktlog)) return;
    if (xml_check_value(keyname, "FiltersetRevision", keyvalue, revision)) return;
    if (xml_check_value(keyname, "FiltersetName", keyvalue, fsname)) return;
}



#ifdef UTM_WIN

LONG svcstate_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG svcstate_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
