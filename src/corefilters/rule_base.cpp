// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "rule_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char rule_base::this_class_name[] = "rule_base";

rule_base::rule_base()
{
    clear();
}

rule_base::rule_base(const rule_base& rhs)
{
    operator=(rhs);
}

rule_base::rule_base(const rule_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

rule_base::~rule_base()
{
}

rule_base& rule_base::operator=(const rule_base& rhs)
{
    id = rhs.id;
    mirrored = rhs.mirrored;
    descr = rhs.descr;
    action = rhs.action;
    rwr_fwd = rhs.rwr_fwd;
    proto = rhs.proto;
    nicalias = rhs.nicalias;
    pkt_options = rhs.pkt_options;
    condition_type = rhs.condition_type;
    condition_limit = rhs.condition_limit;
    cond_mac_type = rhs.cond_mac_type;
    cond_mac_data = rhs.cond_mac_data;
    prevfilter_type = rhs.prevfilter_type;
    natuse = rhs.natuse;
    wday = rhs.wday;
    time_from = rhs.time_from;
    time_to = rhs.time_to;
    src_type = rhs.src_type;
    src_ip = rhs.src_ip;
    src_mask = rhs.src_mask;
    src_addr_type = rhs.src_addr_type;
    src_port_type = rhs.src_port_type;
    src_port = rhs.src_port;
    src_port_to = rhs.src_port_to;
    src_atkey = rhs.src_atkey;
    src_uid = rhs.src_uid;
    src_procnick = rhs.src_procnick;
    src_mac = rhs.src_mac;
    src_host = rhs.src_host;
    dst_type = rhs.dst_type;
    dst_ip = rhs.dst_ip;
    dst_mask = rhs.dst_mask;
    dst_addr_type = rhs.dst_addr_type;
    dst_port_type = rhs.dst_port_type;
    dst_port = rhs.dst_port;
    dst_port_to = rhs.dst_port_to;
    dst_atkey = rhs.dst_atkey;
    dst_uid = rhs.dst_uid;
    dst_procnick = rhs.dst_procnick;
    dst_mac = rhs.dst_mac;
    dst_host = rhs.dst_host;
    return *this;
}

bool rule_base::operator==(const rule_base& rhs) const
{
    return equals(&rhs);
}

bool rule_base::equals(const ubase* rhs) const
{
    if (rhs == NULL)
    {
       return false;
    }

    const rule_base* other = dynamic_cast<const rule_base*>(rhs);

    if (!(id == other->id)) return false;
    if (!(mirrored == other->mirrored)) return false;
    if (!(descr == other->descr)) return false;
    if (!(action == other->action)) return false;
    if (!(rwr_fwd == other->rwr_fwd)) return false;
    if (!(proto == other->proto)) return false;
    if (!(nicalias == other->nicalias)) return false;
    if (!(pkt_options == other->pkt_options)) return false;
    if (!(condition_type == other->condition_type)) return false;
    if (!(condition_limit == other->condition_limit)) return false;
    if (!(cond_mac_type == other->cond_mac_type)) return false;
    if (!(cond_mac_data == other->cond_mac_data)) return false;
    if (!(prevfilter_type == other->prevfilter_type)) return false;
    if (!(natuse == other->natuse)) return false;
    if (!(wday == other->wday)) return false;
    if (!(time_from == other->time_from)) return false;
    if (!(time_to == other->time_to)) return false;
    if (!(src_type == other->src_type)) return false;
    if (!(src_ip == other->src_ip)) return false;
    if (!(src_mask == other->src_mask)) return false;
    if (!(src_addr_type == other->src_addr_type)) return false;
    if (!(src_port_type == other->src_port_type)) return false;
    if (!(src_port == other->src_port)) return false;
    if (!(src_port_to == other->src_port_to)) return false;
    if (!(src_atkey == other->src_atkey)) return false;
    if (!(src_uid == other->src_uid)) return false;
    if (!(src_procnick == other->src_procnick)) return false;
    if (!(src_mac == other->src_mac)) return false;
    if (!(src_host == other->src_host)) return false;
    if (!(dst_type == other->dst_type)) return false;
    if (!(dst_ip == other->dst_ip)) return false;
    if (!(dst_mask == other->dst_mask)) return false;
    if (!(dst_addr_type == other->dst_addr_type)) return false;
    if (!(dst_port_type == other->dst_port_type)) return false;
    if (!(dst_port == other->dst_port)) return false;
    if (!(dst_port_to == other->dst_port_to)) return false;
    if (!(dst_atkey == other->dst_atkey)) return false;
    if (!(dst_uid == other->dst_uid)) return false;
    if (!(dst_procnick == other->dst_procnick)) return false;
    if (!(dst_mac == other->dst_mac)) return false;
    if (!(dst_host == other->dst_host)) return false;

    return true;
}

void rule_base::clear()
{
    id = 0;
    mirrored = DIRECTION_TWOWAY;
    descr.clear();
    action = ACTION_COUNT;
    rwr_fwd = 0;
    proto = 0;
    nicalias = NICALIAS_DEFAULT;
    pkt_options = PKTOPT_NONE;
    condition_type = COND_ALWAYS;
    condition_limit = 0;
    cond_mac_type = COND_MAC_ANY;
    cond_mac_data.clear();
    prevfilter_type = PREVFILTER_ANY;
    natuse = NATUSE_ANY;
    wday = WDAY_MON|WDAY_TUE|WDAY_WED|WDAY_THU|WDAY_FRI|WDAY_SAT|WDAY_SUN;
    time_from = 0;
    time_to = 23;
    src_type = RULE_MYIP;
    src_ip = 0;
    src_mask = 0;
    src_addr_type = 0;
    src_port_type = PORT_ANY;
    src_port = 0;
    src_port_to = 0;
    src_atkey = 0;
    src_uid = 0;
    src_procnick = 0;
    src_mac.clear();
    src_host.clear();
    dst_type = RULE_IP;
    dst_ip = 0;
    dst_mask = 0;
    dst_addr_type = 0;
    dst_port_type = PORT_ANY;
    dst_port = 0;
    dst_port_to = 0;
    dst_atkey = 0;
    dst_uid = 0;
    dst_procnick = 0;
    dst_mac.clear();
    dst_host.clear();
}

void rule_base::xml_create()
{
    rule_base orig;

    xml_append_root( RULE_XMLTAG_RULE);
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
    xml_append_node("Mirrored", mirrored, orig.mirrored);
    xml_append_node("Description", descr, orig.descr);
    xml_append_node("Action", action, orig.action);
    xml_append_node("Rwr_fwd", rwr_fwd, orig.rwr_fwd);
    xml_append_node("Proto", proto, orig.proto);
    xml_append_node("ViaNic", nicalias, orig.nicalias);
    xml_append_node("TcpOptions", pkt_options, orig.pkt_options);
    xml_append_node("ConditionType", condition_type, orig.condition_type);
    xml_append_node("ConditionLimit", condition_limit, orig.condition_limit);
    xml_append_node("ConditionMacType", cond_mac_type, orig.cond_mac_type);
    xml_append_node("ConditionMacData", cond_mac_data, orig.cond_mac_data);
    xml_append_node("PrevFilterType", prevfilter_type, orig.prevfilter_type);
    xml_append_node("Nat", natuse, orig.natuse);
    xml_append_node("Wday", wday, orig.wday);
    xml_append_node("TimeFrom", time_from, orig.time_from);
    xml_append_node("TimeTo", time_to, orig.time_to);
    xml_append_node("SrcType", src_type, orig.src_type);
    xml_append_node("SrcIp", src_ip, orig.src_ip);
    xml_append_node("SrcMask", src_mask, orig.src_mask);
    xml_append_node("SrcAddrType", src_addr_type, orig.src_addr_type);
    xml_append_node("SrcPortType", src_port_type, orig.src_port_type);
    xml_append_node("SrcPort", src_port, orig.src_port);
    xml_append_node("SrcPortTo", src_port_to, orig.src_port_to);
    xml_append_node("SrcAtKey", src_atkey, orig.src_atkey);
    xml_append_node("SrcUid", src_uid, orig.src_uid);
    xml_append_node("SrcProcNick", src_procnick, orig.src_procnick);
    xml_append_node("SrcMac", src_mac, orig.src_mac);
    xml_append_node("SrcHost", get_src_host_str());
    xml_append_node("DstType", dst_type, orig.dst_type);
    xml_append_node("DstIp", dst_ip, orig.dst_ip);
    xml_append_node("DstMask", dst_mask, orig.dst_mask);
    xml_append_node("DstAddrType", dst_addr_type, orig.dst_addr_type);
    xml_append_node("DstPortType", dst_port_type, orig.dst_port_type);
    xml_append_node("DstPort", dst_port, orig.dst_port);
    xml_append_node("DstPortTo", dst_port_to, orig.dst_port_to);
    xml_append_node("DstAtKey", dst_atkey, orig.dst_atkey);
    xml_append_node("DstUid", dst_uid, orig.dst_uid);
    xml_append_node("DstProcNick", dst_procnick, orig.dst_procnick);
    xml_append_node("DstMac", dst_mac, orig.dst_mac);
    xml_append_node("DstHost", get_dst_host_str());
}

void rule_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "Id", keyvalue, id)) return;
    if (xml_check_value(keyname, "Mirrored", keyvalue, mirrored)) return;
    if (xml_check_value(keyname, "Description", keyvalue, descr)) return;
    if (xml_check_value(keyname, "Action", keyvalue, action)) return;
    if (xml_check_value(keyname, "Rwr_fwd", keyvalue, rwr_fwd)) return;
    if (xml_check_value(keyname, "Proto", keyvalue, proto)) return;
    if (xml_check_value(keyname, "ViaNic", keyvalue, nicalias)) return;
    if (xml_check_value(keyname, "TcpOptions", keyvalue, pkt_options)) return;
    if (xml_check_value(keyname, "ConditionType", keyvalue, condition_type)) return;
    if (xml_check_value(keyname, "ConditionLimit", keyvalue, condition_limit)) return;
    if (xml_check_value(keyname, "ConditionMacType", keyvalue, cond_mac_type)) return;
    if (xml_check_value(keyname, "ConditionMacData", keyvalue, cond_mac_data)) return;
    if (xml_check_value(keyname, "PrevFilterType", keyvalue, prevfilter_type)) return;
    if (xml_check_value(keyname, "Nat", keyvalue, natuse)) return;
    if (xml_check_value(keyname, "Wday", keyvalue, wday)) return;
    if (xml_check_value(keyname, "TimeFrom", keyvalue, time_from)) return;
    if (xml_check_value(keyname, "TimeTo", keyvalue, time_to)) return;
    if (xml_check_value(keyname, "SrcType", keyvalue, src_type)) return;
    if (xml_check_value(keyname, "SrcIp", keyvalue, src_ip)) return;
    if (xml_check_value(keyname, "SrcMask", keyvalue, src_mask)) return;
    if (xml_check_value(keyname, "SrcAddrType", keyvalue, src_addr_type)) return;
    if (xml_check_value(keyname, "SrcPortType", keyvalue, src_port_type)) return;
    if (xml_check_value(keyname, "SrcPort", keyvalue, src_port)) return;
    if (xml_check_value(keyname, "SrcPortTo", keyvalue, src_port_to)) return;
    if (xml_check_value(keyname, "SrcAtKey", keyvalue, src_atkey)) return;
    if (xml_check_value(keyname, "SrcUid", keyvalue, src_uid)) return;
    if (xml_check_value(keyname, "SrcProcNick", keyvalue, src_procnick)) return;
    if (xml_check_value(keyname, "SrcMac", keyvalue, src_mac)) return;
    if (xml_check_value(keyname, "SrcHost", keyvalue, boost::bind(&rule_base::parse_src_host_string,this,_1))) return;
    if (xml_check_value(keyname, "DstType", keyvalue, dst_type)) return;
    if (xml_check_value(keyname, "DstIp", keyvalue, dst_ip)) return;
    if (xml_check_value(keyname, "DstMask", keyvalue, dst_mask)) return;
    if (xml_check_value(keyname, "DstAddrType", keyvalue, dst_addr_type)) return;
    if (xml_check_value(keyname, "DstPortType", keyvalue, dst_port_type)) return;
    if (xml_check_value(keyname, "DstPort", keyvalue, dst_port)) return;
    if (xml_check_value(keyname, "DstPortTo", keyvalue, dst_port_to)) return;
    if (xml_check_value(keyname, "DstAtKey", keyvalue, dst_atkey)) return;
    if (xml_check_value(keyname, "DstUid", keyvalue, dst_uid)) return;
    if (xml_check_value(keyname, "DstProcNick", keyvalue, dst_procnick)) return;
    if (xml_check_value(keyname, "DstMac", keyvalue, dst_mac)) return;
    if (xml_check_value(keyname, "DstHost", keyvalue, boost::bind(&rule_base::parse_dst_host_string,this,_1))) return;
}



#ifdef UTM_WIN

LONG rule_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG rule_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
