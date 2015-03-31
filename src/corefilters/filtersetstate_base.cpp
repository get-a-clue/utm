// This is auto-generated file. Do not modify it!

#include "stdafx.h"
#include "filtersetstate_base.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace utm {

const char filtersetstate_base::this_class_name[] = "filtersetstate_base";

filtersetstate_base::filtersetstate_base()
{
    clear();
}

filtersetstate_base::filtersetstate_base(const filtersetstate_base& rhs)
{
    operator=(rhs);
}

filtersetstate_base::filtersetstate_base(const filtersetstate_base& rhs, bool make_threadsafe_copyobj)
{

    operator=(rhs);
}

filtersetstate_base::~filtersetstate_base()
{
}

filtersetstate_base& filtersetstate_base::operator=(const filtersetstate_base& rhs)
{
    comment = rhs.comment;
    filtersetfile = rhs.filtersetfile;
    filterstates = rhs.filterstates;
    lastmodified = rhs.lastmodified;
    return *this;
}

bool filtersetstate_base::operator==(const filtersetstate_base& rhs) const
{
    if (!(comment == rhs.comment)) return false;
    if (!(filtersetfile == rhs.filtersetfile)) return false;
    if (!(filterstates == rhs.filterstates)) return false;
    if (!(lastmodified == rhs.lastmodified)) return false;

    return true;
}

void filtersetstate_base::clear()
{
    comment.clear();
    filtersetfile.clear();
    filterstates.clear();
    lastmodified.clear();
}

void filtersetstate_base::xml_create()
{
    filtersetstate_base orig;

    xml_append_root( FILTERSETSTATE_XMLTAG_TRAFFICCOUNTERS);
    if (xml_has_root_attr()) {
       xmlattr_container attr;
       xml_get_root_attr(attr);
       for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
          pugi::xml_node root = xml_get_root();
          pugi::xml_attribute attr = root.append_attribute(iter->first.c_str());
          attr.set_value(iter->second.c_str());
       }
    }

    xml_append_node("Comment", comment, orig.comment);
    xml_append_node("FiltersetFile", filtersetfile, orig.filtersetfile);
    xml_append_node("Filter", get_filterstates());
    xml_append_node("LastModification", lastmodified, orig.lastmodified);
}

void filtersetstate_base::xml_catch_value(const char *keyname, const char *keyvalue)
{
    if (xml_check_value(keyname, "Comment", keyvalue, comment)) return;
    if (xml_check_value(keyname, "FiltersetFile", keyvalue, filtersetfile)) return;
    if (xml_check_value(keyname, "Filter", keyvalue, boost::bind(&filtersetstate_base::parse_filterstate_string,this,_1))) return;
    if (xml_check_value(keyname, "LastModification", keyvalue, lastmodified)) return;
}



#ifdef UTM_WIN

LONG filtersetstate_base::SaveToRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
    HKEY h;
    LONG result = RegOpenKeyEx(hk, pRegistryPath, 0, KEY_ALL_ACCESS, &h);
    if (result == ERROR_SUCCESS) {



    RegCloseKey(h);
    };

    return result;
}

LONG filtersetstate_base::ReadFromRegistry(const TCHAR* pRegistryPath, const HKEY hk)
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
