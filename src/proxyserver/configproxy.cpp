#include "stdafx.h"
#include "configproxy.h"

#include <boost/algorithm/string.hpp> 

namespace utm {

const char configproxy::this_class_name[] = "configproxy";

configproxy::configproxy(void)
{
	clear();
}

configproxy::configproxy(const configproxy& r)
{
	operator=(r);
}

configproxy::~configproxy(void)
{
}

configproxy& configproxy::operator=(const configproxy &rhs)
{
	enabled = rhs.enabled;
	webserver_mode = rhs.webserver_mode;
	max_threads = rhs.max_threads;
	listen_port = rhs.listen_port;
	dbmask = rhs.dbmask;
	dbprefix = rhs.dbprefix;
	denybody = rhs.denybody;
	denycode = rhs.denycode;
	webserver_hostname = rhs.webserver_hostname;
	third_level_domains = rhs.third_level_domains;
	askgoogle_site = rhs.askgoogle_site;
	askgoogle_lang = rhs.askgoogle_lang;
	askgoogle_useragent = rhs.askgoogle_useragent;

	conditems.clear();
	conditem_container::const_iterator conditer;
	for (conditer = rhs.conditems.begin(); conditer != rhs.conditems.end(); ++conditer)
	{
		conditem *cdi = NULL;
		if (conditer->get() != NULL)
		{
			cdi = conditem::create(*conditer->get());
			conditems.push_back(std::shared_ptr<conditem>(cdi));
		}
	}

	rules.clear();
	proxyrule_container::const_iterator ruleiter;
	for (ruleiter = rhs.rules.begin(); ruleiter != rhs.rules.end(); ++ruleiter)
	{
		rule_add(*ruleiter);
	}

	logging.clear();
	loggingparams_container::const_iterator logiter;
	for (logiter = rhs.logging.begin(); logiter != rhs.logging.end(); ++logiter)
	{
		logging.push_back(*logiter);
	}

	messages_eng = rhs.messages_eng;
	messages_rus = rhs.messages_rus;

	return *this;
}

bool configproxy::operator==(const configproxy& rhs) const
{
	if (enabled != rhs.enabled)
		return false;

	if (webserver_mode != rhs.webserver_mode)
		return false;

	if (max_threads != rhs.max_threads)
		return false;

	if (listen_port != rhs.listen_port)
		return false;

	if (dbmask != rhs.dbmask)
		return false;

	if (dbprefix != rhs.dbprefix)
		return false;

	if (denybody != rhs.denybody)
		return false;

	if (denycode != rhs.denycode)
		return false;

	if (webserver_hostname != rhs.webserver_hostname)
		return false;

	if (third_level_domains.size() != rhs.third_level_domains.size())
		return false;

	domains_list_constiterator iter = third_level_domains.begin();
	domains_list_constiterator iter2 = rhs.third_level_domains.begin();
	for (; iter != third_level_domains.end(); ++iter, ++iter2)
	{
		if ((*iter) != (*iter2))
			return false;
	}

	if (conditems.size() != rhs.conditems.size())
		return false;

	conditem_container::const_iterator conditer;
	conditem_container::const_iterator conditer2 = rhs.conditems.begin();
	for (conditer = conditems.begin(); conditer != conditems.end(); ++conditer, ++conditer2)
	{
		if (!((*conditer->get()) == (*conditer2->get())))
			return false;
	}

	if (rules.size() != rhs.rules.size())
		return false;

	proxyrule_container::const_iterator ruleiter;
	proxyrule_container::const_iterator ruleiter2 = rhs.rules.begin();
	for (ruleiter = rules.begin(); ruleiter != rules.end(); ++ruleiter, ++ruleiter2)
	{
		if (!((*ruleiter) == (*ruleiter2)))
			return false;
	}

	if (logging.size() != rhs.logging.size())
		return false;

	loggingparams_container::const_iterator logiter;
	loggingparams_container::const_iterator logiter2 = rhs.logging.begin();
	for (logiter = logging.begin(); logiter != logging.end(); ++logiter, ++logiter2)
	{
		if (!((*logiter) == (*logiter2)))
			return false;
	}

	if (!(messages_eng == rhs.messages_eng))
		return false;

	if (!(messages_rus == rhs.messages_rus))
		return false;

	return true;
}

void configproxy::clear()
{
	enabled = false;
	webserver_mode = false;
	max_threads = 16;
	listen_port = 3128;
	dbmask = 4;
	dbprefix = "db";
	dbprefix.clear();
	webserver_hostname.clear();
	third_level_domains.clear();
	askgoogle_site.clear();
	askgoogle_lang.clear();
	askgoogle_useragent.clear();
}

const char *configproxy::get_askgoogle_site()
{
	if (askgoogle_site.empty())
		return DEFAULT_ASKGOOGLE_SITE;

	return askgoogle_site.c_str();
}

const char *configproxy::get_askgoogle_lang()
{
	if (askgoogle_lang.empty())
		return DEFAULT_ASKGOOGLE_lang;

	return askgoogle_lang.c_str();
}

const char *configproxy::get_askgoogle_useragent()
{
	if (askgoogle_useragent.empty())
		return DEFAULT_ASKGOOGLE_USERAGENT;

	return askgoogle_useragent.c_str();
}

void configproxy::xml_create()
{
	xml_append_root(CONFIGPROXY_XMLTAG_PROXY);
	xml_append_node(CONFIGPROXY_XMLTAG_ENABLED, enabled);
	xml_append_node(CONFIGPROXY_XMLTAG_WEBSERVERMODE, webserver_mode);
	xml_append_node(CONFIGPROXY_XMLTAG_MAXTHREADS, max_threads);
	xml_append_node(CONFIGPROXY_XMLTAG_PORT, listen_port);
	xml_append_node(CONFIGPROXY_XMLTAG_DBMASK, dbmask);
	xml_append_node(CONFIGPROXY_XMLTAG_DBPREFIX, dbprefix);
	xml_append_node(CONFIGPROXY_XMLTAG_DENYBODY, denybody);
	xml_append_node(CONFIGPROXY_XMLTAG_DENYCODE, denycode);
	xml_append_node(CONFIGPROXY_XMLTAG_WEBSERVERHOSTNAME, webserver_hostname);

	std::string domains;
	stringtools::join(third_level_domains, ' ', domains);
	xml_append_node(CONFIGPROXY_XMLTAG_THIRDLEVELDOMAINS, domains);

	xml_append_node(CONFIGPROXY_XMLTAG_ASKSITE, askgoogle_site);
	xml_append_node(CONFIGPROXY_XMLTAG_ASKLANG, askgoogle_lang);
	xml_append_node(CONFIGPROXY_XMLTAG_ASKUSERAGENT, askgoogle_useragent);

	conditem_iterator iter;
	for (iter = conditems.begin(); iter != conditems.end(); ++iter)
	{
		iter->get()->xml_create();
		xml_append_child(iter->get()->get_first_child());
	}

	proxyrule_iterator iter2;
	for (iter2 = rules.begin(); iter2 != rules.end(); ++iter2)
	{
		iter2->xml_create();
		xml_append_child(iter2->get_first_child());
	}

	loggingparams_iterator iter3;
	for (iter3 = logging.begin(); iter3 != logging.end(); ++iter3)
	{
		iter3->xml_create();
		xml_append_child(iter3->get_first_child());
	}

	{
		messages_eng.xml_create();
		xml_append_child(messages_eng.get_first_child());
	}

	{
		messages_rus.xml_create();
		xml_append_child(messages_rus.get_first_child());
	}
}

void configproxy::xml_catch_value(const char *keyname, const char *keyvalue)
{
	unsigned int tmp;
	std::string str;

	while(1)
	{
		if (xml_check_value(keyname, CONFIGPROXY_XMLTAG_ENABLED, keyvalue, enabled)) break;
		if (xml_check_value(keyname, CONFIGPROXY_XMLTAG_WEBSERVERMODE, keyvalue, webserver_mode)) break;

		if (xml_check_value(keyname, CONFIGPROXY_XMLTAG_MAXTHREADS, keyvalue, tmp))
		{
			max_threads = static_cast<unsigned short>(tmp);
			break;
		};

		if (xml_check_value(keyname, CONFIGPROXY_XMLTAG_PORT, keyvalue, tmp))
		{
			listen_port = static_cast<unsigned short>(tmp);
			break;
		};

		if (xml_check_value(keyname, CONFIGPROXY_XMLTAG_DBMASK, keyvalue, tmp))
		{
			dbmask = static_cast<unsigned short>(tmp);
			break;
		};

		if (xml_check_value(keyname, CONFIGPROXY_XMLTAG_DBPREFIX, keyvalue, dbprefix)) break;
		if (xml_check_value(keyname, CONFIGPROXY_XMLTAG_DENYBODY, keyvalue, denybody)) break;
		if (xml_check_value(keyname, CONFIGPROXY_XMLTAG_DENYCODE, keyvalue, denycode)) break;
		if (xml_check_value(keyname, CONFIGPROXY_XMLTAG_WEBSERVERHOSTNAME, keyvalue, webserver_hostname)) break;

		if (xml_check_value(keyname, CONFIGPROXY_XMLTAG_THIRDLEVELDOMAINS, keyvalue, str))
		{
			std::vector<std::string> domains;
			stringtools::split(domains, str, ' ', split::no_empties);
			for (std::vector<std::string>::iterator iter = domains.begin(); iter != domains.end(); ++iter)
			{
				third_level_domains.push_back(*iter);
			}

			break;
		};

		if (xml_check_value(keyname, CONFIGPROXY_XMLTAG_ASKSITE, keyvalue, askgoogle_site)) break;
		if (xml_check_value(keyname, CONFIGPROXY_XMLTAG_ASKLANG, keyvalue, askgoogle_lang)) break;
		if (xml_check_value(keyname, CONFIGPROXY_XMLTAG_ASKUSERAGENT, keyvalue, askgoogle_useragent)) break;

		break;
	}
}

ubase* configproxy::xml_catch_subnode(const char *keyname)
{
	if (strcmp(keyname, CONFIGPROXY_XMLTAG_CONDITEM) == 0)
	{
		conditem_fake.clear();
		ubase *u = (ubase *)&conditem_fake;
		return u;
	}

	if (strcmp(keyname, PROXYRULE_XMLTAG) == 0)
	{
		return insert_element_and_get_lastptr(rules);
	}

	if (strcmp(keyname, LOGGINGPARAMS_XMLTAG_PARAMS) == 0)
	{
		return insert_element_and_get_lastptr(logging);
	}

	if (strcmp(keyname, PROXYMSG_XMLTAG_ENG) == 0)
	{
		messages_eng.clear();
		ubase *u = (ubase *)&messages_eng;
		return u;
	}

	if (strcmp(keyname, PROXYMSG_XMLTAG_RUS) == 0)
	{
		messages_rus.clear();
		ubase *u = (ubase *)&messages_rus;
		return u;
	}

	return NULL;
}

void configproxy::xml_catch_subnode_finished(const char *keyname)
{
	if (strcmp(keyname, CONFIGPROXY_XMLTAG_CONDITEM) == 0)
	{
		std::string s(conditem_fake.condvalue);
		boost::algorithm::trim(s);

		conditem *ci = conditem::create(conditem_fake);
		if (ci != NULL)
		{
			ci->parse(s);
			conditems.push_back(std::shared_ptr<conditem>(ci));
		}
	}

	if (strcmp(keyname, PROXYRULE_XMLTAG) == 0)
	{
		proxyrule_container::reverse_iterator riter = rules.rbegin(); 
		if (riter != rules.rend())
		{
			proxyrule_container::iterator iter = --riter.base();

			proxyrule_items_iterator pri_iter;
			for (pri_iter = iter->items.begin(); pri_iter != iter->items.end(); ++pri_iter)
			{
				if ((!pri_iter->condname.empty()) && (pri_iter->ci.get() == NULL))
				{
					conditem_iterator cond_iter;
					for (cond_iter = conditems.begin(); cond_iter != conditems.end(); ++cond_iter)
					{
						conditem* cdi = cond_iter->get();
						if (cdi != NULL)
						{
							if (cdi->condname == pri_iter->condname)
							{
								pri_iter->ci = *cond_iter;
								break;
							}
						}
					}
				}
			}
		}
	}
}

void configproxy::get_denyresponse(proxymsg::msgtypes msgtype, const char *browserlang, const char *requestedsite, const char *proxyrulename, std::string& denycontent, std::string& denyresponsecode)
{
	int http_code = 0;

	// kazak TODO
	int lang = proxymsg::parse_browser_language(browserlang);

	if (lang == (int)proxymsg::lang_eng)
	{
		messages_eng.get_message(msgtype, requestedsite, proxyrulename, denycontent, http_code);
	}
	else
	{
		messages_rus.get_message(msgtype, requestedsite, proxyrulename, denycontent, http_code);
	}

	std::ostringstream oss;
	oss << http_code;
	denyresponsecode = oss.str();
}

bool configproxy::process_request(proxyrequest_params *params)
{
	int retval = proxyrule::action_none;

	int i = 1;
	for (proxyrule_iterator iter = rules.begin(); iter != rules.end(); ++iter)
	{
		params->proxyrulename4log = iter->get_rulename4log();
		params->proxyrulename4web = iter->get_rulename4web();
		params->proxyrule_nolog_access = iter->nolog_access;
		params->proxyruleaction = (int)iter->action;

		retval = iter->process_request(params);
		if (retval != proxyrule::action_none)
		{
			// to prevent dening domains like "championat.net" that 
			// referenced from "championat.com"

			if (retval == proxyrule::action_deny)
			{
				if ((params->request != NULL) && (!params->request->referer.original_addrurl.empty()))
				{
					if (params->request->url.second_level_host == params->request->referer.second_level_host)
					{
						if (params->request->url.second_level_domain != params->request->referer.second_level_domain)
						{
							retval = proxyrule::action_none;
							continue;
						}
					}
				}
			}

			break;
		}

		i++;
	}

	if (retval == proxyrule::action_deny)
		return false;

	//
	// actions listed below:
	// proxyrule::action_none
	// proxyrule::action_allow
	//
	// will return "true" (i.e. will pass http request)

	return true;
}

void configproxy::rule_get(size_t index, proxyrule& prule)
{
	size_t k = 0;

	utm::proxyrule_iterator iter;
	for (iter = rules.begin(); iter != rules.end(); ++iter)
	{
		if (index == k)
		{
			prule = *iter;
			break;
		}
		k++;
	}

	if (index != k)
	{
		std::ostringstream oss;
		oss << "The rule (index=" << index << ") is not found.";
		throw std::exception(oss.str().c_str());
	}
}

void configproxy::rule_add(const proxyrule& prule, size_t insert_pos)
{
	proxyrule rule;
	rule.copy_properties(prule);
	
	proxyrule_items_container::const_iterator pri_iter;
	for (pri_iter = prule.items.begin(); pri_iter != prule.items.end(); ++pri_iter)
	{
		if (pri_iter->condname.empty())
		{
			throw std::exception("New proxy rule must have non-empty conditems name");
		}

		proxyrule_item pri;
		pri.copy_properties(*pri_iter);

		bool found = false;

		conditem_iterator conditer;
		for (conditer = conditems.begin(); conditer != conditems.end(); ++conditer)
		{
			conditem* cdi = conditer->get();
			if (cdi != NULL)
			{
				if (cdi->condname == pri_iter->condname)
				{
					pri.ci = *conditer;
					found = true;
					break;
				}
			}
		}

		if (!found)
		{
			throw std::exception("The corresponding conditem was not found");
		}

		rule.items.push_back(pri);
	}

	if (rules.size() < insert_pos)
		rules.push_back(rule);
	else
	{
		proxyrule_iterator rulepos = rules.begin();
		while (insert_pos > 0)
		{
			++rulepos;
			insert_pos--;
		}

		rules.insert(rulepos, rule);
	}
}

void configproxy::rule_replace(const proxyrule& prule, size_t index)
{
	rule_add(prule, index);
	rule_delete(index + 1);
}

void configproxy::rule_delete(size_t index)
{
	size_t k = 0;

	utm::proxyrule_iterator iter;
	for (iter = rules.begin(); iter != rules.end(); ++iter)
	{
		if (index == k)
		{
			rules.erase(iter);
			break;
		}
		k++;
	}

	purge_conditems();
}

void configproxy::rule_moveup(size_t index)
{
	size_t rulescount = rules.size();

	if ((index == 0) || (rulescount < 2) || (index >= rulescount))
	{
		std::ostringstream oss;
		oss << "The rule (index=" << index << ") cannot be moved up.";
		throw std::exception(oss.str().c_str());
	}

	index--;
	proxyrule_iterator iter = rules.begin(); ++iter;
	proxyrule_iterator iter2 = rules.begin();
	for(; iter != rules.end(); ++iter, ++iter2, index--)
	{
		if (index == 0)
			break;
	}

	std::swap(*iter, *iter2);
}

void configproxy::rule_movedown(size_t index)
{
	size_t rulescount = rules.size();

	if ((rulescount < 2) || (index >= (rulescount - 1)))
	{
		std::ostringstream oss;
		oss << "The rule (index=" << index << ") cannot be moved down.";
		throw std::exception(oss.str().c_str());
	}

	proxyrule_iterator iter = rules.begin();
	proxyrule_iterator iter2 = rules.begin(); ++iter2;
	for(; iter != rules.end(); ++iter, ++iter2, index--)
	{
		if (index == 0)
			break;
	}

	std::swap(*iter, *iter2);
}


// Delete unused conditems from configproxy
void configproxy::purge_conditems()
{
	conditem_iterator cond;
	for (cond = conditems.begin(); cond != conditems.end(); )
	{
		conditem* ci = cond->get();

		bool found = false;

		utm::proxyrule_iterator iter;
		for (iter = rules.begin(); iter != rules.end(); ++iter)
		{
			proxyrule_items_iterator pri_iter;
			for (pri_iter = iter->items.begin(); pri_iter != iter->items.end(); ++pri_iter)
			{
				if (pri_iter->ci.get() == ci)
				{
					found = true;
					break;
				}
			}

			if (found)
				break;
		}

		if (!found)
			cond = conditems.erase(cond);
		else
			++cond;
	}
}

void configproxy::load_default_logfacilities(configproxy& cfg)
{
	const char* fac[] = { "access", "squid", "google" };

	for (int i = 0; i < 3; i++)
	{
		bool found = false;
		loggingparams_iterator iter = cfg.logging.begin();
		for (; iter != cfg.logging.end(); ++iter)
		{
			if (strcmp(iter->facility.c_str(), fac[i]) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			loggingparams p;
			p.facility = fac[i];

			std::ostringstream oss; 
			oss << "Logs";
#ifdef UTM_WIN
			oss << "\\";
#else
			oss << "/";
#endif
			oss << fac[i] << "_%Y_%m.txt";
			p.set_filename(oss.str());

			p.set_maxlogsize(10);
			p.set_maxlogrotate(8);

			cfg.logging.push_back(p);
		}
	}
}

void configproxy::load_default_thirdleveldomains(configproxy& cfg)
{
	cfg.third_level_domains.clear();

	const char* doms = "co.jp co.kr com.ua asn.au com.au net.au id.au org.au co.uk ltd.uk me.uk mod.uk net.uk nic.uk org.uk";
	utm::stringtools::split(cfg.third_level_domains, doms, ' ', utm::split::no_empties);
}

#ifdef UTM_DEBUG
void configproxy::test_fillparams(int test_num)
{
	clear();

	enabled = true;
	max_threads = 4;
	listen_port = 3000;
	dbmask = 2;
	dbprefix = "tmeterproxy";
	load_default_thirdleveldomains(*this);

	for (int j = 0; j < 10; j++)
	{
		std::ostringstream oss;
		oss << j;

		conditem_addrip *p = new conditem_addrip();
		p->condname = "mytest conditem_addrip ";
		p->condname.append(oss.str());

		p->condvalue = "192.168.0.";
		p->condvalue.append(oss.str());
		p->parse(p->condvalue);

		conditems.push_back(std::unique_ptr<conditem>(p));

		conditem_addrurl *p1 = new conditem_addrurl(conditem_addrurl::urlpart_host, p->condvalue.c_str());
		p1->condname = "mytest condition_addrurl ";
		p1->condname.append(oss.str());

		conditems.push_back(std::unique_ptr<conditem>(p1));
	}

	proxyrule pr;
	pr.rulename = "The best rule!";

	proxyrule_item pi1;
	pi1.invert = false;
	pi1.condname = "mytest conditem_addrip 0";
	pr.items.push_back(pi1);

	proxyrule_item pi2;
	pi2.invert = true;
	pi2.condname = "mytest condition_addrurl 0";
	pr.items.push_back(pi2);

	rule_add(pr);

	loggingparams lp1;
	lp1.facility = "squid";
	lp1.enable_file = true;

	logging.push_back(lp1);

	loggingparams lp2;
	lp2.facility = "acl";
	lp2.enable_console = true;

	logging.push_back(lp2);

}

void configproxy::test_loadconfig()
{
	test_report tr(this_class_name);

	for (int i = 0; i < 2; i++)
	{
		for (int r = 0; r < 2; r++)
		{
			configproxy cfg;

			if (i == 0)
				test_loadconfig1_fromcode(cfg, r == 0 ? false : true);

			if (i == 1)
				test_loadconfig2_fromcode(cfg);

			std::string xml;
			cfg.xml_create();
			cfg.xml_get_string(xml);

			configproxy cfg2;
			cfg2.xml_parse(xml.c_str());

			if (!(cfg == cfg2))
				throw test_exception(this_class_name, "test_loadconfig", i);

			configproxy cfg3(cfg);
			size_t ci_size_total = cfg.conditems.size();
			size_t ci_size_rule0 = cfg.rules.begin()->items.size();
			
			cfg3.rule_delete(0);

			if (ci_size_total == 0 || ci_size_rule0 == 0 || ((ci_size_total - ci_size_rule0) != cfg3.conditems.size()))
				throw test_exception(this_class_name, "test_loadconfig", i);
		}
	}
}
#endif

void configproxy::test_loadconfig1_fromcode(configproxy& cfg, bool reverse_order)
{
	size_t insert_pos = reverse_order ? 0 : INT_MAX;

	cfg.clear();
	cfg.enabled = true;
	load_default_thirdleveldomains(cfg);

	proxyrule pr1, pr2, pr3, pr4;

	{
		// ProxyRule 1

		pr1.action = proxyrule::action_deny;
		pr1.rulename = "sexrule";

		std::shared_ptr<conditem_addrurl> ci0 = std::shared_ptr<conditem_addrurl>(new conditem_addrurl(conditem_addrurl::urlpart_host, "vtraxe.com vtrah.net"));
		ci0->condname = "sex_excludes";
		cfg.conditems.push_back(ci0);

		std::shared_ptr<conditem_sitecategory> ci1 = std::shared_ptr<conditem_sitecategory>(new conditem_sitecategory());
		ci1->set_conditem_sitecategory(conditem_sitecategory::category_googlesex);
		ci1->condname = "sex";
		cfg.conditems.push_back(ci1);

		proxyrule_item pri0("sex_excludes", true, ci0);
		pr1.items.push_back(pri0);

		proxyrule_item pri1("sex", false, ci1);
		pr1.items.push_back(pri1);

//		pr1.enabled = false;
		cfg.rule_add(pr1, insert_pos);
	}

	{
		// ProxyRule 2

		pr2.action = proxyrule::action_allow;
		pr2.rulename = "allowrule";

		std::shared_ptr<conditem_sitecategory> ci2 = std::shared_ptr<conditem_sitecategory>(new conditem_sitecategory());
		ci2->set_conditem_sitecategory(conditem_sitecategory::category_all);
		ci2->condname = "allsites";
		cfg.conditems.push_back(ci2);

		proxyrule_item pri2("allsites", false, ci2);
		pr2.items.push_back(pri2);

		cfg.rule_add(pr2, insert_pos);
	}

	{
		// ProxyRule 3

		addrip_v4 localhost("127.0.0.1");

		pr3.action = proxyrule::action_allow;
		pr3.rulename = "Welcome, localhost!";

		std::shared_ptr<conditem_addrip> ci3 = std::shared_ptr<conditem_addrip>(new conditem_addrip(conditem_addrip::addr_single, localhost, localhost));
		ci3->condname = "localhost";
		cfg.conditems.push_back(ci3);

		proxyrule_item pri3("localhost", false, ci3);
		pr3.items.push_back(pri3);

		cfg.rule_add(pr3, insert_pos);
	}

	{
		// ProxyRule 4

		addrip_v4 r1("192.168.0.0");
		addrip_v4 r2("192.168.255.255");
		addrip_v4 u1("10.0.0.0");
		addrip_v4 u2("10.255.255.255");
		addrip_v4 q1("172.30.0.0");
		addrip_v4 q2("255.255.0.0");

		pr4.action = proxyrule::action_deny;
		pr4.rulename = "Go away, globalnet";

		std::shared_ptr<conditem_addrip> ci4 = std::shared_ptr<conditem_addrip>(new conditem_addrip(conditem_addrip::addr_range, u1, u2));
		ci4->condname = "private net0";
		cfg.conditems.push_back(ci4);

		std::shared_ptr<conditem_addrip> ci5 = std::shared_ptr<conditem_addrip>(new conditem_addrip(conditem_addrip::addr_network, q1, q2));
		ci5->condname = "private net1";
		cfg.conditems.push_back(ci5);

		std::shared_ptr<conditem_addrip> ci6 = std::shared_ptr<conditem_addrip>(new conditem_addrip(conditem_addrip::addr_range, r1, r2));
		ci6->condname = "private net2";
		cfg.conditems.push_back(ci6);

		proxyrule_item pri4("private net0", true, ci4);
		pr4.items.push_back(pri4);

		proxyrule_item pri5("private net1", true, ci5);
		pr4.items.push_back(pri5);

		proxyrule_item pri6("private net2", true, ci6);
		pr4.items.push_back(pri6);

		cfg.rule_add(pr4, insert_pos);
	}

	{
		configproxy cfg2(cfg);

		int PROXYRULES_TOTAL = 4;
		if (cfg2.rules.size() != PROXYRULES_TOTAL)
			throw test_exception(this_class_name, "test_loadconfig1_fromcode"); 

		const char *p[] = { "!urlhost(vtraxe.com vtrah.net) & sitecategory(googlesex)",
			"sitecategory(all)",
			"srcip(127.0.0.1)",
			"!srcip(10.0.0.0-10.255.255.255) & !srcip(172.30.0.0/255.255.0.0) & !srcip(192.168.0.0-192.168.255.255)"
		};

		proxyrule_iterator ruleiter = cfg2.rules.begin();
		int r = reverse_order ? (PROXYRULES_TOTAL - 1) : 0;
		int count = 0;
		while(count < PROXYRULES_TOTAL)
		{
			std::string descr;
			ruleiter->create_description(descr);

			if (strcmp(p[r], descr.c_str()) != 0)
				throw test_exception(this_class_name, "test_loadconfig1_fromcode"); 

			if (!reverse_order)
				r++;
			else
				r--;

			count++;
			++ruleiter;
		}

		int CONDITEMS_TOTAL = 7;
		if (cfg2.conditems.size() != CONDITEMS_TOTAL)
			throw test_exception(this_class_name, "test_loadconfig1_fromcode"); 

		conditem_iterator conditer = cfg2.conditems.begin();

		int k = 0;
		while(k < CONDITEMS_TOTAL)
		{
			std::shared_ptr<conditem> ci = *conditer;

			bool error = false;

			switch(k)
			{
				case 0: {
					conditem_addrurl* cci0 = (conditem_addrurl*)ci.get();
					error = cci0->urlpart != conditem_addrurl::urlpart_host;
					break; }

				case 1: {
					conditem_sitecategory* cci1 = (conditem_sitecategory*)ci.get();
					error = cci1->categorytype != conditem_sitecategory::category_googlesex;
					break; }

				case 2: {
					conditem_sitecategory* cci2 = (conditem_sitecategory*)ci.get();
					error = cci2->categorytype != conditem_sitecategory::category_all;
					break; }

				case 3: {
					conditem_addrip* cci3 = (conditem_addrip*)ci.get();
					error = cci3->addrtype != conditem_addrip::addr_single;
					break; }

				case 4: {
					conditem_addrip* cci4 = (conditem_addrip*)ci.get();
					error = cci4->addrtype != conditem_addrip::addr_range;
					break; }

				case 5: {
					conditem_addrip* cci5 = (conditem_addrip*)ci.get();
					error = cci5->addrtype != conditem_addrip::addr_network;
					break; }

				case 6: {
					conditem_addrip* cci6 = (conditem_addrip*)ci.get();
					error = cci6->addrtype != conditem_addrip::addr_range;
					break; }
			}

			if (error)
				throw test_exception(this_class_name, "test_loadconfig1_fromcode");

			k++;
			++conditer;
		}
	}

	return;
}

void configproxy::test_loadconfig2_fromcode(configproxy& cfg)
{
	cfg.clear();
	cfg.enabled = true;
	load_default_thirdleveldomains(cfg);

	addrip_v4 localhost("127.0.0.1");
	addrip_v4 any_ip("0.0.0.0");
	addrip_v4 any_mask("0.0.0.0");

	proxyrule pr1, pr2;

	{
		pr1.action = proxyrule::action_allow;
		pr1.rulename = "localhostrule";

		std::shared_ptr<conditem_addrip> ci0 = std::shared_ptr<conditem_addrip>(new conditem_addrip(conditem_addrip::addr_single, localhost, localhost));
		ci0->condname = "localhost";
		cfg.conditems.push_back(ci0);

		proxyrule_item pri0;
		pri0.ci = ci0;
		pri0.invert = false;
		pri0.condname = "localhost";
		pr1.items.push_back(pri0);

		cfg.rule_add(pr1);
	}

	{
		std::shared_ptr<conditem_addrip> ci1 = std::shared_ptr<conditem_addrip>(new conditem_addrip(conditem_addrip::addr_network, any_ip, any_mask));
		ci1->condname = "any";
		cfg.conditems.push_back(ci1);

		proxyrule pr2;
		pr2.action = proxyrule::action_deny;
		pr2.rulename = "anyrule";
		proxyrule_item pri2;
		pri2.ci = ci1;
		pri2.invert = false;
		pri2.condname = "any";
		pr2.items.push_back(pri2);

		cfg.rule_add(pr2);
	}

	return;
}

void configproxy::test_loadconfig3_fromcode(configproxy& cfg)
{
	size_t insert_pos = INT_MAX;

	cfg.clear();
	cfg.enabled = true;
	load_default_thirdleveldomains(cfg);

	{
		// ProxyRule 1
		proxyrule pr1;

		pr1.action = proxyrule::action_deny;
		pr1.rulename = "Black list";

		std::shared_ptr<conditem_addrurl> ci0 = std::shared_ptr<conditem_addrurl>(new conditem_addrurl(conditem_addrurl::urlpart_host, "porno lesbian"));
		ci0->condname = "bad_words";
		cfg.conditems.push_back(ci0);

		proxyrule_item pri0("bad_words", false, ci0);
		pr1.items.push_back(pri0);

		cfg.rule_add(pr1, insert_pos);
	}

	{
		// ProxyRule 2
		proxyrule pr2;

		addrip_v4 lo("127.0.0.1");
		addrip_v4 r1("192.168.0.0");
		addrip_v4 r2("192.168.255.255");
		addrip_v4 u1("10.0.0.0");
		addrip_v4 u2("10.255.255.255");
		addrip_v4 q1("172.30.0.0");
		addrip_v4 q2("255.255.0.0");

		pr2.action = proxyrule::action_deny;
		pr2.rulename = "Prohibit access from Internet";

		std::shared_ptr<conditem_addrip> ci3 = std::shared_ptr<conditem_addrip>(new conditem_addrip(conditem_addrip::addr_single, lo, lo));
		ci3->condname = "localhost";
		cfg.conditems.push_back(ci3);

		std::shared_ptr<conditem_addrip> ci4 = std::shared_ptr<conditem_addrip>(new conditem_addrip(conditem_addrip::addr_range, u1, u2));
		ci4->condname = "private net0";
		cfg.conditems.push_back(ci4);

		std::shared_ptr<conditem_addrip> ci5 = std::shared_ptr<conditem_addrip>(new conditem_addrip(conditem_addrip::addr_network, q1, q2));
		ci5->condname = "private net1";
		cfg.conditems.push_back(ci5);

		std::shared_ptr<conditem_addrip> ci6 = std::shared_ptr<conditem_addrip>(new conditem_addrip(conditem_addrip::addr_range, r1, r2));
		ci6->condname = "private net2";
		cfg.conditems.push_back(ci6);

		proxyrule_item pri3("localhost", true, ci3);
		pr2.items.push_back(pri3);

		proxyrule_item pri4("private net0", true, ci4);
		pr2.items.push_back(pri4);

		proxyrule_item pri5("private net1", true, ci5);
		pr2.items.push_back(pri5);

		proxyrule_item pri6("private net2", true, ci6);
		pr2.items.push_back(pri6);

		cfg.rule_add(pr2, insert_pos);
	}

	{
		// ProxyRule 3 - allow access to all sites using https protocol
		proxyrule pr3;

		pr3.action = proxyrule::action_allow;
		pr3.rulename = "Https welcome";

		std::shared_ptr<conditem_sitecategory> ci2 = std::shared_ptr<conditem_sitecategory>(new conditem_sitecategory());
		ci2->set_conditem_sitecategory(conditem_sitecategory::category_https);
		ci2->condname = "https";
		cfg.conditems.push_back(ci2);

		proxyrule_item pri2("https", false, ci2);
		pr3.items.push_back(pri2);

		cfg.rule_add(pr3, insert_pos);
	}

	{
		// ProxyRule 3b - allow access to "White list sites"
		proxyrule pr3;

		pr3.action = proxyrule::action_allow;
		pr3.rulename = "White list";

		std::shared_ptr<conditem_addrurl> ci2 = std::shared_ptr<conditem_addrurl>(new conditem_addrurl(conditem_addrurl::urlpart_host, "google.com googleadservices.com youtube.com youtu.be ytimg.com"));
		ci2->condname = "white_list";
		cfg.conditems.push_back(ci2);

		proxyrule_item pri2("white_list", false, ci2);
		pr3.items.push_back(pri2);

		cfg.rule_add(pr3, insert_pos);
	}

	{
		// ProxyRule 4 - deny access to googlesex
		proxyrule pr4;

		pr4.action = proxyrule::action_deny;
		pr4.rulename = "Adult content";

		std::shared_ptr<conditem_sitecategory> ci2 = std::shared_ptr<conditem_sitecategory>(new conditem_sitecategory());
		ci2->set_conditem_sitecategory(conditem_sitecategory::category_googlesex);
		ci2->condname = "sexual";
		cfg.conditems.push_back(ci2);

		proxyrule_item pri2("sexual", false, ci2);
		pr4.items.push_back(pri2);

		cfg.rule_add(pr4, insert_pos);
	}

	{
		// ProxyRule 5 - allow access to all sites
		proxyrule pr5;

		pr5.action = proxyrule::action_allow;
		pr5.rulename = "All sites";

		std::shared_ptr<conditem_sitecategory> ci2 = std::shared_ptr<conditem_sitecategory>(new conditem_sitecategory());
		ci2->set_conditem_sitecategory(conditem_sitecategory::category_all);
		ci2->condname = "all";
		cfg.conditems.push_back(ci2);

		proxyrule_item pri2("all", false, ci2);
		pr5.items.push_back(pri2);

		cfg.rule_add(pr5, insert_pos);
	}

	return;
}



}
