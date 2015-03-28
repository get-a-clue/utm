#pragma once

#if defined(_MSC_VER)
#define UTM_WIN
#endif

#ifdef UTM_WIN

#include <Windows.h>
#include <tchar.h>

#endif

#define DEFAULT_ASKGOOGLE_SITE "www.google.com"
#define DEFAULT_ASKGOOGLE_lang "en"
#define DEFAULT_ASKGOOGLE_USERAGENT "User-Agent: Opera/9.80 (Windows NT 6.1; U; ru) Presto/2.10.229 Version/11.61"

//#define DENYPAGE_PATTERN_TIMESTAMP "%timestamp%"
//#define DENYPAGE_PATTERN_HOST "%host%"

//#define DEFAULT_DENYRESPONSE_TITLE "<HTML><TITLE>Access denied</TITLE><BODY>"
//#define DEFAULT_DENYRESPONSE_FOOTER "</BODY></HTML>"
//#define DEFAULT_DENYRESPONSE_CODE "403 Forbidden"

//#define DEFAULT_DENYRESPONSE_NOTCONFIGURED_BODY "Sorry, configuration is not defined. Requests are not served."
//#define DEFAULT_DENYRESPONSE_BLOCK_BODY "Sorry, but access to site '%host%' is prohibited by proxy rule '%rule%'."
//#define DEFAULT_DENYRESPONSE_NOTACCEPT_BODY "Sorry, but accepting connections are not allowing because configuration is being updating. Please, try to reload page later."
//#define DEFAULT_DENYRESPONSE_NOTRESOLVED_BODY "Unfortunatenaly, host '%host%' has been not resolved. Check your Internet connection and ensure that you typed hostname properly."
//#define DEFAULT_DENYRESPONSE_GOOGLEERROR_BODY "There are network error while trying to reach google servers when check a status of host '%host%'."

#include <string>
#include <vector>
#include <map>
#include <list>

#include <addrip_v4.h>
#include <stringtools.h>
#include <utime.h>
#include <ubase.h>
#include <ubase_test.h>
#include <loggingparams.h>

#include "conditem.h"
#include "conditem_addrip.h"
#include "conditem_addrurl.h"
#include "conditem_sitecategory.h"
#include "proxyrule.h"
#include "proxymsg.h"

#define CONFIGPROXY_XMLTAG_PROXY "Proxy"
#define CONFIGPROXY_XMLTAG_ENABLED "Enabled"
#define CONFIGPROXY_XMLTAG_WEBSERVERMODE "WebserverMode"
#define CONFIGPROXY_XMLTAG_MAXTHREADS "MaxThreads"
#define CONFIGPROXY_XMLTAG_PORT "Port"
#define CONFIGPROXY_XMLTAG_DBMASK "DbMask"
#define CONFIGPROXY_XMLTAG_DBPREFIX "DbPrefix"
#define CONFIGPROXY_XMLTAG_DENYBODY "Denybody"
#define CONFIGPROXY_XMLTAG_DENYCODE "Denycode"
#define CONFIGPROXY_XMLTAG_CONDITEM "Conditem"
#define CONFIGPROXY_XMLTAG_WEBSERVERHOSTNAME "WebserverHostname"
#define CONFIGPROXY_XMLTAG_THIRDLEVELDOMAINS "Thirddom"
#define CONFIGPROXY_XMLTAG_ASKSITE "AskSite"
#define CONFIGPROXY_XMLTAG_ASKLANG "AskLang"
#define CONFIGPROXY_XMLTAG_ASKUSERAGENT "AskUserAgent"

namespace utm {

typedef std::vector<std::string> domains_list_container;
typedef std::vector<std::string>::iterator domains_list_iterator;
typedef std::vector<std::string>::const_iterator domains_list_constiterator;

typedef std::shared_ptr<conditem> conditem_smartptr;
typedef std::list<std::shared_ptr<conditem>> conditem_container;
typedef std::list<std::shared_ptr<conditem>>::iterator conditem_iterator;

typedef std::list<proxyrule> proxyrule_container;
typedef std::list<proxyrule>::iterator proxyrule_iterator;

typedef std::list<loggingparams> loggingparams_container;
typedef std::list<loggingparams>::iterator loggingparams_iterator;

class configproxy : public ubase
{
public:
	static const char this_class_name[];
	const char* get_this_class_name() { return configproxy::this_class_name; };

	enum deny_response_types {
		response_notconfigured = 0,
		response_notaccept = 1,
		response_block = 2,
		response_notresolved = 3,
		response_googleerror = 4
	};

	configproxy(void);
	configproxy(const configproxy& r);
	~configproxy(void);

	configproxy& operator=(const configproxy &rhs);
	bool operator ==(const configproxy& rhs) const;

	bool enabled;
	bool webserver_mode;
	unsigned short max_threads;
	unsigned short listen_port;
	unsigned short dbmask;
	std::string dbprefix;
	std::string denybody;
	std::string denycode;
	std::string webserver_hostname;
	domains_list_container third_level_domains;

	std::string askgoogle_site;
	std::string askgoogle_lang;
	std::string askgoogle_useragent;

	const char *get_askgoogle_site();
	const char *get_askgoogle_lang();
	const char *get_askgoogle_useragent();

	conditem conditem_fake;
	conditem_container conditems;
	proxyrule_container rules;
	loggingparams_container logging;
	proxymsg messages_eng;
	proxymsg_rus messages_rus;

	void rule_get(size_t index, proxyrule& prule);
	void rule_add(const proxyrule& prule, size_t insert_pos = INT_MAX);
	void rule_replace(const proxyrule& prule, size_t index);
	void rule_delete(size_t index);
	void rule_moveup(size_t index);
	void rule_movedown(size_t index);
	void purge_conditems();

	void clear();
	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue);
	ubase* xml_catch_subnode(const char *name);
	void xml_catch_subnode_finished(const char *name);

	void get_denyresponse(proxymsg::msgtypes msgtype, const char *browserlang, const char *requestedsite, const char *proxyrulename, std::string& denycontent, std::string& denyresponsecode);	
	static std::string configproxy::make_defaultdenyresponse(deny_response_types denytype);

	bool process_request(proxyrequest_params *params);

	static void load_default_logfacilities(configproxy& cfg);
	static void load_default_thirdleveldomains(configproxy& cfg);

#ifdef UTM_DEBUG
	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num);

	static void test_loadconfig();
#endif

	static void test_loadconfig1_fromcode(configproxy& cfg, bool reverse_order = false);
	static void test_loadconfig2_fromcode(configproxy& cfg);
	static void test_loadconfig3_fromcode(configproxy& cfg);
};

}