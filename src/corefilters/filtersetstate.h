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
	filtersetstate(void);
	~filtersetstate(void);

	void create_from_filterset(const filterset& fs);
	void apply_to_filterset(filterset& fs) const;

	ubase* xml_catch_subnode(const char *name);
	void xml_catch_subnode_finished(const char *name);
};

}

#endif // UTM_FILTERSTATE_H
