#include "stdafx.h"
#include "filtersetcons.h"

#include "filtercons.h"

namespace utm {

filtersetcons::filtersetcons()
{
}


filtersetcons::~filtersetcons()
{
}

void filtersetcons::fill_from_filterset(const filterset& fs, unsigned int seqnum)
{
	// NOTE: diff_size, start_diffindex are class members

	filterset_data fdata;
	fdata.create_diff(fs.fdata, seqnum, diff_size, start_diffindex);

	total_points = fdata.get_total_points();
	next_seqnum = fdata.get_next_seqnum();

	diff_seqnum.set(reinterpret_cast<unsigned char*>(fdata.seqnum_array), diff_size*sizeof(unsigned int));
	diff_time.set(reinterpret_cast<unsigned char*>(fdata.time_array), diff_size*sizeof(fdata_time));

	for (auto iter = fs.filters.items.begin(); iter != fs.filters.items.end(); ++iter)
	{
		std::unique_ptr<unsigned short> diff(new unsigned short[diff_size * 2]);
		filter2* pf = dynamic_cast<filter2 *>(iter->get());

		pf->select_diffspeed(diff_size, start_diffindex, diff.get(), diff.get() + diff_size);

		filtercons* fc = new filtercons();
		fc->filter_id = pf->get_id();
		fc->bytes_sent = pf->cnt_sent.get_cnt();
		fc->bytes_recv = pf->cnt_recv.get_cnt();
		fc->speed_array.set(reinterpret_cast<unsigned char*>(diff.get()), diff_size*2*sizeof(unsigned short));

		filtercons_s.add_element(fc);
	}
}

void filtersetcons::apply_to_filterset(filterset& fs)
{
	unsigned int next_point = fs.fdata.get_next_point();
	unsigned int seqnum_points = diff_seqnum.size() / sizeof(unsigned int);
	unsigned int* seqnum_data = reinterpret_cast<unsigned int*>(diff_seqnum.get());

	unsigned int array_points = diff_time.size() / sizeof(fdata_time);
	fdata_time* array_data = reinterpret_cast<fdata_time*>(diff_time.get());
	for (unsigned int i = 0; i < seqnum_points; i++)
	{
		fs.fdata.goto_next_point(*array_data, *seqnum_data);
		array_data++;
		seqnum_data++;
	}

	auto filter_iter = fs.filters.items.begin();
	bool plain_mode = true;
	filter2* pf;

	for (auto iter = filtercons_s.items.begin(); iter != filtercons_s.items.end(); ++iter)
	{
		filtercons* fc = dynamic_cast<filtercons *>(iter->get());

		unsigned int filter_id = fc->filter_id;
		if ((plain_mode) && (filter_iter != fs.filters.items.end()) && (filter_iter->get()->get_id() == filter_id))
		{
			pf = dynamic_cast<filter2 *>(filter_iter->get());
			++filter_iter;
		}
		else
		{
			plain_mode = false;
			pf = dynamic_cast<filter2 *>(fs.filters.findptr_by_id(filter_id));
			if (pf == NULL)
				continue;
		}

		pf->cnt_recv.set_cnt(fc->bytes_recv);
		pf->cnt_sent.set_cnt(fc->bytes_sent);
		unsigned int points = fc->speed_array.size() / sizeof(unsigned short);
		unsigned short* diff_sent = reinterpret_cast<unsigned short*>(fc->speed_array.get());
		unsigned short* diff_recv = reinterpret_cast<unsigned short*>(fc->speed_array.get());
		diff_recv += points / 2;
		pf->apply_diffspeed(points / 2, next_point, diff_sent, diff_recv);
	}
}

ubase* filtersetcons::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, "F") == 0)
	{
		u = filtercons_s.init_and_get_temp_item(new filtercons());
	}

	return u;
}

void filtersetcons::xml_catch_subnode_finished(const char *keyname)
{
	if (strcmp(keyname, "F") == 0)
	{
		filtercons_s.commit_temp_item();
	}
}

}