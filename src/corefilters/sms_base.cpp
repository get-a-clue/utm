// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "sms_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char sms_base::this_class_name[] = "sms_base";

sms_base::sms_base()
{
    clear();
}

sms_base::sms_base(const sms_base& rhs)
{
    operator=(rhs);
}

sms_base::sms_base(const sms_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

sms_base::~sms_base()
{
}

sms_base& sms_base::operator=(const sms_base& rhs)
{
    sender_uid = rhs.sender_uid;
    sender_name = rhs.sender_name;
    recp_uid = rhs.recp_uid;
    recp_name = rhs.recp_name;
    time_creation = rhs.time_creation;
    time_expire = rhs.time_expire;
    msg_text = rhs.msg_text;
    msg_id = rhs.msg_id;
    return *this;
}

bool sms_base::operator==(const sms_base& rhs) const
{
    return equals(&rhs);
}

bool sms_base::equals(const ubase* rhs) const
{
    if (rhs == NULL)
    {
       return false;
    }

    const sms_base* other = dynamic_cast<const sms_base*>(rhs);

    if (!(sender_uid == other->sender_uid)) return false;
    if (!(sender_name == other->sender_name)) return false;
    if (!(recp_uid == other->recp_uid)) return false;
    if (!(recp_name == other->recp_name)) return false;
    if (!(time_creation == other->time_creation)) return false;
    if (!(time_expire == other->time_expire)) return false;
    if (!(msg_text == other->msg_text)) return false;
    if (!(msg_id == other->msg_id)) return false;

    return true;
}

void sms_base::clear()
{
    sender_uid = 0;
    sender_name.clear();
    recp_uid = 0;
    recp_name.clear();
    time_creation = 0;
    time_expire = 0;
    msg_text.clear();
    msg_id.clear();
}

void sms_base::xml_create()
{
    sms_base orig;

    xml_append_root( SMS_XMLTAG_ROOT);
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

    xml_append_node("SenderUid", sender_uid, orig.sender_uid);
    xml_append_node("SenderName", sender_name, orig.sender_name);
    xml_append_node("RecpUid", recp_uid, orig.recp_uid);
    xml_append_node("RecpName", recp_name, orig.recp_name);
    xml_append_node("Create", time_creation, orig.time_creation);
    xml_append_node("Expire", time_expire, orig.time_expire);
    xml_append_node("MsgText", msg_text, orig.msg_text);
    xml_append_node("MsgId", msg_id, orig.msg_id);
}

void sms_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "SenderUid", keyvalue, sender_uid)) return;
    if (xml_check_value(keyname, "SenderName", keyvalue, sender_name)) return;
    if (xml_check_value(keyname, "RecpUid", keyvalue, recp_uid)) return;
    if (xml_check_value(keyname, "RecpName", keyvalue, recp_name)) return;
    if (xml_check_value(keyname, "Create", keyvalue, time_creation)) return;
    if (xml_check_value(keyname, "Expire", keyvalue, time_expire)) return;
    if (xml_check_value(keyname, "MsgText", keyvalue, msg_text)) return;
    if (xml_check_value(keyname, "MsgId", keyvalue, msg_id)) return;
}



#ifdef UTM_WIN

LONG sms_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG sms_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
