// monitor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/asio.hpp>
#include <utm.h>

#include "arp_table.h"
#include "macvendors.h"
#include "monitor_minidump.h"
#include "monitor_total.h"
#include "monitor_pinger.h"
#include "monitor_pinger2.h"
#include "proc_list.h"
#include "port_pid_table.h"
#include "proc_row_list.h"
#include "udpserver_test.h"

#include <ubase_test.h>

int _tmain(int argc, _TCHAR* argv[])
{
#ifdef UTM_DEBUG

	_tsetlocale(LC_ALL, _T(""));
/*
	utm::ioservice_holder iosholder;
	utm::udpserver_test td(iosholder.ios, 7641);
	iosholder.ios.run();
*/

	utm::arp_table::test_all();
	utm::macvendors::test_all();
	utm::monitor_minidump::test_all();

	utm::ubase_test<utm::monitor_minidump> test_monitor_minidump;
	test_monitor_minidump.test_all();

	utm::monitor_total::test_all();

//	utm::monitor_pinger2::test_all();

/*
	while(1)
	{
		utm::monitor_range mr;
		mr.test_fillparams(1);
		utm::monitor_range_list mrl;
		mrl.add_element(mr);

		utm::monitor_total mt;
		mt.add_event = utm::monitor_total::test_print_event;

		mt.init_from_ranges(mrl);
		utm::macvendors mv;
		mv.load_from_file("oui.txt");

		while(1)
		{
			boost::asio::io_service io;
			utm::monitor_pinger2 mp(&io);
			mp.main(mt, mv);
			mp.main(mt, mv);
			mp.main(mt, mv);
			mp.main(mt, mv);
			mt.analyze(mv);
		}

		std::cout << std::endl;
	}
*/

	utm::proc_list::test_all();

	utm::proc_list p;
	p.init(utm::proc_list::PROC_LIST_FULL_MODE);

	utm::procnicknamelist plist;
	utm::userlist ulist;
	p.refresh(plist, ulist);

	utm::port_pid_table tcpports;
	tcpports.load_ports(6, p);

	std::string xml;
	utm::proc_row_list procrowlist;
	procrowlist.create_from_proc_list(p, xml, true);
	procrowlist.xml_save("c:\\tmp\\proc_row_list.xml");

//	p.test_print();

#endif  // UTM_DEBUG

	return 0;
}

