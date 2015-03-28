#include "stdafx.h"
#include "filtersetstate.h"

namespace utm {

filtersetstate::filtersetstate(void)
{
}


filtersetstate::~filtersetstate(void)
{
}

void filtersetstate::create_from_filterset(const filterset& fs)
{
	clear();

	comment.assign("This file contains the current traffic counters of your filterset. This is not a final report, it is just temporary file. Don't delete it or modify it!");
	filtersetfile = fs.filterset_file;

	for (auto iter = fs.filters.items.begin(); iter != fs.filters.items.end(); ++iter)
	{
		filterstate fst;
		fst.filter_id = iter->get_id();
		fst.filter_name = iter->get_name();
		fst.bytes_recv = iter->cnt_recv.cnt;
		fst.bytes_sent = iter->cnt_sent.cnt;
		fst.limflags = iter->m_wTrafficLimitFlags;
		fst.lastreset.from_time_t(iter->last_reset_ts.ts);

		filterstates.add_element(fst);
	}
}

void filtersetstate::apply_to_filterset(filterset& fs) const
{
	for (auto iter = filterstates.items.begin(); iter != filterstates.items.end(); ++iter)
	{
		filter2* pf = fs.filters.findptr_by_id(iter->get_id());
		if (pf != NULL)
		{
			pf->cnt_recv.cnt = iter->bytes_recv;
			pf->cnt_sent.cnt = iter->bytes_sent;
			pf->m_wTrafficLimitFlags = iter->limflags;
			pf->last_reset_ts = utm::utimestamp(iter->lastreset.to_time_t());
		}
	}
}

ubase* filtersetstate::xml_catch_subnode(const char *keyname)
{
	ubase *u = NULL;

	if (strcmp(keyname, FILTERSTATE_XMLTAG_FILTER) == 0)
	{
		u = (ubase *)filterstates.get_temp_item();
		return u;
	}

	return NULL;
}

void filtersetstate::xml_catch_subnode_finished(const char *keyname)
{
	if (strcmp(keyname, FILTERSTATE_XMLTAG_FILTER) == 0)
	{
		filterstates.commit_temp_item();
		return;
	}
}

}