#pragma once

#include <ubaselist.h>
#include "procnickname.h"

namespace utm {

class procnicknamelist : public ubaselist<procnickname>
{
public:
	procnicknamelist(void);
	procnicknamelist(const procnicknamelist& rhs) { operator=(rhs); };
	~procnicknamelist(void);

	bool has_write_protection() const { return true; };
	bool find_by_pathexe(const gstring& _pathexe, procnickname& result) const;

	void clear();
    void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *tag_name, const char *class_name);
	void xml_catch_subnode_finished(const char *keyname);
};

}