#ifndef _UTM_URLFILTER_H
#define _UTM_URLFILTER_H

#pragma once

#define URLFILTERING_NO_MATCH 0
#define URLFILTERING_POSITIVE_MATCH 1
#define URLFILTERING_BLOCK_MATCH 2

#include "urlfilter_base.h"

namespace utm {

class urlfilter : public urlfilter_base
{
public:
	static const char this_class_name[];

public:
	urlfilter(void);
	~urlfilter(void);

	const char *get_this_class_name() const { return this_class_name; };

	void set_id(unsigned int id) { ufid = id; };
	unsigned int get_id() const { return ufid; };
	const gstring& get_name() const { return name; };

	std::string get_urlrules_str() const;
	std::string get_urlrules_str(const std::string& delimiter) const;
	void parse_urlrules_str(const char *p);
	void parse_urlrules_str(const char *p, char delimiter);

	bool check_fid(unsigned int filter_id) const;
	int match(const char *host, const char *uri, const char *hosturi) const;

	static int match_rule(const char* rule_str, const char *urladdr);
	static int match_rule(const char* rule_str, const char *urladdr, size_t* poutpos);

#ifdef UTM_DEBUG
	static void test_all();
#endif
};

}

#endif // _UTM_URLFILTER_H