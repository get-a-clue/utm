#ifndef UTM_FILTERSETSTATE_H
#define UTM_FILTERSETSTATE_H

#pragma once
#include <utm.h>

#include <filtersetstate_base.h>
#include <filterset.h>

namespace utm {

class filtersetstate : public filtersetstate_base
{
public:
	static const char this_class_name[];

public:
	filtersetstate(void);
	~filtersetstate(void);

	const char *get_this_class_name() const { return this_class_name; };

	void create_from_filterset(const filterset& fs);
	void apply_to_filterset(filterset& fs) const;

	ubase* xml_catch_subnode(const char *tag_name, const char *class_name);
	void xml_catch_subnode_finished(const char *name);
};

}

#endif // UTM_FILTERSTATE_H
