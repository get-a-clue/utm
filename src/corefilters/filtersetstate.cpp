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
		filter2* pf = dynamic_cast<filter2 *>(iter->get());

		filterstate* fst = new filterstate();
		fst->filter_id = pf->get_id();
		fst->filter_name = pf->get_name();
		fst->bytes_recv = pf->cnt_recv.get_cnt();
		fst->bytes_sent = pf->cnt_sent.get_cnt();
		fst->limflags = pf->m_wTrafficLimitFlags;
		fst->lastreset.from_time_t(pf->last_reset_ts.ts);

		filterstates.add_element(fst);
	}
}

void filtersetstate::apply_to_filterset(filterset& fs) const
{
	for (auto iter = filterstates.items.begin(); iter != filterstates.items.end(); ++iter)
	{
		filterstate* fstate = dynamic_cast<filterstate *>(iter->get());
		filter2* pf = dynamic_cast<filter2 *>(fs.filters.findptr_by_id(fstate->get_id()));
		if (pf != NULL)
		{
			pf->cnt_recv.set_cnt(fstate->bytes_recv);
			pf->cnt_sent.set_cnt(fstate->bytes_sent);
			pf->m_wTrafficLimitFlags = fstate->limflags;
			pf->last_reset_ts = utm::utimestamp(fstate->lastreset.to_time_t());
		}
	}
}

ubase* filtersetstate::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, FILTERSTATE_XMLTAG_FILTER) == 0)
	{
		u = filterstates.init_and_get_temp_item(new filterstate());
	}

	return u;
}

void filtersetstate::xml_catch_subnode_finished(const char *keyname)
{
	if (strcmp(keyname, FILTERSTATE_XMLTAG_FILTER) == 0)
	{
		filterstates.commit_temp_item();
	}
}

}