#include "stdafx.h"
#include "html_document.h"

#include <ubase_test.h>

#include <algorithm>

#define BODY_START_TAG "<body"
#define TITLE_START_TAG "<title>"
#define TITLE_END_TAG "</title>"
#define META_START_TAG "<meta "
#define CONTENT_SIGNATURE_SINGLE "content='"
#define CONTENT_SIGNATURE_DOUBLE "content=\""
#define META_TAG_MAX_SIZE 16384

namespace utm {

const char html_document::this_class_name[] = "html_document";

html_document::html_document(void)
{
	clear();
}

html_document::~html_document(void)
{
}

void html_document::clear()
{
	last_processed_tag = 0;
	converted_chars = 0;
	has_body_tag = false;
	title.clear();
	html.clear();
	meta_tags.clear();
}

bool html_document::get_has_body_tag() const
{
	return has_body_tag;
}

//
// This function finds "title" and "meta" tags inside html document.
// Also, it converts found strings from given charset.
//
void html_document::parse(const std::string& charset, const std::string& s)
{
	std::string::size_type len = s.size();
	if (len == 0)
		return;

	if (has_body_tag)
		return;

	const char *begin = s.c_str() + last_processed_tag;
	const char *p = begin;
	size_t i = last_processed_tag;

	std::string mytitle;
	bool in_title = false;
	size_t title_start_pos = 0;
	size_t title_end_pos = 0;

	std::string mymeta;
	bool in_meta = false;
	size_t meta_start_pos = 0;
	size_t meta_end_pos = 0;

	bool b;
	char c;

	while (i < len)
	{
		c = *p;
		b = ((c == '<') || (c == '>'));

		i++;
		p++;

		if (!b)
		{
			continue;
		}

		if ((!in_meta) && (c == '<'))
		{
			if (ci_check_tag("title", p, i, len))
			{
				in_title = true;
				title_start_pos = i + sizeof("title");
				continue;
			}

			if (in_title)
			{
				if (ci_check_tag("/title", p, i, len))
				{
					in_title = false;
					title_end_pos = i;

					mytitle = std::string(begin + title_start_pos, title_end_pos - title_start_pos - 1);
					title = gstring::convert(mytitle, charset, true);

					last_processed_tag = i;
					continue;
				}
			}

			if (ci_check_tag("meta", p, i, len))
			{
				in_meta = true;
				meta_start_pos = i + sizeof("meta");
				continue;
			}

			if (ci_check_tag("body", p, i, len))
			{
				last_processed_tag = i;
				has_body_tag = true;
				break;
			}
		}

		if ((in_meta) && (c == '>'))
		{
			meta_end_pos = i;
			mymeta = std::string(s.c_str() + meta_start_pos, meta_end_pos - meta_start_pos);

			if (find_meta(mymeta, charset, CONTENT_SIGNATURE_DOUBLE, '"'))
			{
				last_processed_tag = i;
				in_meta = false;
			}
			else
			if (find_meta(mymeta, charset, CONTENT_SIGNATURE_SINGLE, '\''))
			{
				last_processed_tag = i;
				in_meta = false;
			}
		}
	}
}

bool html_document::find_meta(const std::string& mymeta, const std::string& charset, const char *content_signature, const char quote_char)
{
	std::string::size_type content_pos_start = mymeta.find(content_signature);
	if (content_pos_start != std::string::npos)
	{
		std::string::size_type content_pos_finish = mymeta.rfind(quote_char);
		if (content_pos_finish != std::string::npos)
		{
			bool end_tag_found = true;

			char r;
			size_t pos = content_pos_finish + 1;
			size_t metasize = mymeta.size() - 1;

			while (pos < metasize)
			{
				r = mymeta.at(pos);

				if ((r != ' ') && (r != '/'))
				{
					end_tag_found = false;
					break;
				}
				pos++;
			}

			if (end_tag_found)
			{
				size_t content_signature_length = strlen(content_signature);
				size_t content_size = content_pos_finish - content_pos_start - content_signature_length;

				if (content_size < META_TAG_MAX_SIZE)
				{
					std::string content = mymeta.substr(content_pos_start + content_signature_length, content_size);
					gstring value = gstring::convert(content, charset, true);
					std::string key = mymeta.substr(0, content_pos_start - 1);

					std::map<std::string, gstring>::iterator iter = meta_tags.find(key);
					if (iter != meta_tags.end())
					{
						gstring oldvalue = iter->second;
						oldvalue.append(value.c_str());
						iter->second = oldvalue;
					}
					else
					{
						std::pair<std::string, gstring> p(key, value);
						meta_tags.insert(p);
					}
				}

				return true;
			}
		}
	}

	return false;
}

bool html_document::ci_check_tag(const char *tag, const char *content, size_t current_pos, size_t contentlen) const
{
	bool retval = false;

	char tag_char;
	char content_char;

	size_t i = 0;
	size_t taglen = strlen(tag);

	while ((current_pos + i) < contentlen)
	{
		content_char = *(content + i);
		tag_char = *(tag + i);

		if (content_char <= 0)
			break;

		if ((content_char != tag_char) && (content_char != toupper(tag_char)))
			break;

		i++;
		if (taglen == i)
		{
			if ((current_pos + i) < contentlen)
			{
				content_char = *(content + i);
				retval = (content_char == ' ' || content_char == '>');
			}

			break;
		}
	}

	return retval;
}

bool html_document::find_title(const gstring& search_text) const
{
	size_t pos = title.find(search_text.c_str(), 0);

	return (pos != std::string::npos);
}

bool html_document::find_meta(const gstring& search_text) const
{
	bool retval = false;

	meta_container::const_iterator iter;
	for (iter = meta_tags.begin(); iter != meta_tags.end(); ++iter)
	{
		size_t pos = (*iter).second.find(search_text.c_str(), 0);

		if (pos != std::string::npos)
		{
			retval = true;
			break;
		}
	}

	return retval;
}

#ifdef UTM_DEBUG
void html_document::test_all()
{
	test_report tr(this_class_name);

	std::string wc = "<HTML><TITLE>my title</TITLE>";
	wc.append("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1252\">");
	wc.append("<meta name=\"description\" content=\"This is the test porno description\">");
	wc.append("<BODY>azxcv</BODY></HTML>");

	html_document hd;
	std::string c = "windows-1252";
	hd.parse(c, wc);

	gstring s1("president");
	gstring s2("porno");

	bool b1 = hd.find_meta(s1);
	if (b1)
		throw test_exception(this_class_name, "1");

	bool b2 = hd.find_meta(s2);
	if (!b2)
		throw test_exception(this_class_name, "1");

	return;
}
#endif
}