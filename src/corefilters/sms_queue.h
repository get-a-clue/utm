#pragma once

#include <boost/thread/mutex.hpp>

#include <ubase.h>
#include <gstring.h>
#include <list>

#include "sms.h"

#define SMS_QUEUE_XMLTAG_ROOT "SmsQueue"

namespace utm {

typedef std::list<utm::sms> sms_container;

class sms_queue : public ubase
{
public:
	static const char* this_class_name;

	enum sms_reason { sms_reason_put_queue = 0, sms_reason_expired = 1, sms_reason_delivered = 2 };
	static const char* sms_reason_str[];
	static const int sms_reason_int[];
	static const int sms_reason_qty;

	sms_queue(void);
	sms_queue(const sms_queue& rhs);
	virtual ~sms_queue(void);

    bool  operator==(const sms_queue& rhs) const;

	mutable boost::mutex guard;
	sms_container items;

	void make_sms_queue_filename(const gstring& filterset_folder, const gstring& filterset_file);
	const gstring& get_sms_queue_filename() const { return sms_filename; }

	void set_log_folder(const gstring& log_folder);
private:
	gstring sms_filename;
	gstring sms_log_folder;
	gstring sms_log_filename;

public:
	void add_sms_new_bulk(const sms_container& smslist);
	void add_sms_existed(const sms& s);
	void add_sms_new(const sms& s);
	void add_sms_new_to_users(const sms& s, const std::map<unsigned int, gstring>& selected_users);
	void select_sms_for_user(unsigned int userid, sms_container& smslist) const;
	void delivery_preparing_sms_for_user(unsigned int userid, std::ostringstream& res);
	void delete_sms_bulk(const sms_container& smslist);
	void delete_sms(const sms& s);
	void purge();
	void log_sms_bulk(const sms_container& smslist, sms_reason reason);
	void log_sms(const sms& s, sms_reason reason);
	size_t size() const;

	void clear();
    void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
    virtual ubase* xml_catch_subnode(const char *name);

#ifdef UTM_DEBUG
public:
	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num);
#endif
};

}