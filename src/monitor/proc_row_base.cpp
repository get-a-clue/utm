// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "proc_row_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char proc_row_base::this_class_name[] = "proc_row_base";

proc_row_base::proc_row_base()
{
    clear();
}

proc_row_base::proc_row_base(const proc_row_base& rhs)
{
    operator=(rhs);
}

proc_row_base::proc_row_base(const proc_row_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

proc_row_base::~proc_row_base()
{
}

proc_row_base& proc_row_base::operator=(const proc_row_base& rhs)
{
    id = rhs.id;
    pid = rhs.pid;
    nick_id = rhs.nick_id;
    bytes_sent = rhs.bytes_sent;
    bytes_recv = rhs.bytes_recv;
    speed_sent = rhs.speed_sent;
    speed_recv = rhs.speed_recv;
    last_activity = rhs.last_activity;
    exename = rhs.exename;
    username = rhs.username;
    return *this;
}

bool proc_row_base::operator==(const proc_row_base& rhs) const
{
    return equals(&rhs);
}

bool proc_row_base::equals(const ubase* rhs) const
{
    if (rhs == NULL)
    {
       return false;
    }

    const proc_row_base* other = dynamic_cast<const proc_row_base*>(rhs);

    if (!(id == other->id)) return false;
    if (!(pid == other->pid)) return false;
    if (!(nick_id == other->nick_id)) return false;
    if (!(bytes_sent == other->bytes_sent)) return false;
    if (!(bytes_recv == other->bytes_recv)) return false;
    if (!(speed_sent == other->speed_sent)) return false;
    if (!(speed_recv == other->speed_recv)) return false;
    if (!(last_activity == other->last_activity)) return false;
    if (!(exename == other->exename)) return false;
    if (!(username == other->username)) return false;

    return true;
}

void proc_row_base::clear()
{
    id = 0;
    pid = 0;
    nick_id = 0;
    bytes_sent = 0;
    bytes_recv = 0;
    speed_sent = 0;
    speed_recv = 0;
    last_activity.clear();
    exename.clear();
    username.clear();
}

void proc_row_base::xml_create()
{
    proc_row_base orig;

    xml_append_root( PROCROW_XMLTAG_PROCROW);
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
    xml_append_node("Pid", pid, orig.pid);
    xml_append_node("Nid", nick_id, orig.nick_id);
    xml_append_node("S", bytes_sent, orig.bytes_sent);
    xml_append_node("R", bytes_recv, orig.bytes_recv);
    xml_append_node("SSp", speed_sent, orig.speed_sent);
    xml_append_node("RSp", speed_recv, orig.speed_recv);
    xml_append_node("A", last_activity, orig.last_activity);
    xml_append_node("E", exename, orig.exename);
    xml_append_node("U", username, orig.username);
}

void proc_row_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "Id", keyvalue, id)) return;
    if (xml_check_value(keyname, "Pid", keyvalue, pid)) return;
    if (xml_check_value(keyname, "Nid", keyvalue, nick_id)) return;
    if (xml_check_value(keyname, "S", keyvalue, bytes_sent)) return;
    if (xml_check_value(keyname, "R", keyvalue, bytes_recv)) return;
    if (xml_check_value(keyname, "SSp", keyvalue, speed_sent)) return;
    if (xml_check_value(keyname, "RSp", keyvalue, speed_recv)) return;
    if (xml_check_value(keyname, "A", keyvalue, last_activity)) return;
    if (xml_check_value(keyname, "E", keyvalue, exename)) return;
    if (xml_check_value(keyname, "U", keyvalue, username)) return;
}



#ifdef UTM_WIN

LONG proc_row_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG proc_row_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
