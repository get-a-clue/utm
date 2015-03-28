// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "filtersetcons_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char filtersetcons_base::this_class_name[] = "filtersetcons_base";

filtersetcons_base::filtersetcons_base()
{
    clear();
}

filtersetcons_base::filtersetcons_base(const filtersetcons_base& rhs)
{
    operator=(rhs);
}

filtersetcons_base::filtersetcons_base(const filtersetcons_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

filtersetcons_base::~filtersetcons_base()
{
}

filtersetcons_base& filtersetcons_base::operator=(const filtersetcons_base& rhs)
{
    total_bytes_captured = rhs.total_bytes_captured;
    total_bytes_speed = rhs.total_bytes_speed;
    total_packets_captured = rhs.total_packets_captured;
    total_packets_speed = rhs.total_packets_speed;
    total_netflow_packets_captured = rhs.total_netflow_packets_captured;
    total_points = rhs.total_points;
    next_seqnum = rhs.next_seqnum;
    diff_size = rhs.diff_size;
    start_diffindex = rhs.start_diffindex;
    diff_seqnum = rhs.diff_seqnum;
    diff_time = rhs.diff_time;
    filtercons_s = rhs.filtercons_s;
    return *this;
}

bool filtersetcons_base::operator==(const filtersetcons_base& rhs) const
{
    if (!(total_bytes_captured == rhs.total_bytes_captured)) return false;
    if (!(total_bytes_speed == rhs.total_bytes_speed)) return false;
    if (!(total_packets_captured == rhs.total_packets_captured)) return false;
    if (!(total_packets_speed == rhs.total_packets_speed)) return false;
    if (!(total_netflow_packets_captured == rhs.total_netflow_packets_captured)) return false;
    if (!(total_points == rhs.total_points)) return false;
    if (!(next_seqnum == rhs.next_seqnum)) return false;
    if (!(diff_size == rhs.diff_size)) return false;
    if (!(start_diffindex == rhs.start_diffindex)) return false;
    if (!(diff_seqnum == rhs.diff_seqnum)) return false;
    if (!(diff_time == rhs.diff_time)) return false;
    if (!(filtercons_s == rhs.filtercons_s)) return false;

    return true;
}

void filtersetcons_base::clear()
{
    total_bytes_captured = 0;
    total_bytes_speed = 0;
    total_packets_captured = 0;
    total_packets_speed = 0;
    total_netflow_packets_captured = 0;
    total_points = 0;
    next_seqnum = 0;
    diff_size = 0;
    start_diffindex = 0;
    diff_seqnum.clear();
    diff_time.clear();
    filtercons_s.clear();
}

void filtersetcons_base::xml_create()
{
    filtersetcons_base orig;

    xml_append_root( FILTERSETCONS_XMLTAG_TRAFFICCONS);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("TB", total_bytes_captured, orig.total_bytes_captured);
    xml_append_node("TBS", total_bytes_speed, orig.total_bytes_speed);
    xml_append_node("TP", total_packets_captured, orig.total_packets_captured);
    xml_append_node("TPS", total_packets_speed, orig.total_packets_speed);
    xml_append_node("TN", total_netflow_packets_captured, orig.total_netflow_packets_captured);
    xml_append_node("totpnt", total_points, orig.total_points);
    xml_append_node("nseq", next_seqnum, orig.next_seqnum);
    xml_append_node("dsize", diff_size, orig.diff_size);
    xml_append_node("dindex", start_diffindex, orig.start_diffindex);
    xml_append_node("SN", diff_seqnum, orig.diff_seqnum);
    xml_append_node("TM", diff_time, orig.diff_time);
    xml_append_node("F", get_filtercons());
}

void filtersetcons_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "TB", keyvalue, total_bytes_captured)) return;
    if (xml_check_value(keyname, "TBS", keyvalue, total_bytes_speed)) return;
    if (xml_check_value(keyname, "TP", keyvalue, total_packets_captured)) return;
    if (xml_check_value(keyname, "TPS", keyvalue, total_packets_speed)) return;
    if (xml_check_value(keyname, "TN", keyvalue, total_netflow_packets_captured)) return;
    if (xml_check_value(keyname, "totpnt", keyvalue, total_points)) return;
    if (xml_check_value(keyname, "nseq", keyvalue, next_seqnum)) return;
    if (xml_check_value(keyname, "dsize", keyvalue, diff_size)) return;
    if (xml_check_value(keyname, "dindex", keyvalue, start_diffindex)) return;
    if (xml_check_value(keyname, "SN", keyvalue, diff_seqnum)) return;
    if (xml_check_value(keyname, "TM", keyvalue, diff_time)) return;
    if (xml_check_value(keyname, "F", keyvalue, boost::bind(&filtersetcons_base::parse_filtercons_string,this,_1))) return;
}



#ifdef UTM_WIN

LONG filtersetcons_base::CreateRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_READ, &h);
    if (result == ERROR_FILE_NOT_FOUND)
    {
       result = RegCreateKeyEx(hk, pRegistryPath, NULL, NULL, NULL, KEY_ALL_ACCESS, NULL, &h, NULL);
    }

    if (result == ERROR_SUCCESS)
    {
       RegCloseKey(h);
    };

    return result;
}

LONG filtersetcons_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG filtersetcons_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
