#ifndef PROXY_CONN_H
#define PROXY_CONN_H

#ifndef NOPROXYSERVER

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "proxyserver_common.h"
#include "proxy-serverbase.h"
#include "http_headers.h"
#include "configproxy.h"
#include "proxymsg.h"

#define BUFFER_SIZE 8192

namespace utm {

struct connection_stat
{
public:
	size_t hdr_bytes_brw_read;
	size_t hdr_bytes_brw_write;
	size_t hdr_bytes_srv_read;
	size_t hdr_bytes_srv_write;

	size_t post_bytes_brw_read;
	size_t post_bytes_srv_write;

	size_t body_count_srv_read;
	size_t body_bytes_srv_read;
	size_t body_bytes_brw_write;
};

class connection : public boost::enable_shared_from_this<connection> {
public:
	typedef boost::shared_ptr<connection> pointer;

	static pointer create(ba::io_service& io_service, serverbase* _proxysrv) {
		return pointer(new connection(io_service, _proxysrv));
	}

	ba::ip::tcp::socket& socket() {
		return brw_sock;
	}

	serverbase* proxysrv;

	void start();
	void finish();

	~connection();

	static size_t connections_started;
	static size_t connections_finished;

private:
	connection(ba::io_service& io_service, serverbase* _proxysrv);

	void handle_browser_read_headers(const bs::error_code& err, size_t len);

	void http_start_connect();
	void http_start_write_to_server();
	void http_browser_start_write(const bs::error_code& err, size_t len);
	void http_browser_write();
	void http_browser_write(const bs::error_code& err, size_t len);
	void http_browser_start_write_html();
	void http_browser_write_html(const bs::error_code& err, size_t len);
	void http_server_write(const bs::error_code& err, size_t len);
	void http_browser_read_postdata(const bs::error_code& err, size_t len);
	void http_server_write_postdata(const bs::error_code& err, size_t len);
	void http_server_read_headers(const bs::error_code& err, size_t len);
	void http_server_read_body(const bs::error_code& err, size_t len);
	void http_resolve(const boost::system::error_code& err, ba::ip::tcp::resolver::iterator endpoint_iterator);
	void http_connect(const boost::system::error_code& err, ba::ip::tcp::resolver::iterator endpoint_iterator);
	void http_browser_read_postdata_in_webservermode(const bs::error_code& err, size_t len);

	void ssl_start_connect();
	void ssl_handle_resolve(const boost::system::error_code& err, ba::ip::tcp::resolver::iterator endpoint_iterator);
	void ssl_handle_connect(const boost::system::error_code& err, ba::ip::tcp::resolver::iterator endpoint_iterator);
	void ssl_start_reading(const bs::error_code& err, size_t len);
	void ssl_browser_read(const bs::error_code& err, size_t len);
	void ssl_browser_write(const bs::error_code& err, size_t len);
	void ssl_server_write(const bs::error_code& err, size_t len);
	void ssl_server_read(const bs::error_code& err, size_t len);

	void shutdown();
	void shutdown(const bs::error_code& err);
	void shutdown(bool is_graceful);

	std::string record_makefilename(char postfix, size_t seqnum = 0);
	void record_data(const char *p, size_t len, char postfix, size_t seqnum = 0);
	void record_message(const char *p, size_t len, const bs::error_code& err);

	boost::posix_time::ptime timestamp_start;
	boost::posix_time::ptime timestamp_finish;

	ba::io_service& io_service_;
	ba::ip::tcp::socket brw_sock;
	ba::ip::tcp::socket srv_sock;
	ba::ip::tcp::resolver resolver_;
	bool proxy_closed;
	bool is_persistent;
	
	size_t brw_buff_lastlen;
	size_t brw_buff_endpos;

	size_t srv_buff_lastlen;
	size_t srv_buff_endpos;

	boost::array<char, BUFFER_SIZE> brw_buff;
	boost::array<char, BUFFER_SIZE> srv_buff;

	connection_stat stat;
	request_http_headers req;
	response_http_headers resp;

	int connection_id;
	int request_counter;
	bool debug_http;

	void start_web_server();
	void send_deny_response(proxymsg::msgtypes msgtype, const char *requestedsite, const char *proxyrulename, bool include_http_header, bool use_gzip);
	void send_deny_response(std::string& denyresponsecode, std::string& denycontent, bool include_http_header, bool use_gzip);
	void process_request();
	static std::string process_until_header_end(const std::string& existed_data, char *databuff, size_t len, size_t *processed_len, bool *end_found);
};

}

#endif

#endif // PROXY_CONN_H
