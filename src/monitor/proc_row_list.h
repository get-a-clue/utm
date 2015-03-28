#ifndef _UTM_PROC_ROW_LIST_H
#define _UTM_PROC_ROW_LIST_H

#pragma once
#include <utm.h>

#include <ubaselist.h>
#include <proc_row.h>
#include <proc_list.h>

#define PROCROW_XMLTAG_PROCROW_TOTAL_ITEMS "PrTotalItems"

namespace utm {

class proc_row_list : public ubaselist<proc_row>
{
public:
	proc_row_list();
	~proc_row_list();

	bool operator==(const proc_row_list& rhs) const;

	void create_from_proc_list(const proc_list& plist, std::string& xml, bool include_xml_declaration = true);
	void update_proc_list(proc_list& plist);

	void clear();
	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *keyname);
};

}

#endif // _UTM_PROC_ROW_LIST_H
