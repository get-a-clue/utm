// proxyserver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <addrip_v4.h>
#include <ubase_test.h>
#include <event_holder.h>
#include <ufs.h>

#include "http_utils.h"
#include "http_headers.h"
#include "punycoder.h"
#include "addrurl.h"
#include "askgoogle.h"
#include "configproxy.h"
#include "html_document.h"
#include "conditem_addrurl.h"
#include "conditem_sitecategory.h"
#include "proxyrule.h"
#include "proxyrule_item.h"
#include "accessrecord.h"
#include "squidrecord.h"
#include "googlerecord.h"
#include "sqcore.h"
#include "proxymsg.h"
#include "proxy-server.hpp"
#include "mimetypes.h"

#include <boost/asio/ip/address.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>

void proxyserverthread();

void test()
{
#ifdef UTM_DEBUG
	try
	{
		utm::gstring mpath = utm::ufs::get_current_directory();
		mpath << "mime.types";

		utm::mimetypes mt;
		mt.readfromfile(mpath.getmb());

		utm::response_http_headers::test_all();
		utm::http_utils::test_all();
		utm::punycoder::test_all();
		utm::addrurl::test_all();
		utm::addrurl_params::test_all();
//		utm::askgoogle::test_all();
		utm::conditem_addrurl::test_all();
		utm::html_document::test_all();

		utm::ubase_test<utm::proxymsg> test_proxymsg;
		test_proxymsg.test_all();

		utm::ubase_test<utm::configproxy> test_configproxy;
		test_configproxy.test_all();

		utm::configproxy::test_loadconfig();
	} 
	catch(utm::test_exception& ex)
	{
		std::cout << ex.what() << std::endl;
		throw;
	}
#endif
}

boost::mutex mtxWait;
boost::condition cndSignal;

int _tmain(int argc, _TCHAR* argv[])
{
	size_t test_count = 0;
	while(1)
	{
		test();
		test_count++;
		std::cout << "Test passed: " << test_count << std::endl;
		break;
	}
/*
	boost::thread proxythread(&proxyserverthread);

	int c = getchar();
	std::cout << "notify_one" << std::endl;
	cndSignal.notify_one();

	Sleep(2000);
*/
	return 0;
}

void LogSqcoreError(const char *err)
{
	std::cout << err << std::endl;
	return;
}

void proxyserverthread()
{
	std::cout << "proxyserverthread" << std::endl;

	utm::configproxy cfg;
	utm::configproxy::test_loadconfig3_fromcode(cfg);

	try 
	{
		utm::sqcore *sq = utm::sqcore::instance();
		sq->report_problem_ptr = &LogSqcoreError;

		ios_deque io_services;
		std::deque<ba::io_service::work> io_service_work;
		
		boost::thread_group thr_grp;
		
		for (int i = 0; i < cfg.max_threads; ++i) 
		{
			io_service_ptr ios(new ba::io_service);
			io_services.push_back(ios);
			io_service_work.push_back(ba::io_service::work(*ios));
			thr_grp.create_thread(boost::bind(&ba::io_service::run, ios));
		}

		utm::server server(io_services, cfg.listen_port);

		if (cfg.logging.size() == 0)
		{
			utm::configproxy::load_default_logfacilities(cfg);
			for (BOOST_AUTO(iter, cfg.logging.begin()); iter != cfg.logging.end(); ++iter)
			{
				iter->enable_file = true;
			}
		}

		server.cfg = std::unique_ptr<utm::configproxy>(&cfg);
		server.init_loggers(true);

		server.is_accept_connections = true;

		// The join_all method blocks your program until all the threads finished.

		boost::mutex::scoped_lock mtxWaitLock(mtxWait);
		cndSignal.wait(mtxWait);
		std::cout << "signal received" << std::endl;

		server.is_accept_connections = false;

		server.close();
		std::cout << "ioservices stop" << std::endl;

		thr_grp.interrupt_all();
		std::cout << "interrupt_all" << std::endl;

		thr_grp.join_all();
		std::cout << "join_all" << std::endl;

		if (server.squidlogger != NULL)
			server.squidlogger->clear();

		server.cfg.release();
	} 
	catch (std::exception&) 
	{
	}

	std::cout << "thread terminated" << std::endl;
}

