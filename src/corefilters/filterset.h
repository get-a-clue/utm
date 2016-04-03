#ifndef UTM_FILTERSET_H
#define UTM_FILTERSET_H

#pragma once

#include <filterset_data.h>
#include <filterset_base.h>

#define START_SEQUENCE_NUMBER_FILTERID 0

namespace utm {

class filterset : public filterset_base
{
public:
	filterset(void);
	virtual ~filterset(void);

	filterset_data fdata;

	lat_as_string_container get_lat() const;
	void parse_lat_string(const char *lat_string);

	void copy_counters(const filterset& fs);
	void reset_all_counters();
	void reset_on_schedule(bool reset_history, std::string& filterids);

	void process_traffic_limit_flags(const gstring& flagfolder);
	void select_users_by_filter_id(unsigned int filter_id, std::map<unsigned int, gstring>& selected_users) const;
	void make_user_agentinfo_as_xml(const utime& cutime, const fsuser& user, std::ostringstream& res) const;

public:
	unsigned int get_kilobytes() const { return (m_bUseMiB == true) ? 1000 : 1024; };
	unsigned int get_megabytes() const { return (m_bUseMiB == true) ? 1000000 : 1048576; };
	unsigned int get_gigabytes() const { return (m_bUseMiB == true) ? 1000000000 : 1073741824; };

	void prepare_proc_usage();
	bool get_proc_usage() const { return is_proc_used; };

	void prepare_shaper_usage();
	bool get_shaper_usage() const { return is_shaper_used; };

	bool is_addrtable_used(unsigned int atkey) const;

private:
	bool is_proc_used;
	bool is_shaper_used;

public:
	ubase* xml_catch_subnode(const char *tag_name, const char *class_name);
	void xml_catch_subnode_finished(const char *name);

public:
	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num);
};

}

#endif // UTM_FILTERSET_H