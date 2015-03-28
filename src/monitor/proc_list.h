#ifndef UTM_PROC_LIST_H
#define UTM_PROC_LIST_H

#pragma once
#include <utm.h>

#ifdef UTM_WIN

#include <utimestamp.h>
#include <gstring.h>
#include <procnicknamelist.h>
#include <userlist.h>

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <iostream>

#define UTM_MAX_PROC_COUNT 16384
#define UTM_MAX_PROC_PID 65536

namespace utm {

struct proc_info {
	proc_info() { clear(); }

	inline void clear() { memset(this, 0, sizeof(proc_info)); };
	void reset_counters()
	{
		bytes_sent = 0;
		bytes_recv = 0;
		bytes_sent_prev = 0;
		bytes_recv_prev = 0;
		speed_sent = 0;
		speed_recv = 0;
		last_activity.clear();
	};

	unsigned short flag;
	unsigned short nick_id;
	unsigned int filterset_uid;
	std::uint64_t bytes_sent;
	std::uint64_t bytes_recv;
	std::uint64_t bytes_sent_prev;
	std::uint64_t bytes_recv_prev;
	unsigned short speed_sent;
	unsigned short speed_recv;
	utimestamp last_activity;

	bool is_proc_active() const
	{
		return flag & 0x0001;
	}
};

struct proc_info2 {
	std::unique_ptr<gstring> exefile;
	std::unique_ptr<gstring> username;
};

typedef std::map<std::string, std::string> drivemap_container;

class proc_list
{
public:
	static const char this_class_name[];
	enum proc_list_mode { PROC_LIST_SHORT_MODE = 0, PROC_LIST_FULL_MODE = 1 };

	proc_list(void);
	~proc_list(void);

	static void detect_drives(drivemap_container& drives);
	static unsigned int GetPidIndex(unsigned int pid) { return pid / 4; };
	static unsigned int GetPidByIndex(unsigned int pidIndex) { return pidIndex * 4; };

	void init(proc_list_mode mode);
	void reset_counters();
	int get_active_proc_size() const;

	void refresh(const procnicknamelist& procnicks, const userlist& users);
	void refresh_speed(unsigned int interval, const utimestamp& current_timestamp);
	bool check_proc_exist(unsigned int pid) const;
	proc_info* get_procptr_by_pid(unsigned int pid) const;

	std::unique_ptr<proc_info[]> proc_ptr;
	std::unique_ptr<proc_info2[]> proc2_ptr;

protected:
	proc_list_mode mode;
	DWORD OpenProcessDesiredAccess;

public:
#ifdef UTM_DEBUG
	void test_print();
	static void test_all();
#endif
};

}

#endif // UTM_WIN

#endif // UTM_PROC_LIST_H