#ifndef _UTM_FILTERSETCONS_H
#define _UTM_FILTERSETCONS_H

#pragma once
#include <utm.h>

#include <filtersetcons_base.h>
#include <filterset.h>

namespace utm {

class filtersetcons : public filtersetcons_base
{
public:
	filtersetcons();
	~filtersetcons();

	void fill_from_filterset(const filterset& fs, unsigned int seqnum);
	void apply_to_filterset(filterset& fs);

	ubase* xml_catch_subnode(const char *keyname);
	void xml_catch_subnode_finished(const char *keyname);

};

}

#endif // _UTM_FILTERSETCONS_H