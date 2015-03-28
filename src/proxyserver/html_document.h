#pragma once

#include <string>
#include <map>

#include <gstring.h>

namespace utm {

typedef std::map<std::string, gstring> meta_container;

class html_document
{
public:
	static const char this_class_name[];

	html_document(void);
	~html_document(void);

protected:
	bool has_body_tag;
	gstring title;
	meta_container meta_tags;
	gstring html;
	size_t converted_chars;
	size_t last_processed_tag;

public:
	bool get_has_body_tag() const;
	void parse(const std::string& charset, const std::string& s);
	bool find_meta(const std::string& mymeta, const std::string& charset, const char *content_signature, const char quote_char);
	bool find_title(const gstring& search_text) const;
	bool find_meta(const gstring& search_text) const;
	bool ci_check_tag(const char *tag, const char *p, size_t current_pos, size_t len) const;

	void clear();

#ifdef UTM_DEBUG
	static void test_all();
#endif
};

}
