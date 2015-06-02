#include "stdafx.h"


#include <iostream>
#include <sstream>
#include <fstream>

#include "fastformat.h"
#include "addrpair_v4.h"
#include "addrip_v4.h"
#include "addrtable_v4.h"
#include "addrmac.h"
#include "addrpair.h"
#include "blob.h"
#include "gstring.h"
#include "adapterinfo.h"
#include "adapterlist.h"
#include "addrtable.h"
#include "enumhelper.h"
#include "md5.h"
#include "base64.h"
#include "crc32.h"
#include "idset.h"
#include "ubaselist.h"
#include "ubase_test.h"
#include "event_test.h"
#include "event_msg_queue.h"
#include "event_pkt.h"
#include "event_pkt_queue.h"
#include "loggingparams.h"
#include "stringtools.h"
#include "ufs.h"
#include "utime.h"
#include "uhex.h"
#include "simple_queue.h"
#include "SysData.h"

#ifdef UTM_DEBUG

const utm::gchar* __stdcall makeloc(const utm::gchar* src)
{
	if (_tcscmp(src, _T("message 3")) == 0)
		return _T("message 3");

	if (_tcscmp(src, _T("message 4")) == 0)
		return _T("message 4");

	return _T("message");
}

#endif

int _tmain(int argc, _TCHAR* argv[])
{
	_tsetlocale(0, _T(""));

	utm::CSysNet si;
	si.RefreshLocalAddresses();
	si.RefreshLocalAddresses();

#ifdef UTM_DEBUG

	utm::stringtools::localize = &makeloc;

	utm::utime prev(true);
	int i = 0;
	while(1)
	{
		try
		{
			utm::fastformat::test_all();
			utm::md5::test_all();
			utm::base64::test_all();
			utm::crc32::test_all();
			utm::utime::test_all();
			utm::utimestamp::test_all();
			utm::blob::test_all();

			utm::addrip_v4::test_all();
			utm::addrmac::test_all();
			utm::addrpair<utm::addrip_v4>::test_all();
			utm::addrtable_v4::test_all();
			utm::ubaselist_test::test_all();

			utm::adapterinfo::test_all();
			utm::ubase_test<utm::adapterinfo> test_adapterinfo;
			test_adapterinfo.test_all();

			utm::adapterlist::test_all();
			utm::ubase_test<utm::adapterlist> test_adapterlist;
			test_adapterlist.test_all();

			utm::event_test::test_all();
			utm::event_pkt::test_all();

			utm::ufs::test_all();

			utm::ubase_test<utm::event_msg> test_eventmsg;
			test_eventmsg.test_all();

			utm::ubase_test<utm::event_msg_queue> test_eventqueue;
			test_eventqueue.test_all();

			utm::event_pkt_queue::test_all();

			utm::event_msg_queue::test_parse_diff_dump_flags();

			utm::ubase_test<utm::loggingparams> test_loggingparams;
			test_loggingparams.test_all();

			utm::idgeneric::test_all();
			utm::idset::test_all();
			utm::idlist::test_all();
			utm::enumhelper::test_all();
			utm::stringtools::test_all();
			utm::uhex::test_all();

			utm::simple_queue_test::test_all();
			utm::CSysData::test_all();
		}
		catch(const std::string& ex)
		{
			std::cout << ex << std::endl;
			throw ex;
		}
		catch(const std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
			throw ex;
		}

 		utm::utime u1(true);
		time_t diff = u1.to_time_t() - prev.to_time_t();

		if (diff != 0)
		{
			std::cout << u1.to_string(utm::utime::format_log) << "   " << i << std::endl;
			prev = u1;
		}

		i++;
		break;
	};

	utm::addrtable<utm::addrip_v4> at;
#endif

	return 0;
}