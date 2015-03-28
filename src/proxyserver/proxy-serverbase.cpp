#include "StdAfx.h"
#include "proxy-serverbase.h"
#include "sqcore.h"

#include <algorithm>

#include <boost/algorithm/string.hpp>

namespace utm {

serverbase::serverbase(void)
{
	is_accept_connections = false;

	accesslogger = NULL;
	squidlogger = NULL;
	googlelogger = NULL;
	eventlogger = NULL;
}

serverbase::~serverbase(void)
{
	if (accesslogger != NULL)
		delete accesslogger;

	if (squidlogger != NULL)
		delete squidlogger;

	if (googlelogger != NULL)
		delete googlelogger;

//	if (eventlogger != NULL)
//		delete eventlogger;
}

void serverbase::init_loggers(bool enable_file)
{
	gstring current_directory = ufs::get_current_directory();

	sqcore *sq = sqcore::instance();
	sq->dbdirectory = current_directory.getmb() + "proxy\\";
	sq->dbprefix = "db_";

	loggingparams_container logging = cfg->logging;

	loggingparams_iterator iter;
	for (iter = logging.begin(); iter != logging.end(); ++iter)
	{
#ifndef DEBUG
		iter->enable_console = false;
#endif
		if (enable_file)
			iter->enable_file = true;

		if (iter->get_directory().empty())
		{
			iter->set_directory(current_directory);
		}

		if (iter->facility == "squid")
		{
			squidlogger = new utm::event_holder<utm::squidrecord>();
			squidlogger->copy_params_from(*iter);
		}

		if (iter->facility == "google")
		{
			googlelogger = new utm::event_holder<utm::googlerecord>();
			googlelogger->copy_params_from(*iter);
		}

		if (iter->facility == "access")
		{
			accesslogger = new utm::event_holder<utm::accessrecord>();
			accesslogger->copy_params_from(*iter);
		}
	}
}

bool serverbase::process_request(proxyrequest_params* pp)
{
	pp->whatsite = boost::bind(&utm::serverbase::whatsite, this, _1);
	bool retval = false;
	
	try
	{
		retval = cfg->process_request(pp);
	}
	catch(const utm::askgoogle_exception& e)
	{
		pp->askexc = std::unique_ptr<askgoogle_exception>(new askgoogle_exception(e));

		if (googlelogger != NULL)
		{
			googlerecord gr;
			gr.id = 0;
			gr.type = event_type::ET_ERROR;
			gr.time.now();
			gr.result_code = GOOGLE_ERROR;
			gr.site = pp->request->url.second_level_domain;
			gr.msg.assign(e.what());
			googlelogger->add_item(gr, false);
		}
	}
	catch(std::exception&)
	{
	}

	if ((accesslogger != NULL) && (!pp->proxyrule_nolog_access))
	{
		accessrecord ar;

		ar.id = 0;
		ar.type = event_type::ET_INFORMATION;
		ar.time.now();
		ar.request_phase = pp->request_phase == proxyrequest_params::phase_replybody ? "reply" : "reqst";
		ar.request_url = pp->request->url.original_addrurl;
		ar.client_ip = pp->request->client_addr;
		ar.resume = "none_";

		if (pp->proxyruleaction == (int)proxyrule::action_allow) ar.resume = "allow_";
		if (pp->proxyruleaction == (int)proxyrule::action_deny) ar.resume = "deny_";

		if (pp->proxyrulename4log != NULL)
			ar.resume.append(pp->proxyrulename4log);

		if (pp->request_phase == proxyrequest_params::phase_requestheader)
		{
			accesslogger->add_item(ar);
		}
	}

	return retval;
}

int serverbase::whatsite(const request_http_headers* req)
{
	if (req == NULL)
		return GOOGLE_SEX;

	return whatsiteref(req);
}

int serverbase::whatsiteref(const request_http_headers* req)
{
	int retval = GOOGLE_SEX;

	std::string::size_type css_pos = req->url.path.rfind(".css");
	if (css_pos != std::string::npos)
		return GOOGLE_OK;

	if (req->url.second_level_host == "google")
		return GOOGLE_OK;

	if (req->url.second_level_host == "microsoft")
		return GOOGLE_OK;

	if (req->url.second_level_host == "yandex")
		return GOOGLE_OK;

	std::string domain = req->url.second_level_domain;

	domains_list_container::iterator iter;
	iter = std::find(cfg->third_level_domains.begin(), cfg->third_level_domains.end(), domain);
	if (iter != cfg->third_level_domains.end())
	{
		domain = req->url.third_level_domain;
	}

	int gcstatus = gc.check_site(domain);
	if ((gcstatus != GOOGLECACHE_NOT_FOUND) && (gcstatus != GOOGLECACHE_IS_PENDIND))
		return gcstatus;

	if (gcstatus == GOOGLECACHE_IS_PENDIND)
	{
		std::shared_ptr<signalcatcher> ssc(new signalcatcher());
		gc.add_callback(domain, ssc);

		boost::mutex mtx_wait; 
		boost::mutex::scoped_lock mtx_wait_lock(mtx_wait);

//		scc->signal.wait(mtx_wait); // infinite wait
		if (!ssc->signal.timed_wait(mtx_wait, boost::posix_time::milliseconds(1000)))
		{
			// timeout
			return 0;
		}

		gcstatus = gc.check_site(domain);
		return gcstatus;
	}

	bool has_pending_query = false;

	try
	{
		sqcore *sq = sqcore::instance();

		int sqstatus = sq->select_site(domain.c_str());

		if (sqstatus == SQCORE_NOTFOUND)
		{
			gc.add_pending_query(domain);
			has_pending_query = true;

			retval = askgoogle::analyze_site(domain.c_str(), req, cfg->get_askgoogle_site());

			if (googlelogger != NULL)
			{
				googlerecord gr;
				gr.id = 0;
				gr.type = event_type::ET_INFORMATION;
				gr.time.now();
				gr.result_code = retval;
				gr.site = domain;
//				gr.msg = askres.error_message;
				googlelogger->add_item(gr, false);
			}

			if (retval != GOOGLE_ERROR)
			{
				sq->update_site(domain.c_str(), retval);
				gc.set_completed_query(domain, retval);
				has_pending_query = false;
			}
			else
			{
				retval = GOOGLE_SEX;
			}
		}
		else
		{
			retval = sqstatus;
		}
	}
	catch(const askgoogle_exception& e)
	{
		if (has_pending_query)
		{
			gc.remove_pending_query(domain);
		}
		throw e;
	}

	if (has_pending_query)
	{
		gc.remove_pending_query(domain);
	}

	return retval;
}

void serverbase::process_webserver_request(const utm::request_http_headers& request, utm::response_http_headers& response)
{


}

}
