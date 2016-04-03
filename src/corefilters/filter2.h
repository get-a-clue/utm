#ifndef UTM_FILTER2_H
#define UTM_FILTER2_H

#pragma once
#include <utm.h>

#include "filter_.h"

#include "filter_extra.h"
#include "pktcollector_e.h"
#include "pktcollector_ex.h"
#include <addrtablemap_v4.h>
#include <utime.h>
#include <utimestamp.h>

#define LOGPKT_PRIVILEGED 1
#define LOGPKT_WRITEMAC 2
#define LOGPKT_WRITETOS 4
#define LOGPKT_WRITEFILTERNAME 8

//
// Ќабор битовых полей дл€
// m_nMasterOptions:
//

#define MASTER_CHECK_ENABLE 1 	// битовое поле (бит 0)
			      	// ќпредел€ет, включена ли опци€ динамического управлени€ скоростью
#define MASTER_CHECK_SENT 2	// битовое поле (бит 1), 
				// определ€ющее какой счетчик (sent или recv) брать в расчет при проверки услови€
#define MASTER_CHECK_LESS 4	// битовое поле (бит 2),
				// определ€ющее какое условие (меньше или больше) провер€ть в мастер фильтре
#define MASTER_CHECK_SPEED 8	// битовое поле (бит 3),
				// определ€ющее какую скорость (текущую или среднюю за последние 10 сек.) провер€ть в мастер фильтре

namespace utm {

class filter2 :	public filter
{
public:
    static const char this_class_name[];

public:
	filter2(void);
	virtual ~filter2(void);

	const char *get_this_class_name() const { return this_class_name; };

	void copy_filter_counters(const filter2& f);
	void reset_filter_counters(bool reset_history);
	bool reset_on_schedule(const standard_timeset& stimeset, bool reset_history);
	utimestamp last_reset_ts;
	pktcollector_e pcollector;
	pktcollector_ex hcollector;

	void set_id(unsigned int id) { m_id = id; };
	unsigned int get_id() const { return m_id; };
	const gstring& get_name() const { return m_szFilterName; };

	bool is_traffic_limit_exceeded(int mbytes = 1048576, bool check_warning_level = false) const;
	bool is_traffic_limitwarn_exceeded(int mbytes = 1048576) const;
	int get_actual_speed() const;

	void match_filter(const match_filter_input& data, match_filter_result& result, bool clear_result_before);
	void match_filter(const match_filter_input& data, match_filter_result& result);

	static void test_match_filter();
};

}

#endif // UTM_FILTER2_H