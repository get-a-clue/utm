#include "stdafx.h"
#include "filterset_data.h"

namespace utm {

const char filterset_data::this_class_name[] = "filterset_data";

filterset_data::filterset_data()
{
	clear();
}


filterset_data::~filterset_data()
{
}

void filterset_data::copy(const filterset_data& fd)
{
	memcpy(this, &fd, sizeof(filterset_data));
}

void filterset_data::clear()
{
	memset(this, 0, sizeof(filterset_data));
}


void filterset_data::goto_next_point(time_t current_time, unsigned int current_seqnum)
{
	if (current_seqnum != 0)
		next_seqnum = current_seqnum;

	seqnum_array[next_point] = next_seqnum;
	time_array[next_point] = static_cast<fdata_time>(current_time);

	if (total_points < MAXVALUES)
		total_points++;

	if (next_point >= (MAXVALUES - 1))
		next_point = 0;
	else
		next_point++;

	next_seqnum++;
}

unsigned int filterset_data::get_current_point() const
{
	if (total_points == 0)
		return 0;

	if ((next_point == 0) || (next_point >= (MAXVALUES - 1)))
		return (MAXVALUES - 1);

	return (next_point - 1);
}

void filterset_data::calc_start_last_index(unsigned int total_points, unsigned int next_point, unsigned int& start_index, unsigned int& last_index)
{
	last_index = 0;

	if (total_points < MAXVALUES)
	{
		last_index = total_points;
	}
	else
	{
		start_index = next_point;
		last_index = start_index + MAXVALUES;
	}
}

void filterset_data::create_diff(const filterset_data& fd, unsigned int seqnum, unsigned int& diff_size, unsigned int& start_diffindex)
{
	total_points = fd.total_points;
	next_point = fd.next_point;
	next_seqnum = fd.next_seqnum;

	unsigned int start_index = 0;
	unsigned int last_index = 0;
	calc_start_last_index(total_points, next_point, start_index, last_index);
	
	diff_size = 0;
	start_diffindex = 0;
	unsigned int curindex;
	for (unsigned int i = start_index; i < last_index; i++)
	{
		curindex = i % MAXVALUES;
		if (seqnum < fd.seqnum_array[curindex])
		{
			start_diffindex = i;
			diff_size = (last_index - start_diffindex);

			if (diff_size <= MAXVALUES)
			{
				unsigned int n;
				for (unsigned int k = 0; k < diff_size; k++, i++)
				{
					n = i % MAXVALUES;
					time_array[k] = fd.time_array[n];
					seqnum_array[k] = fd.seqnum_array[n];
				}
			}
			else
				throw std::exception("Fantastic case");

			break;
		}
	}
}

#ifdef UTM_DEBUG
void filterset_data::test_all()
{
	test_report tr(this_class_name);

	const unsigned int TEST_POINTS = 20;
	unsigned int diff_size, start_diffindex;
	{
		unsigned int POINTS = TEST_POINTS;
		filterset_data fd;

		fd.total_points = POINTS;
		fd.next_point = POINTS;
		for (unsigned int i = 0; i < POINTS; i++)
		{
			fd.seqnum_array[i] = i + 1;
			fd.time_array[i] = i + 1001;
		}

		{
			filterset_data fdiff;
			fdiff.create_diff(fd, 0, diff_size, start_diffindex);

			TEST_CASE_CHECK(fdiff.total_points, fd.total_points);
			TEST_CASE_CHECK(fdiff.next_point, fd.next_point);
			TEST_CASE_CHECK(diff_size, POINTS);
			TEST_CASE_CHECK(start_diffindex, unsigned int(0));
			for (unsigned int j = 0; j < MAXVALUES; j++)
			{
				if (j < POINTS)
				{
					TEST_CASE_CHECK(fd.seqnum_array[j], fdiff.seqnum_array[j]);
					TEST_CASE_CHECK(fd.time_array[j], fdiff.time_array[j]);
				}
				else
				{
					TEST_CASE_CHECK(unsigned int(0), fdiff.seqnum_array[j]);
					TEST_CASE_CHECK(fdata_time(0), fdiff.time_array[j]);
				}
			}
		}

		{
			filterset_data fdiff;
			fdiff.create_diff(fd, POINTS / 2, diff_size, start_diffindex);

			TEST_CASE_CHECK(fdiff.total_points, fd.total_points);
			TEST_CASE_CHECK(fdiff.next_point, fd.next_point);
			TEST_CASE_CHECK(diff_size, POINTS / 2);
			TEST_CASE_CHECK(start_diffindex, unsigned int(POINTS / 2));
			for (unsigned int j = 0; j < MAXVALUES; j++)
			{
				if (j < TEST_POINTS / 2)
				{
					TEST_CASE_CHECK(fd.seqnum_array[j + (POINTS / 2)], fdiff.seqnum_array[j]);
					TEST_CASE_CHECK(fd.time_array[j + (POINTS / 2)], fdiff.time_array[j]);
				}
				else
				{
					TEST_CASE_CHECK(unsigned int(0), fdiff.seqnum_array[j]);
					TEST_CASE_CHECK(fdata_time(0), fdiff.time_array[j]);
				}
			}
		}
	}

	{
		unsigned int POINTS = TEST_POINTS + MAXVALUES;
		filterset_data fd;

		fd.total_points = POINTS;
		fd.next_point = POINTS % MAXVALUES;
		for (unsigned int i = 0; i < POINTS; i++)
		{
			unsigned int curindex = i % MAXVALUES;
			fd.seqnum_array[curindex] = i + 1;
			fd.time_array[curindex] = i + 1001;
		}

		{
			filterset_data fdiff;
			fdiff.create_diff(fd, 0, diff_size, start_diffindex);

			TEST_CASE_CHECK(fdiff.total_points, fd.total_points);
			TEST_CASE_CHECK(fdiff.next_point, fd.next_point);
			for (unsigned int j = 0; j < MAXVALUES; j++)
			{
				unsigned int src_index = (j + TEST_POINTS) % MAXVALUES;
				TEST_CASE_CHECK(fd.seqnum_array[src_index], fdiff.seqnum_array[j]);
				TEST_CASE_CHECK(fd.time_array[src_index], fdiff.time_array[j]);
			}
		}

		for (unsigned int k = 0; k < MAXVALUES; k += 8)
		{
			filterset_data fdiff;
			fdiff.create_diff(fd, TEST_POINTS + k, diff_size, start_diffindex);

			TEST_CASE_CHECK(fdiff.total_points, fd.total_points);
			TEST_CASE_CHECK(fdiff.next_point, fd.next_point);
			TEST_CASE_CHECK(diff_size, MAXVALUES - k);
			for (unsigned int j = 0; j < MAXVALUES; j++)
			{
				unsigned int src_index = (j + TEST_POINTS + k) % MAXVALUES;
				if (j < (MAXVALUES - k))
				{
					TEST_CASE_CHECK(fd.seqnum_array[src_index], fdiff.seqnum_array[j]);
					TEST_CASE_CHECK(fd.time_array[src_index], fdiff.time_array[j]);
				}
				else
				{
					TEST_CASE_CHECK(unsigned int(0), fdiff.seqnum_array[j]);
					TEST_CASE_CHECK(fdata_time(0), fdiff.time_array[j]);
				}
			}
		}

		{
			filterset_data fdiff;
			fdiff.create_diff(fd, TEST_POINTS + MAXVALUES - 1, diff_size, start_diffindex);
			TEST_CASE_CHECK(diff_size, unsigned int(1));
			TEST_CASE_CHECK(start_diffindex, unsigned int(TEST_POINTS + MAXVALUES - 1));
		}

		{
			filterset_data fdiff;
			fdiff.create_diff(fd, TEST_POINTS + MAXVALUES, diff_size, start_diffindex);
			TEST_CASE_CHECK(diff_size, unsigned int(0));
			TEST_CASE_CHECK(start_diffindex, unsigned int(0));
		}

		{
			filterset_data fdiff;
			fdiff.create_diff(fd, TEST_POINTS + MAXVALUES + 1, diff_size, start_diffindex);
			TEST_CASE_CHECK(diff_size, unsigned int(0));
			TEST_CASE_CHECK(start_diffindex, unsigned int(0));
		}
	}

}
#endif


}