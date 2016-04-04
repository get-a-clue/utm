#ifndef _UTM_PROC_ROW_H
#define _UTM_PROC_ROW_H

#pragma once
#include <utm.h>

#include <proc_row_base.h>

namespace utm {

class proc_row : public proc_row_base
{
public:
	static const char this_class_name[];

public:
	proc_row();
	~proc_row();

	const char *get_this_class_name() const { return this_class_name; };

	bool equals(const ubase* rhs) const;
	ubase* xml_catch_subnode(const char *tag_name, const char *class_name);

};

}

#endif // _UTM_PROC_ROW_H