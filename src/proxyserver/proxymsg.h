#pragma once

#if defined(_MSC_VER)
#define UTM_WIN
#include <WinSock2.h>
#endif

#include <gstring.h>
#include <ubase.h>
#include <enumhelper.h>

#include <map>
#include <string>

#define PROXYMSG_PATTERN_TIMESTAMP "%timestamp%"
#define PROXYMSG_PATTERN_SITE "%site%"
#define PROXYMSG_PATTERN_PROXYRULE "%rule%"
#define PROXYMSG_PATTERN_HOST "%host%"

#define PROXYMSG_XMLTAG_ENG "MsgEng"
#define PROXYMSG_XMLTAG_RUS "MsgRus"
#define PROXYMSG_XMLTAG_MSG "M"
#define PROXYMSG_XMLTAG_HEADER "Header"
#define PROXYMSG_XMLTAG_FOOTER "Footer"

#define PROXYMSG_DEFAULT_HEADER_ENG "<HTML><TITLE>Access denied</TITLE><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><style type=\"text/css\">h3 {font-family: Verdana, Arial; font-style:bold font-size:18pt;} p {font-family: Verdana, Arial; font-size:14pt;} td.h1 { font-family:Verdana,Arial; font-size:12pt; font-style:italic; text-align:left; border-top:1px solid black; } </style><BODY><h3>Sorry</h3><p>"
#define PROXYMSG_DEFAULT_FOOTER_ENG "</p><table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td class=\"h1\">Generated at %host%@%timestamp%</h1><td></tr></table></BODY></HTML>"

#define PROXYMSG_DEFAULT_HEADER_RUS "<HTML><TITLE>Доступ запрещен</TITLE><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><style type=\"text/css\">h3 {font-family: Verdana, Arial; font-style:bold font-size:18pt;} p {font-family: Verdana, Arial; font-size:14pt;} td.h1 { font-family:Verdana,Arial; font-size:12pt; font-style:italic; text-align:left; border-top:1px solid black; } </style><BODY><h3>Извините</h3><p>"
#define PROXYMSG_DEFAULT_FOOTER_RUS "</p><table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td class=\"h1\">Сформировано %host%@%timestamp%</h1><td></tr></table></BODY></HTML>"

#define PROXYMSG_MSGTYPE_MAXITEMS 6

#define PROXYMSG_MSGTYPE_NONE_ENG "None"
#define PROXYMSG_MSGTYPE_NOTCONFIGURED_ENG "Configuration is not defined. Requests are not served."
#define PROXYMSG_MSGTYPE_BLOCK_ENG "Access to site '%site%' is prohibited by proxy rule '%rule%'."
#define PROXYMSG_MSGTYPE_NOTACCEPT_ENG "Accepting connections are not allowing because configuration is being updating. Please, try to reload page later."
#define PROXYMSG_MSGTYPE_NOTRESOLVED_ENG "Unfortunatenaly, host '%site%' has been not resolved. Check your Internet connection and ensure that you typed hostname properly."
#define PROXYMSG_MSGTYPE_GOOGLEERROR_ENG "There are network error while trying to reach google servers when check a status of host '%site%'."

#define PROXYMSG_MSGTYPE_NONE_RUS "Нет"
#define PROXYMSG_MSGTYPE_NOTCONFIGURED_RUS "Конфигурация не определена. Запросы от пользователей не обслуживаются."
#define PROXYMSG_MSGTYPE_BLOCK_RUS "Доступ к сайту '%site%' запрещен прокси правилом '%rule%'."
#define PROXYMSG_MSGTYPE_NOTACCEPT_RUS "Обслуживание запросов приостановлено поскольку идет процесс обновления конфигурации. Пожалуйста, повторите запрос позже."
#define PROXYMSG_MSGTYPE_NOTRESOLVED_RUS "К сожалению, сайт '%site%' не найден. Проверьте соединение с Интернетом и убедитесь, что имя сайта введено правильно."
#define PROXYMSG_MSGTYPE_GOOGLEERROR_RUS "Произошла ошибка при обращении к серверам google при определении категории сайта '%site%'."

#define PROXYMSG_MSGTYPE_REPLY_CODE 403

namespace utm {

struct replymsg {
	gstring msg;
	int http_code;

	bool operator==(const replymsg& rhs) const
	{
		if (msg != rhs.msg)
			return false;

		if (http_code != rhs.http_code)
			return false;

		return true;
	}
};

typedef std::map<int, replymsg> replymsg_container;

class proxymsg : public ubase
{
public:
	static const char this_class_name[];
	const char* get_this_class_name() const { return proxymsg::this_class_name; };

	proxymsg(void);
	virtual ~proxymsg(void);

	enum msgtypes {
		msgtype_none = 0,
		msgtype_notconfigured = 10,
		msgtype_notaccept = 20,
		msgtype_block = 30,
		msgtype_notresolved = 40,
		msgtype_googleerror = 50
	};
	static const char* msgtypes_str_eng[];
	static const char* msgtypes_str_rus[];
	static const int msgtypes_int[];

	enum msglangs {
		lang_eng = 0,
		lang_rus = 1
	};

	bool operator==(const proxymsg& rhs) const;

	virtual int get_lang() const { return lang_eng; };
	replymsg_container messages;
	gstring header;
	gstring footer;

	void fill_by_default();
	void set_message(int msgtype, const gstring& msg, int http_code);
	void get_message(int msgtype, const char *requestedsite, const char *proxyrulename, std::string& msg, int& http_code);
	static void get_default_message(int msgtype, const char *browserlang, std::string& msg, int& http_code);
	static void get_default_header_footer(int msgtype, gstring& hdr, gstring& ftr);
	static void format_message(const char *message, const char *header, const char *footer, const char *requestedsite, const char *proxyrulename, std::string& result);
	static int parse_browser_language(const char *browserlang);

	void clear();
	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue);
	ubase* xml_catch_subnode(const char *name) { return NULL; };

#ifdef UTM_DEBUG
	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num);
#endif
};

class proxymsg_rus : public proxymsg
{
public:
	proxymsg_rus()
	{
		fill_by_default();
	}

	int get_lang() const { return lang_rus; };
};

}