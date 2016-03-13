#include "stdafx.h"

#include "stringtools.h"

#include <vector>
#include <string>

#include <boost/test/unit_test.hpp>

namespace utm {

LocalizationFuncPtr stringtools::localize = NULL;
const char stringtools::this_class_name[] = "stringtools";

struct msgpart
{
	msgpart(size_t _pos_start, size_t _pos_end, bool _localization_required)
	{
		pos_start = _pos_start;
		pos_end = _pos_end;
		localization_required = _localization_required;
	}

	size_t pos_start;
	size_t pos_end;
	bool localization_required;
};

void stringtools::LocalizeMessage(const gstring& srcmsg, gstring& dstmsg)
{
	size_t srclen = srcmsg.size();
	if (localize == NULL)
	{
		dstmsg.reserve(srclen + 1);
	}

	std::vector<msgpart> parts;

	size_t pos = 0;
	size_t pos_prev = 0;
	while (1)
	{
		pos = srcmsg.find(LSTR_BEGIN, pos_prev);
		if (pos == std::string::npos)
			break;

		if (pos_prev != pos)
		{
			msgpart p(pos_prev, pos, false);
			parts.push_back(p);
		}

		size_t pos_end = srcmsg.find(LSTR_END, pos + LSTR_BEGIN_SIZE);
		if (pos_end == std::string::npos)
			break;

		if ((pos + LSTR_BEGIN_SIZE) < pos_end)
		{
			msgpart t(pos + LSTR_BEGIN_SIZE, pos_end, true);
			parts.push_back(t);
		}

		pos_prev = pos_end + LSTR_END_SIZE;
	}

	const gchar* src = srcmsg.c_str();
	pos = 0;
	for (std::vector<msgpart>::iterator iter = parts.begin(); iter != parts.end(); ++iter)
	{
		size_t len = iter->pos_end - iter->pos_start;
		if (!iter->localization_required)
		{
			dstmsg.append(src + iter->pos_start, len);
			pos = iter->pos_end + LSTR_BEGIN_SIZE;
		}
		else
		{
			if (localize == NULL)
			{
				dstmsg.append(src + iter->pos_start, len);
			}
			else
			{
				gstring orig;
				orig.append(src + iter->pos_start, len);

				gstring loc((*localize)(orig.c_str()));
				dstmsg.append(loc.c_str());
			}
			pos = iter->pos_end + LSTR_END_SIZE;
		}
	}

	dstmsg.append(srcmsg.c_str() + pos);
}

const gchar* stringtools::strLSTR(const gchar* str)
{
	if (localize == NULL)
		return str;

	return (*localize)(str);
}

BOOST_AUTO_TEST_CASE(stringtools_test_all)
{
	std::vector<std::string> parts1;
	utm::stringtools::split(parts1, "The quick brown fox jumps over the lazy dog", ' ');
	BOOST_REQUIRE_EQUAL(parts1.size(), size_t(9));
	BOOST_REQUIRE_EQUAL(parts1[0], std::string("The"));
	BOOST_REQUIRE_EQUAL(parts1[1], std::string("quick"));

	return;
}

}