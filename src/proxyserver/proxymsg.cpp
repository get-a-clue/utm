#include "stdafx.h"
#include "proxymsg.h"

#include <gstring.h>
#include <utime.h>
#include <stringtools.h>

#include <string>
#include <vector>

namespace utm {

const char proxymsg::this_class_name[] = "proxymsg";

const char* proxymsg::msgtypes_str_eng[] = {
	PROXYMSG_MSGTYPE_NONE_ENG,
	PROXYMSG_MSGTYPE_NOTCONFIGURED_ENG,
	PROXYMSG_MSGTYPE_NOTACCEPT_ENG,
	PROXYMSG_MSGTYPE_BLOCK_ENG,
	PROXYMSG_MSGTYPE_NOTRESOLVED_ENG,
	PROXYMSG_MSGTYPE_GOOGLEERROR_ENG
};

const char* proxymsg::msgtypes_str_rus[] = {
	PROXYMSG_MSGTYPE_NONE_RUS,
	PROXYMSG_MSGTYPE_NOTCONFIGURED_RUS,
	PROXYMSG_MSGTYPE_NOTACCEPT_RUS,
	PROXYMSG_MSGTYPE_BLOCK_RUS,
	PROXYMSG_MSGTYPE_NOTRESOLVED_RUS,
	PROXYMSG_MSGTYPE_GOOGLEERROR_RUS
};

const int proxymsg::msgtypes_int[] = {
	(int)proxymsg::msgtype_none,
	(int)proxymsg::msgtype_notconfigured,
	(int)proxymsg::msgtype_notaccept,
	(int)proxymsg::msgtype_block,
	(int)proxymsg::msgtype_notresolved,
	(int)proxymsg::msgtype_googleerror
};

proxymsg::proxymsg(void)
{
	fill_by_default();
}

proxymsg::~proxymsg(void)
{
}

bool proxymsg::operator==(const proxymsg& rhs) const
{
	if (messages.size() != rhs.messages.size())
		return false;

	replymsg_container::const_iterator iter;
	replymsg_container::const_iterator iter2 = rhs.messages.begin();
	for (iter = messages.begin(); iter != messages.end(); ++iter, ++iter2)
	{
		if (!((*iter) == (*iter2)))
			return false;
	}

	return true;
}

void proxymsg::fill_by_default()
{
	for (int i = 0; i < PROXYMSG_MSGTYPE_MAXITEMS; i++)
	{
		int msgtype = msgtypes_int[i];
		const char *m = (get_lang() == (int)lang_eng) ? msgtypes_str_eng[i] : msgtypes_str_rus[i];
		set_message(msgtype, gstring(m), PROXYMSG_MSGTYPE_REPLY_CODE);
	}

	get_default_header_footer(get_lang(), header, footer);
}

void proxymsg::set_message(int msgtype, const gstring& msg, int http_code)
{
	replymsg m;
	m.msg = msg;
	m.http_code = http_code;

	if (messages.find(msgtype) == messages.end())
	{
		messages.insert(std::pair<int, replymsg>(msgtype, m));
	}
	else
	{
		messages[msgtype] = m;
	}
}

void proxymsg::get_message(int msgtype, const char *requestedsite, const char *proxyrulename, std::string& msg, int& http_code)
{
	std::string p;
	http_code = PROXYMSG_MSGTYPE_REPLY_CODE;

	replymsg_container::iterator iter = messages.find(msgtype);
	if (iter != messages.end())
	{
		p = iter->second.msg.getutf8();
		http_code = iter->second.http_code;
	}
	else
	{
		gstring g = (get_lang() == (int)lang_eng) ? gstring(msgtypes_str_eng[0]) : gstring(msgtypes_str_rus[0]);
		p = g.getutf8();
	}

	format_message(p.c_str(), header.getutf8().c_str(), footer.getutf8().c_str(), requestedsite, proxyrulename, msg);
}

void proxymsg::get_default_header_footer(int msgtype, gstring& hdr, gstring& ftr)
{
	if (msgtype == (int)lang_eng)
	{
		hdr.assign(PROXYMSG_DEFAULT_HEADER_ENG);
		ftr.assign(PROXYMSG_DEFAULT_FOOTER_ENG);
	}
	else
	{
		hdr.assign(PROXYMSG_DEFAULT_HEADER_RUS);
		ftr.assign(PROXYMSG_DEFAULT_FOOTER_RUS);
	}
}

void proxymsg::get_default_message(int msgtype, const char *browserlang, std::string& msg, int& http_code)
{
	int lang = parse_browser_language(browserlang);

	http_code = PROXYMSG_MSGTYPE_REPLY_CODE;

	for (int i = 0; i < PROXYMSG_MSGTYPE_MAXITEMS; i++)
	{
		if (msgtypes_int[i] == msgtype)
		{
			gstring p, hdr, ftr;
			get_default_header_footer(lang, hdr, ftr);

			if (lang == (int)lang_eng)
			{
				p.assign(msgtypes_str_eng[i]);
			}
			else
			{
				p.assign(msgtypes_str_rus[i]);
			}

			format_message(p.getutf8().c_str(), hdr.getutf8().c_str(), ftr.getutf8().c_str(), NULL, NULL, msg);

			return;
		}
	}
}

void proxymsg::format_message(const char *message, const char *header, const char *footer, const char *requestedsite, const char *proxyrulename, std::string& result)
{
	std::ostringstream oss;
	oss << header << message << footer;

	result = oss.str();

	{
		size_t pos = result.find(PROXYMSG_PATTERN_TIMESTAMP);
		if (pos != std::string::npos)
		{
			utime ut(true);
			result = result.replace(pos, sizeof(PROXYMSG_PATTERN_TIMESTAMP) - 1, ut.to_string(utime::format_log));
		}
	}

	{
		size_t pos = result.find(PROXYMSG_PATTERN_HOST);
		if (pos != std::string::npos)
		{
			char hostname[250];
			if (0 == gethostname(hostname, 250))
			{
				result = result.replace(pos, sizeof(PROXYMSG_PATTERN_HOST) - 1, hostname);
			}
		}
	}

	{
		size_t pos = result.find(PROXYMSG_PATTERN_SITE);
		if (pos != std::string::npos)
		{
			result = result.replace(pos, sizeof(PROXYMSG_PATTERN_SITE) - 1, requestedsite != NULL ? requestedsite : "");
		}
	}

	{
		size_t pos = result.find(PROXYMSG_PATTERN_PROXYRULE);
		if (pos != std::string::npos)
		{
			result = result.replace(pos, sizeof(PROXYMSG_PATTERN_PROXYRULE) - 1, proxyrulename != NULL ? proxyrulename : "");
		}
	}
}

void proxymsg::clear()
{
	messages.clear();
}

void proxymsg::xml_create()
{
	if (get_lang() == (int)lang_eng)
	{
		xml_append_root(PROXYMSG_XMLTAG_ENG);
	}
	else
	{
		xml_append_root(PROXYMSG_XMLTAG_RUS);
	}

	xml_append_node(PROXYMSG_XMLTAG_HEADER, header);
	xml_append_node(PROXYMSG_XMLTAG_FOOTER, footer);

	replymsg_container::iterator iter;
	for (iter = messages.begin(); iter != messages.end(); ++iter)
	{
		gstring u;
		u << iter->first << "|" << iter->second.http_code << "|" << iter->second.msg;
		xml_append_node(PROXYMSG_XMLTAG_MSG, u);
	}
}

void proxymsg::xml_catch_value(const char *keyname, const char *keyvalue)
{
	std::string tmp;

	while (1)
	{
		if (xml_check_value(keyname, PROXYMSG_XMLTAG_HEADER, keyvalue, header)) break;
		if (xml_check_value(keyname, PROXYMSG_XMLTAG_FOOTER, keyvalue, footer)) break;

		if (xml_check_value(keyname, PROXYMSG_XMLTAG_MSG, keyvalue, tmp))
		{
			std::vector<std::string> k;
			stringtools::split(k, tmp.c_str(), '|', split::no_empties);
			if (k.size() == 3)
			{
				int msgtype = atoi(k[0].c_str());
				int http_code = atoi(k[1].c_str());
				gstring g; g.assign_fromutf8(k[2].c_str());
				set_message(msgtype, g, http_code);
			}
		}
		break;
	}
}

int proxymsg::parse_browser_language(const char *browserlang)
{
	if (browserlang == NULL)
		return (int)lang_eng;

	int len = strlen(browserlang);
	std::vector<char> lang;
	lang.reserve(len + 1);
	while (len-- > 0)
	{
		char c = *browserlang++;
		if (c != 0x20)
			lang.push_back(c);
	}
	lang.push_back(0x00);

	const char *b = &(*lang.begin());

	const char *r = strstr(b, "ru");
	if (r == b)
		return (int)lang_rus;

	if ((r != NULL) && (*(r--) == ','))
		return (int)lang_rus;

	return (int)lang_eng;
}

void proxymsg::test_fillparams(int test_num)
{
	fill_by_default();
}

}
