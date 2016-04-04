// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "monitor_result_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char monitor_result_base::this_class_name[] = "monitor_result_base";

monitor_result_base::monitor_result_base()
{
    clear();
}

monitor_result_base::monitor_result_base(const monitor_result_base& rhs)
{
    operator=(rhs);
}

monitor_result_base::monitor_result_base(const monitor_result_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

monitor_result_base::~monitor_result_base()
{
}

monitor_result_base& monitor_result_base::operator=(const monitor_result_base& rhs)
{
    id = rhs.id;
    range_id = rhs.range_id;
    action_type = rhs.action_type;
    status = rhs.status;
    last_status_change = rhs.last_status_change;
    last_ping_reply = rhs.last_ping_reply;
    curr_serial_request_count = rhs.curr_serial_request_count;
    curr_serial_reply_count = rhs.curr_serial_reply_count;
    last_serial_request_count = rhs.last_serial_request_count;
    last_serial_reply_count = rhs.last_serial_reply_count;
    total_ping_request_count = rhs.total_ping_request_count;
    total_ping_reply_count = rhs.total_ping_reply_count;
    serial = rhs.serial;
    serial_attempts = rhs.serial_attempts;
    curr_tick_request = rhs.curr_tick_request;
    curr_tick_reply = rhs.curr_tick_reply;
    addr = rhs.addr;
    mac = rhs.mac;
    flags = rhs.flags;
    hostname = rhs.hostname;
    descr = rhs.descr;
    return *this;
}

bool monitor_result_base::operator==(const monitor_result_base& rhs) const
{
    return equals(&rhs);
}

bool monitor_result_base::equals(const ubase* rhs) const
{
    if (rhs == NULL)
    {
       return false;
    }

    const monitor_result_base* other = dynamic_cast<const monitor_result_base*>(rhs);

    if (!(id == other->id)) return false;
    if (!(range_id == other->range_id)) return false;
    if (!(action_type == other->action_type)) return false;
    if (!(status == other->status)) return false;
    if (!(last_status_change == other->last_status_change)) return false;
    if (!(last_ping_reply == other->last_ping_reply)) return false;
    if (!(curr_serial_request_count == other->curr_serial_request_count)) return false;
    if (!(curr_serial_reply_count == other->curr_serial_reply_count)) return false;
    if (!(last_serial_request_count == other->last_serial_request_count)) return false;
    if (!(last_serial_reply_count == other->last_serial_reply_count)) return false;
    if (!(total_ping_request_count == other->total_ping_request_count)) return false;
    if (!(total_ping_reply_count == other->total_ping_reply_count)) return false;
    if (!(serial == other->serial)) return false;
    if (!(serial_attempts == other->serial_attempts)) return false;
    if (!(curr_tick_request == other->curr_tick_request)) return false;
    if (!(curr_tick_reply == other->curr_tick_reply)) return false;
    if (!(addr == other->addr)) return false;
    if (!(mac == other->mac)) return false;
    if (!(flags == other->flags)) return false;
    if (!(hostname == other->hostname)) return false;
    if (!(descr == other->descr)) return false;

    return true;
}

void monitor_result_base::clear()
{
    id = 0;
    range_id = 0;
    action_type = 0;
    status = 0;
    last_status_change.clear();
    last_ping_reply.clear();
    curr_serial_request_count = 0;
    curr_serial_reply_count = 0;
    last_serial_request_count = 0;
    last_serial_reply_count = 0;
    total_ping_request_count = 0;
    total_ping_reply_count = 0;
    serial = 0;
    serial_attempts = 0;
    curr_tick_request = 0;
    curr_tick_reply = 0;
    addr.clear();
    mac.clear();
    flags = 0;
    hostname.clear();
    descr.clear();
}

void monitor_result_base::xml_create()
{
    monitor_result_base orig;

    xml_append_root( MONITORRESULT_XMLTAG_MONITORRESULT);
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

    xml_append_node("Id", id, orig.id);
    xml_append_node("RangeId", range_id, orig.range_id);
    xml_append_node("ActionType", action_type, orig.action_type);
    xml_append_node("Status", status, orig.status);
    xml_append_node("LastStatusChange", last_status_change, orig.last_status_change);
    xml_append_node("LastPingReply", last_ping_reply, orig.last_ping_reply);
    xml_append_node("CurrentSerialRequestCount", curr_serial_request_count, orig.curr_serial_request_count);
    xml_append_node("CurrentSerialReplyCount", curr_serial_reply_count, orig.curr_serial_reply_count);
    xml_append_node("LastSerialRequestCount", last_serial_request_count, orig.last_serial_request_count);
    xml_append_node("LastSerialReplyCount", last_serial_reply_count, orig.last_serial_reply_count);
    xml_append_node("TotalPingRequest", total_ping_request_count, orig.total_ping_request_count);
    xml_append_node("TotalPingReply", total_ping_reply_count, orig.total_ping_reply_count);
    xml_append_node("Serial", serial, orig.serial);
    xml_append_node("SerialAttempts", serial_attempts, orig.serial_attempts);
    xml_append_node("CurrentTickRequest", curr_tick_request, orig.curr_tick_request);
    xml_append_node("CurrentTickReply", curr_tick_reply, orig.curr_tick_reply);
    xml_append_node("Addr", addr, orig.addr);
    xml_append_node("Mac", mac, orig.mac);
    xml_append_node("Flags", flags, orig.flags);
    xml_append_node("Hostname", hostname, orig.hostname);
    xml_append_node("Descr", descr, orig.descr);
}

void monitor_result_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "Id", keyvalue, id)) return;
    if (xml_check_value(keyname, "RangeId", keyvalue, range_id)) return;
    if (xml_check_value(keyname, "ActionType", keyvalue, action_type)) return;
    if (xml_check_value(keyname, "Status", keyvalue, status)) return;
    if (xml_check_value(keyname, "LastStatusChange", keyvalue, last_status_change)) return;
    if (xml_check_value(keyname, "LastPingReply", keyvalue, last_ping_reply)) return;
    if (xml_check_value(keyname, "CurrentSerialRequestCount", keyvalue, curr_serial_request_count)) return;
    if (xml_check_value(keyname, "CurrentSerialReplyCount", keyvalue, curr_serial_reply_count)) return;
    if (xml_check_value(keyname, "LastSerialRequestCount", keyvalue, last_serial_request_count)) return;
    if (xml_check_value(keyname, "LastSerialReplyCount", keyvalue, last_serial_reply_count)) return;
    if (xml_check_value(keyname, "TotalPingRequest", keyvalue, total_ping_request_count)) return;
    if (xml_check_value(keyname, "TotalPingReply", keyvalue, total_ping_reply_count)) return;
    if (xml_check_value(keyname, "Serial", keyvalue, serial)) return;
    if (xml_check_value(keyname, "SerialAttempts", keyvalue, serial_attempts)) return;
    if (xml_check_value(keyname, "CurrentTickRequest", keyvalue, curr_tick_request)) return;
    if (xml_check_value(keyname, "CurrentTickReply", keyvalue, curr_tick_reply)) return;
    if (xml_check_value(keyname, "Addr", keyvalue, addr)) return;
    if (xml_check_value(keyname, "Mac", keyvalue, mac)) return;
    if (xml_check_value(keyname, "Flags", keyvalue, flags)) return;
    if (xml_check_value(keyname, "Hostname", keyvalue, hostname)) return;
    if (xml_check_value(keyname, "Descr", keyvalue, descr)) return;
}



#ifdef UTM_WIN

LONG monitor_result_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG monitor_result_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
