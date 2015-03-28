#include "stdafx.h"
#include "filtercnt.h"

#include <base64.h>

namespace utm {

filtercnt::filtercnt(void)
{
	reset(true);
}

filtercnt::~filtercnt(void)
{
}

void filtercnt::reset(bool also_reset_history)
{
	cnt = 0;
	cnt_prev = 0;

	if (also_reset_history)
	{
		cnt_logdb = 0;
		cnt_xml = 0;
		cnt_xml2 = 0;
		cnt_momspeed1 = 0;

		for (size_t i = 0; i < MAXVALUES; i++) 
		{
			cnt_set[i] = 0;
		};
	};
}

void filtercnt::copy(const filtercnt& fc)
{
	memcpy(this, &fc, sizeof(filtercnt));
}

// This procedure packs the 32-bit speed of packets flow (in bytes/sec)
// into 16-bit value to decrease an amount of transfered data over network.
//
// If the speed is less than 128 KByte/sec:
//      The most significant bit must be zero and to get the real speed
//      in bytes/sec, this value must be multipled by 4.
// If the speed is equal or more 128 Kbyte/sec:
//      The most significant bit must be one and the remaining bits define
//      the speed in KBytes/sec over 128 KByte/sec.

unsigned short filtercnt::pack_speed(unsigned int speed)
{
	if (speed < 131072)
	{
		return static_cast<unsigned short>(speed >> 2);
	};

	return static_cast<unsigned short>(0x8000 | (speed >> 10));
};

unsigned int filtercnt::unpack_speed(unsigned short packed_speed)
{
	if (packed_speed < 0x8000)
	{
		return static_cast<unsigned int>(packed_speed << 2);
	}

	return static_cast<unsigned int>((packed_speed & 0x7FFF) << 10);
};

void filtercnt::do_refresh(unsigned int total_points, unsigned int next_point)
{
	delta = cnt - cnt_prev;

	if (delta < 0)
	{
		cnt_speed = static_cast<unsigned int>(delta / -REFRESH_INTERVAL);
	}
	else
	{
		cnt_speed = static_cast<unsigned int>(delta / REFRESH_INTERVAL);
	}

	if (delta != 0)
	{
		cnt_set[next_point] = filtercnt::pack_speed(cnt_speed);

		// For XML Traffic Reports
		cnt_xml += delta;
		cnt_xml2 += delta;

		// For Database Traffic Reports
		cnt_logdb += delta;

		cnt_prev = cnt_prev + delta;
	}

	// Calc average speed for last N points

	unsigned int momspeed1 = cnt_speed;

	unsigned int n, c;
	for (c = 1; c < 5; c++)
	{
		if (total_points < c)
			break;

		n = (next_point >= c) ? (next_point - c) : ((total_points + next_point) - c);
		momspeed1 += filtercnt::unpack_speed(cnt_set[n]);
	}

	cnt_momspeed1 = momspeed1 / c;
}

void filtercnt::select_speedarray_base64(unsigned int total_points, unsigned int next_point, unsigned int array_elements_count, std::string& result) const
{
	size_t array_elements_size = static_cast<size_t>(array_elements_count*sizeof(unsigned short));
	unsigned char *p = (unsigned char*)calloc(array_elements_size, 1);
	unsigned short *psa = reinterpret_cast<unsigned short*>(p);

	bool zeroall = true;
	unsigned int n, c;
	for (c = 1; c <= array_elements_count; c++)
	{
		if (total_points > c)
		{
			n = (next_point >= c) ? (next_point - c) : ((total_points + next_point) - c);
			*psa = cnt_set[n];
			if (cnt_set[n] != 0)
				zeroall = false;
		}
		psa++;
	};

	if (!zeroall)
	{
		base64::encode(p, array_elements_size, result);
	}
	else
	{
		result.clear();
	}

	free(p);
}

}