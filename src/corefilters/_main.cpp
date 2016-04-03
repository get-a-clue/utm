#include "stdafx.h"

#ifdef UTM_DEBUG

#include "rule.h"
#include "filter2.h"
#include "hostname.h"
#include "hosttable.h"
#include "users_connected.h"
#include "userlist.h"
#include "urlfilter.h"
#include "urlfilterlist.h"
#include "monitor_range.h"
#include "filterset_data.h"
#include "filterset.h"
#include "filtersetstate.h"
#include "sms_queue.h"
#include "hostresolver.h"
#include "pkt_queue_filterset.h"
#include "trafficreport_hourtick.h"
#include "trafficreport_daytick.h"
#include "trafficreport_filter.h"
#include "trafficreport.h"
#include "pktcollector_e.h"
#include "pktcollector_ex.h"

#include <gstring.h>
#include <ufs.h>
#include <ubase_test.h>

#include "hostresolver.h"

void filtersetstate_test(int testcase_num)
{
	utm::gstring filepath;

	if (testcase_num == 0)
	{
		filepath = utm::ufs::get_current_directory();
		filepath.append("filtersetstate_test.xml");
	}

	if (testcase_num == 1)
	{
		filepath = "c:\\tmp\\filtersetstate_test.xml";
	}

	utm::filtersetstate fss;
	fss.xml_load(filepath);

	TEST_CASE_CHECK(fss.comment, utm::gstring("This file contains the current traffic counters of your filterset. This is not a final report, it is just temporary file. Don't delete it or modify it!"));
	TEST_CASE_CHECK(fss.filtersetfile, utm::gstring("default.tmf"));
	TEST_CASE_CHECK(fss.filterstates.size(), size_t(3));

	utm::utime ureset(2013, 11, 01, 11, 14, 16);

	int i = 0;
	for (auto iter = fss.filterstates.items.begin(); iter != fss.filterstates.items.end(); ++iter, i++)
	{
		utm::filterstate* pfst = dynamic_cast<utm::filterstate *>(iter->get());

		TEST_CASE_CHECK(pfst->lastreset, ureset);

		if (i == 0)
		{
			TEST_CASE_CHECK(pfst->filter_id, unsigned int(1));
			TEST_CASE_CHECK(pfst->bytes_sent, __int64(123456));
			TEST_CASE_CHECK(pfst->bytes_recv, __int64(654321));
			TEST_CASE_CHECK(pfst->filter_name, utm::gstring("ICMP traffic - sample filter"));
		}

		if (i == 1)
		{
			TEST_CASE_CHECK(pfst->filter_id, unsigned int(2));
			TEST_CASE_CHECK(pfst->bytes_sent, __int64(234567));
			TEST_CASE_CHECK(pfst->bytes_recv, __int64(765432));
			TEST_CASE_CHECK(pfst->filter_name, utm::gstring("traffic to www.w3.org - sample filter"));
		}

		if (i == 2)
		{
			TEST_CASE_CHECK(pfst->filter_id, unsigned int(5));
			TEST_CASE_CHECK(pfst->bytes_sent, __int64(5678901));
			TEST_CASE_CHECK(pfst->bytes_recv, __int64(5678902));
			TEST_CASE_CHECK(pfst->filter_name, utm::gstring("All traffic of this computer - sample filter"));
		}
	}


	if (testcase_num == 0)
	{
		fss.xml_save("c:\\tmp\\filtersetstate_test.xml");
	}

	return;
}

void test()
{
	utm::pkt_queue::test_all();
	utm::pkt_queue_filterset::test_all();
//	utm::hostresolver::test_all();
	utm::hostname_ex::test_all();
//	utm::hosttable::test_all();
	utm::filterset_data::test_all();
	utm::urlfilter::test_all();
	utm::trafficreport_hourtick::test_all();
	utm::trafficreport_daytick::test_all();
	utm::trafficreport_filter::test_all();
	utm::trafficreport::test_all();
	utm::pktcollector_e::test_all();
	utm::pktcollector_ex::test_all();

	utm::ubase_test<utm::sms_queue> test_sms_queue;
	test_sms_queue.test_all();

	utm::ubase_test<utm::users_connected> test_users_connected;
	test_users_connected.test_all();

	utm::ubase_test<utm::userlist> test_userlist;
	test_userlist.test_all();

	utm::ubase_test<utm::urlfilterlist> test_urlfilterlist;
	test_urlfilterlist.test_all();

	utm::ubase_test<utm::monitor_range> test_monitorrange;
	test_monitorrange.test_all();

	utm::ubase_test<utm::rule> test_rule;
//	test_rule.test_all();

	utm::ubase_test<utm::filter> test_filter;
	test_filter.test_all();

	utm::ubase_test<utm::filterset> test_filterset;
	test_filterset.test_all();

	utm::filter2::test_match_filter();

	filtersetstate_test(0);
	filtersetstate_test(1);
}

int _tmain(int argc, _TCHAR* argv[])
{
	_tsetlocale(0, _T(""));

	try
	{
		test();
	}
	catch(const std::exception& ex)
	{
	std::cout << ex.what() << std::endl;
		throw ex;
	}

	return 0;
}

#endif
