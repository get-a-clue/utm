#include "stdafx.h"

#ifndef NOPROXYSERVER

#include "proxy-conn.hpp"
#include "googlecookies.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/crc.hpp>

#define ZLIB_WINAPI
#include "zlib.h"

#include <iostream>
#include <iomanip>

#if defined(_MSC_VER)
#include <iterator>
#endif

#define RECORD_DATA_NOPOSTFIX ' '
#define RECORD_DATA_HTTP_BROWSER 'b'
#define RECORD_DATA_FROMCLIENT 'c'
#define RECORD_DATA_FROMSERVER 's'

namespace utm {

size_t connection::connections_started = 0;
size_t connection::connections_finished = 0;


connection::connection(ba::io_service& io_service, serverbase* _proxysrv) : io_service_(io_service),
													 brw_sock(io_service),
													 srv_sock(io_service),
													 resolver_(io_service),
													 proxy_closed(false),
													 is_persistent(false),
													 request_counter(0),
													 debug_http(false)
{
	proxysrv = _proxysrv;
//	resp.func_process_request = boost::bind(&utm::serverbase::process_request, proxysrv, _1);
}

connection::~connection()
{
	finish();
}

void connection::start() 
{
	if (request_counter == 0)
	{
		connection_id = connections_started;
		connections_started++;
	}

//#ifdef _DEBUG
//	std::cout << "START Connection started/finished: " << connections_started << "/" << connections_finished << std::endl;
//#endif
	request_counter++;

	memset(&stat, 0, sizeof(stat));
	req.clear();
	resp.clear();

	brw_buff_lastlen = 0;
	brw_buff_endpos = 0;
	srv_buff_lastlen = 0;
	srv_buff_endpos = 0;

	timestamp_start = boost::posix_time::microsec_clock::local_time();

	async_read(brw_sock, ba::buffer(brw_buff), ba::transfer_at_least(1),
			   boost::bind(&connection::handle_browser_read_headers,
			   shared_from_this(),
			   ba::placeholders::error,
			   ba::placeholders::bytes_transferred));
}

void connection::finish()
{
	connections_finished++;
//#ifdef _DEBUG
//	std::cout << "~~~~~ Connection started/finished: " << connections_started << "/" << connections_finished << std::endl;
//#endif
	timestamp_finish = boost::posix_time::microsec_clock::local_time();

	if (proxysrv->squidlogger != NULL)
	{
		squidrecord sr;
		sr.timestamp = timestamp_finish;
		sr.elapsed = timestamp_finish - timestamp_start;
		sr.client = req.client_addr.is_unspecified() ? "-" : req.client_addr.to_string();

		if ((resp.response_code >= 100) && (resp.response_code < 1000))
			sr.code = resp.response_code;

		sr.size = stat.hdr_bytes_brw_write + stat.body_bytes_brw_write;
		sr.method = req.method;
		sr.uri = req.url.original_addrurl;
		sr.from = resp.server_addr.is_unspecified() ? "-" : resp.server_addr.to_string();
		sr.content = resp.mime_type;

		proxysrv->squidlogger->add_item(sr);
	};

	req.clear();
	resp.clear();
}

std::string connection::record_makefilename(char postfix, size_t seqnum)
{
	std::ostringstream oss;
	oss << ufs::get_current_directory().getmb() << "debug\\";
	oss << std::setfill('0') << std::setw(6) << connection_id;

	if (seqnum > 0)
		oss << "_" << std::setfill('0') << std::setw(2) << seqnum;

	if (postfix != ' ')
		oss << postfix;

	std::string filename = oss.str();

	boost::filesystem::path path(filename);
	boost::filesystem::path dir = path.parent_path();

	if (!boost::filesystem::is_directory(dir))
		boost::filesystem::create_directories(dir);

	return filename;
}

void connection::record_message(const char *p, size_t len, const bs::error_code& err)
{
	std::string filename = record_makefilename('m', 0);

	std::ofstream ofile;
	ofile.open(filename, std::ios::out | std::ios::binary | std::ios::app);
	
	if (ofile.is_open())
	{
		ofile << p << " : " << len;
		if (err)
			ofile << " **err";
			
		ofile << std::endl;
		ofile.close();
	}
}

void connection::record_data(const char *p, size_t len, char postfix, size_t seqnum)
{
	if (len == 0)
		return;

	std::string filename = record_makefilename(postfix, seqnum);

	std::ofstream ofile;
	ofile.open(filename, std::ios::out | std::ios::binary | std::ios::app);
	
	if (ofile.is_open())
	{
		ofile.write(p, len);
		ofile.close();
	}
}

void connection::handle_browser_read_headers(const bs::error_code& err, size_t len) 
{
	if (debug_http)
	{
		record_data(brw_buff.data(), len, RECORD_DATA_NOPOSTFIX);
		record_message("handle_browser_read_headers", len, err);
	}

	if (err)
	{
		shutdown(err);
		return;
	}

	size_t processed_len = 0;
	bool end_found = false;

	req.original = process_until_header_end(req.original, brw_buff.data(), len, &processed_len, &end_found);

	if (processed_len == len)
	{
		stat.hdr_bytes_brw_read += len;
	}
	else
	{
		stat.hdr_bytes_brw_read += processed_len;
		stat.post_bytes_brw_read += len - processed_len;
	}

	brw_buff_lastlen = len;
	brw_buff_endpos = processed_len;

	if (!end_found) 
	{
		async_read(brw_sock, ba::buffer(brw_buff), ba::transfer_at_least(1),
					boost::bind(&connection::handle_browser_read_headers,
					shared_from_this(),
					ba::placeholders::error,
					ba::placeholders::bytes_transferred));
	} 
	else 
	{ 
		if (!req.parse())
		{
			shutdown();
			return;
		}

		req.client_addr = brw_sock.remote_endpoint().address();
		req.client_port = brw_sock.remote_endpoint().port();

		if (proxysrv->cfg.get() != NULL)
		{
			const char *googlesite = proxysrv->cfg->get_askgoogle_site();
			if (strcmp(req.get_hostname(), googlesite) == 0)
			{
				size_t captcha_pos = stringtools::ci_find_substr(req.referer.path, std::string("captcha"));
				if (captcha_pos != std::string::npos)
				{
					const char *cookie = req.find_header("cookie");
					const char *useragent = req.find_header("user-agent");

					if ((cookie != NULL) && (useragent != NULL))
					{
						googlecookies* gc = googlecookies::instance();
						gc->set_cookie(req.client_addr, useragent, googlesite, cookie);
					}
				}
			}

			if ((!proxysrv->cfg->enabled) || (!proxysrv->is_accept_connections))
			{
				send_deny_response(proxymsg::msgtype_notaccept, req.url.host.c_str(), NULL, true, false);
			}
			else
			{
				process_request();
			}
		}
		else
		{
			send_deny_response(proxymsg::msgtype_notconfigured, req.url.host.c_str(), NULL, true, false);
		}
	}
}

void connection::process_request()
{
	bool urlok = false;

	proxyrequest_params pp(req);

	//	pp.useragent = req.useragent.empty() ? NULL : req.useragent.c_str();

	configproxy* pcfg = proxysrv->cfg.get();

	if ((pcfg != NULL) && (req.reqtype != request_http_headers::request_unknown))
	{
		if ((pcfg->webserver_mode) || 
			(pcfg->webserver_hostname.empty() && req.url.second_level_domain.find(pcfg->webserver_hostname) != std::string::npos))
		{
			urlok = true;

			if (req.method == "POST")
			{
				bs::error_code ec;
				ec.clear();
				http_browser_read_postdata_in_webservermode(ec, 0);
			} 
			else
			{
				start_web_server();
			}
		}
		else
		{
			urlok = proxysrv->process_request(&pp);

			if (urlok)
			{
				if (req.reqtype == request_http_headers::https_thru_proxy)
					ssl_start_connect();
				else
					http_start_connect();
			}
		}
	}

	if (!urlok)
	{
		bool is_sent = false;
		if (pp.askexc.get() != NULL)
		{
			if (pp.askexc->response_code != 200)
			{
				// Show google reply to user

				std::ostringstream osscode;
				osscode << pp.askexc->response_code;

				send_deny_response(osscode.str(), pp.askexc->response_data, true, false);
				is_sent = true;
			}
			else
			{
				if ((pp.askexc->phase == askgoogle_exception::phase_request) ||
					(pp.askexc->phase == askgoogle_exception::phase_nohttp))
				{
					send_deny_response(proxymsg::msgtype_googleerror, pp.request->url.host.c_str(), pp.proxyrulename4web, true, false);
					is_sent = true;
				}
			}
		}

		if (!is_sent)
		{
			send_deny_response(proxymsg::msgtype_block, pp.request->url.host.c_str(), pp.proxyrulename4web, true, false);
		}
	}
}

void connection::send_deny_response(proxymsg::msgtypes msgtype, const char *requestedsite, const char *proxyrulename, bool include_http_header, bool use_gzip)
{
	const char *browserlang = req.find_header("accept-language");

	std::string denycontent;
	std::string denyresponsecode;

	if (proxysrv->cfg != NULL)
	{
		proxysrv->cfg->get_denyresponse(msgtype, browserlang, requestedsite, proxyrulename, denycontent, denyresponsecode);
	}
	else
	{
		int http_code = 0;
		// kazak TODO
		proxymsg::get_default_message(proxymsg::msgtype_notconfigured, browserlang, denycontent, http_code);
		std::ostringstream oss; oss << http_code; denyresponsecode = oss.str();
	}

	send_deny_response(denyresponsecode, denycontent, include_http_header, use_gzip);
}

void connection::send_deny_response(std::string& denyresponsecode, std::string& denycontent, bool include_http_header, bool use_gzip)
{
	size_t contentlen = denycontent.size();

	std::vector<unsigned char> response;

	if (use_gzip)
	{
		boost::crc_32_type result;
		result.process_bytes(denycontent.data(), contentlen);
		int crc32 = result.checksum();
		char *pcrc32 = (char *)&crc32;

        const unsigned char gzipheader[] = {
            0x1f, 0x8b,     // gzip magic number
            0x08,           // compress method "defalte"
            0x01,           // text data
            0, 0, 0, 0,     // timestamp is not set
            0x02,           // maximum compression flag
            0xff            // unknown OS
	    };

		size_t pq = contentlen + 1000;
		std::vector<unsigned char> ap(pq);

		int ret;
		z_stream strm;

		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;

		ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15, 8, Z_DEFAULT_STRATEGY);
		if (ret == Z_OK)
		{
			strm.avail_in = contentlen;
			strm.next_in = (unsigned char *)denycontent.c_str();
			strm.avail_out = pq;
			strm.next_out = ap.data();
			ret = deflate(&strm, Z_FINISH);

			int len_out = pq - strm.avail_out;

			std::copy(gzipheader, gzipheader + 10, std::back_inserter(response));
			std::copy(ap.data() + 2, ap.data() + len_out - 4, std::back_inserter(response));
			std::copy(pcrc32, pcrc32 + 4, std::back_inserter(response));

			unsigned int clen32 = (unsigned int)contentlen;
			char *plen = (char *)&clen32;
			std::copy(plen, plen + sizeof(clen32), std::back_inserter(response));

			(void)deflateEnd(&strm);
		}
	}
	else
	{
		std::ostringstream oss;

		if (include_http_header)
		{
			oss << "HTTP/1.1 " << denyresponsecode << "\r\n";
			oss << "Content type: text/html; charset=utf-8\r\n";
			oss << "Expires: Thu, 01 Dec 1994 16:00:00 GMT\r\n";
			oss << "Content-Length: " << contentlen << "\r\n";
			oss << "Connection: close\r\n\r\n";
		}

		oss << denycontent;
		std::string r = oss.str();
		std::copy(r.begin(), r.end(), std::back_inserter(response));
	}

	size_t len = response.size();
	resp.response_raw.resize(len);
	std::copy(response.begin(), response.end(), resp.response_raw.begin());
	
	http_browser_start_write_html();
}

void connection::start_web_server()
{
//  TODO
//	webserver::start(&req, &resp);
	proxysrv->process_webserver_request(req, resp);
	http_browser_start_write_html();
}

void connection::http_start_connect() 
{
	ba::ip::tcp::resolver::query query(req.get_hostname(), req.get_hostport());
	resolver_.async_resolve(query,
							boost::bind(&connection::http_resolve, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::iterator));
}

void connection::http_resolve(const boost::system::error_code& err,	ba::ip::tcp::resolver::iterator endpoint_iterator) 
{
	if (err)
	{
		// Error in resolving hostname
		send_deny_response(proxymsg::msgtype_notresolved, req.url.host.c_str(), NULL, true, false);
		shutdown(err);
		return;
	}

	ba::ip::tcp::endpoint endpoint = *endpoint_iterator;
	resp.server_addr = endpoint.address();

	srv_sock.async_connect(endpoint,
							boost::bind(&connection::http_connect, shared_from_this(),
							boost::asio::placeholders::error,
							++endpoint_iterator));
}

void connection::http_connect(const boost::system::error_code& err,
								ba::ip::tcp::resolver::iterator endpoint_iterator) {
    if (!err) 
	{
		http_start_write_to_server();
    } 
	else
	if (endpoint_iterator != ba::ip::tcp::resolver::iterator()) 
	{
		srv_sock.close();
		ba::ip::tcp::endpoint endpoint = *endpoint_iterator;
		srv_sock.async_connect(endpoint,
							boost::bind(&connection::http_connect, shared_from_this(),
							boost::asio::placeholders::error,
							++endpoint_iterator));
	
	} 
	else 
	{
		shutdown();
	}
}

void connection::http_start_write_to_server() 
{
	ba::async_write(srv_sock, ba::buffer(req.request_for_server),
 					boost::bind(&connection::http_server_write, shared_from_this(),
					ba::placeholders::error,
					ba::placeholders::bytes_transferred));
}

void connection::http_server_write(const bs::error_code& err, size_t len) 
{
	if (err)
	{
		shutdown(err);
		return;
	}

	stat.hdr_bytes_srv_write += len;

	if (req.get_contentlen() > 0)
	{
		if (brw_buff_lastlen > brw_buff_endpos)
		{
			size_t t = brw_buff_lastlen - brw_buff_endpos;

			ba::async_write(srv_sock, ba::buffer(brw_buff.begin() + brw_buff_endpos, t),
					boost::bind(&connection::http_server_write_postdata,
					shared_from_this(),
					ba::placeholders::error,
					ba::placeholders::bytes_transferred));
		}
		else
		{
			bs::error_code ec;
			ec.clear();

			http_server_write_postdata(ec, 0);
		}
	}
	else
	{
		async_read(srv_sock, ba::buffer(srv_buff), ba::transfer_at_least(1),
				boost::bind(&connection::http_server_read_headers,
							shared_from_this(),
							ba::placeholders::error,
							ba::placeholders::bytes_transferred));
	}
}

void connection::http_browser_read_postdata_in_webservermode(const bs::error_code& err, size_t len)
{
	int postdata_contentlen = req.get_contentlen();
	if (postdata_contentlen > 0)
	{
		if (brw_buff_lastlen > brw_buff_endpos)
		{
			// kazak
			size_t t = brw_buff_lastlen - brw_buff_endpos;
			unsigned char *p = (unsigned char *)brw_buff.begin() + brw_buff_endpos;
			std::copy(p, p + t, std::back_inserter(req.postdata_raw));
			brw_buff_lastlen = 0;
			brw_buff_endpos = 0;
		}

		if (len > 0)
		{
			unsigned char *p = (unsigned char *)brw_buff.begin();
			std::copy(p, p + len, std::back_inserter(req.postdata_raw));
		}

		int postdata_size = (int)req.postdata_raw.size();
		if (postdata_contentlen <= postdata_size)
		{
			start_web_server();
		}
		else
		{
			async_read(brw_sock, ba::buffer(brw_buff), ba::transfer_at_least(1),
				boost::bind(&connection::http_browser_read_postdata_in_webservermode,
				shared_from_this(),
				ba::placeholders::error,
				ba::placeholders::bytes_transferred));
		}
	}
	else
	{
		start_web_server();
	}
}

void connection::http_server_write_postdata(const bs::error_code& err, size_t len) 
{
	if (err)
	{
		shutdown(err);
		return;
	}

	stat.post_bytes_srv_write += len;

	if ((req.get_contentlen() > 0) && (stat.post_bytes_srv_write < (unsigned int)req.get_contentlen()))
	{
		async_read(brw_sock, ba::buffer(brw_buff), ba::transfer_at_least(1),
				boost::bind(&connection::http_browser_read_postdata,
						shared_from_this(),
						ba::placeholders::error,
						ba::placeholders::bytes_transferred));
	}
	else
	{
		async_read(srv_sock, ba::buffer(srv_buff), ba::transfer_at_least(1),
				boost::bind(&connection::http_server_read_headers,
						shared_from_this(),
						ba::placeholders::error,
						ba::placeholders::bytes_transferred));
	}
}

void connection::http_browser_read_postdata(const bs::error_code& err, size_t len) 
{
	if (debug_http)
	{
		record_data(brw_buff.data(), len, RECORD_DATA_NOPOSTFIX);
		record_message("http_browser_read_postdata", len, err);
	}

	if (err)
	{
		shutdown(err);
		return;
	}

	stat.post_bytes_brw_read += len;

	ba::async_write(srv_sock, ba::buffer(brw_buff, len),
			boost::bind(&connection::http_server_write_postdata,
			shared_from_this(),
			ba::placeholders::error,
			ba::placeholders::bytes_transferred));
}

void connection::http_server_read_headers(const bs::error_code& err, size_t len) 
{
	if (debug_http)
	{
		record_data(srv_buff.data(), len, RECORD_DATA_NOPOSTFIX);
		record_message("http_server_read_headers", len, err);
	}

	if (err)
	{
		shutdown(err);
		return;
	}

	size_t processed_len = 0;
	bool end_found = false;

	resp.original = process_until_header_end(resp.original, srv_buff.data(), len, &processed_len, &end_found);

	if (processed_len == len)
	{
		stat.hdr_bytes_srv_read += len;
	}
	else
	{
		stat.hdr_bytes_srv_read += processed_len;
		stat.body_bytes_srv_read += len - processed_len;
	}

	srv_buff_lastlen = len;
	srv_buff_endpos = processed_len;

	if (!end_found) 
	{ 
		// going to read rest of headers

		async_read(srv_sock, ba::buffer(srv_buff), ba::transfer_at_least(1),
					boost::bind(&connection::http_server_read_headers,
								shared_from_this(),
								ba::placeholders::error,
								ba::placeholders::bytes_transferred));
	} 
	else 
	{ 
		if (!resp.parse())
		{
			shutdown();
			return;
		}

		resp.request = &req;

		is_persistent = (
			((req.version == "1.1" && req.conntype != request_http_headers::connection_close) ||
			(req.version == "1.0" && req.conntype == request_http_headers::connection_keepalive)) &&
			((resp.version == "1.1" && req.conntype != request_http_headers::connection_close) ||
			(resp.version == "1.0" && req.conntype == request_http_headers::connection_keepalive))
			);
			
		ba::async_write(brw_sock, ba::buffer(resp.original),
						boost::bind(&connection::http_browser_start_write,
						shared_from_this(),
						ba::placeholders::error,
						ba::placeholders::bytes_transferred));
	}
}

void connection::http_server_read_body(const bs::error_code& err, size_t len) 
{
	if (debug_http)
	{
		bool eof = err == ba::error::eof;
		const char *m = eof ? "http_server_read_body +eof" : "http_server_read_body";

		record_data(srv_buff.data(), len, RECORD_DATA_NOPOSTFIX);
		record_message(m, len, err);
	}

	if (!err || err == ba::error::eof)
	{
		if (resp.response_code == 304)
		{
			shutdown();
			return;
		}

		stat.body_count_srv_read++;
		stat.body_bytes_srv_read += len;

		bool body_result = false;
		try
		{
			body_result = resp.process_rawbody(srv_buff.data(), len);
		}
		catch(std::exception& e)
		{
			std::ostringstream oss;
			oss << "body processing error: " << e.what();
			std::string exmsg = oss.str();
// TODO
//			eventlogger *elog = eventlogger::instance();
//			elog->adderror(exmsg.c_str());
		}
		
		if (!body_result)
		{
			bool use_gzip = resp.content_encoding == response_http_headers::encoding_gzip;
			send_deny_response(proxymsg::msgtype_block, req.url.host.c_str(), NULL, false, use_gzip);
		}
		else
		{
			if (err == ba::error::eof)
				proxy_closed = true;

			if ((proxy_closed) && (len == 0))
			{
				if (resp.is_html)
				{
					http_browser_start_write_html();
				}
				else
				{
					shutdown();
				};
				return;
			}
			else
			{
				if (!resp.is_html)
				{
					ba::async_write(brw_sock, ba::buffer(srv_buff, len),
							boost::bind(&connection::http_browser_write,
							shared_from_this(),
							ba::placeholders::error,
							ba::placeholders::bytes_transferred));
				}
				else
				{
					http_browser_write();
				}
			}
		}
	} 
	else 
	{
		shutdown();
	}
}

void connection::http_browser_start_write(const bs::error_code& err, size_t len) 
{
	if (debug_http)
	{
		record_data(srv_buff.data(), len, RECORD_DATA_HTTP_BROWSER );
		record_message("http_browser_start_write", len, err);
	}

	if (err)
	{
		shutdown(err);
		return;
	}

	stat.hdr_bytes_brw_write += len;

	if (srv_buff_lastlen > srv_buff_endpos)
	{
		size_t t = srv_buff_lastlen - srv_buff_endpos;

		bool body_result = resp.process_rawbody(srv_buff.data()  + srv_buff_endpos, t);

		if (!body_result)
		{
			bool use_gzip = resp.content_encoding == response_http_headers::encoding_gzip;
			send_deny_response(proxymsg::msgtype_block, req.url.host.c_str(), NULL, false, use_gzip);
		}
		else
		{
			if (!resp.is_html)
			{
				ba::async_write(brw_sock, ba::buffer(srv_buff.begin() + srv_buff_endpos, t),
							boost::bind(&connection::http_browser_write,
							shared_from_this(),
							ba::placeholders::error,
							ba::placeholders::bytes_transferred));
			}
			else
			{
				http_browser_write();
			}
		}
	}
	else
	{
		http_browser_write();
	}
}

void connection::http_browser_write()
{
	bs::error_code ec;
	ec.clear();

	http_browser_write(ec, 0);
}

void connection::http_browser_write(const bs::error_code& err, size_t len) 
{
	if (debug_http)
	{
		std::ostringstream oss;
		oss << "http_browser_write, resp.get_contentlen() = " << resp.get_contentlen();
		oss << ", resp.transfer_encoding = " << resp.transfer_encoding;
		oss << ", resp.got_last_chunk = " << resp.got_last_chunk;

		record_data(srv_buff.data(), len, RECORD_DATA_HTTP_BROWSER );
		record_message(oss.str().c_str(), len, err);
	}

	if (err)
	{
		shutdown(err);
		return;
	}

	if (resp.response_code == 304)
	{
		shutdown();
		return;
	}

	stat.body_bytes_brw_write += len;

	if ((resp.get_contentlen() == -1 || stat.body_bytes_srv_read < (unsigned int)resp.get_contentlen()) &&
		(resp.transfer_encoding != response_http_headers::encoding_chunked || !resp.got_last_chunk))
	{
		async_read(srv_sock, ba::buffer(srv_buff), ba::transfer_at_least(1),
					boost::bind(&connection::http_server_read_body,
								shared_from_this(),
								ba::placeholders::error,
								ba::placeholders::bytes_transferred));
	}
	else 
	{
		if ((resp.is_html) && (stat.body_bytes_srv_read > 0))
		{
			http_browser_start_write_html();
		}
		else
		{
			shutdown();
		}

//		if (isPersistent) // do additional checks for persistent connections
	}
}

//
//  Send html page to browser.
//  Html pages are processed by special way: 
//  First step: html page is received completely into internal buffer.
//  Second step: html page is analyzed (looking for "stop words").
//  Third step: html page is send back to browser.
//
void connection::http_browser_start_write_html() 
{
	if (debug_http)
	{
		const bs::error_code err = ba::error::eof;
		record_message("http_browser_start_write_html", resp.response_raw.size(), err);
	}

	size_t resp_len = resp.response_raw.size();

	size_t size_to_write = BUFFER_SIZE;
	if (BUFFER_SIZE > resp_len)
		size_to_write = resp_len;

#if defined(_MSC_VER)
	char *srvbuf = srv_buff.begin();
	size_t srvbufsize = srv_buff.size();
	std::copy(resp.response_raw.begin(), resp.response_raw.begin() + size_to_write, stdext::checked_array_iterator<char *>(srvbuf, srvbufsize));
#else
	std::copy(resp.response_raw.begin(), resp.response_raw.begin() + size_to_write, srv_buff.begin());
#endif

	ba::async_write(brw_sock, ba::buffer(srv_buff, size_to_write),
				boost::bind(&connection::http_browser_write_html,
				shared_from_this(),
				ba::placeholders::error,
				ba::placeholders::bytes_transferred));
}

void connection::http_browser_write_html(const bs::error_code& err, size_t len)
{
	if (debug_http)
	{
		record_data(srv_buff.data(), len, RECORD_DATA_HTTP_BROWSER );
		record_message("http_browser_write_html", len, err);
	}

	if (err)
	{
		shutdown(err);
		return;
	}

	stat.body_bytes_brw_write += len;

	size_t total_resp_len = resp.response_raw.size();
	if (stat.body_bytes_brw_write < total_resp_len)
	{
		size_t size_to_write = BUFFER_SIZE;

		size_t beginpos = stat.body_bytes_brw_write;
		if (BUFFER_SIZE > (total_resp_len - beginpos))
			size_to_write = total_resp_len - beginpos;

		size_t endpos = stat.body_bytes_brw_write + size_to_write;

#if defined(_MSC_VER)
		char *srvbuf = srv_buff.begin();
		size_t srvbufsize = srv_buff.size();
		std::copy(resp.response_raw.begin() + beginpos, resp.response_raw.begin() + endpos, stdext::checked_array_iterator<char *>(srvbuf, srvbufsize));
#else
		std::copy(resp.response_raw.begin() + beginpos, resp.response_raw.begin() + endpos, srv_buff.begin());
#endif

		ba::async_write(brw_sock, ba::buffer(srv_buff, size_to_write),
				boost::bind(&connection::http_browser_write_html,
				shared_from_this(),
				ba::placeholders::error,
				ba::placeholders::bytes_transferred));
	}
	else
	{
		// kazak
		if (resp.keep_connection)
		{
			start();
		}
		else
		{
			shutdown();
		}
	}
}

void connection::shutdown() 
{
	shutdown(false);
}

void connection::shutdown(const bs::error_code& err)
{
	shutdown(err == ba::error::eof);
}

void connection::shutdown(bool is_graceful)
{
	if (is_graceful)
	{
		bs::error_code ec_srv;
		srv_sock.shutdown(ba::ip::tcp::socket::shutdown_both, ec_srv);
	}
	srv_sock.close();

	if (is_graceful)
	{
		bs::error_code ec_brw;
		brw_sock.shutdown(ba::ip::tcp::socket::shutdown_both, ec_brw);
	}
	brw_sock.close();
}

std::string connection::process_until_header_end(const std::string& existed_data, 
	char *databuff, size_t len, size_t *processed_len, bool *end_found)
{
	std::ostringstream oss;
	oss << existed_data;

	std::string delim("\r\n");

	char *begin = databuff;
	char *end = begin + len;
	char *pos = std::find_first_of(begin, end, delim.begin(), delim.end());

	size_t processed = 0;
	bool found = false;
		
	if (pos != NULL)
	{
		char c = 0;
		while (pos != end)
		{
			if (pos - begin > 1)
			{
				processed += pos - begin + 1;
				std::string str = std::string(begin, pos - begin + 1);
				oss << str;
			}
			else
			{
				processed++;
				c = *pos;
				oss << c;
			}

			if (c == '\r' || c == '\n')
			{
				std::string hdr = oss.str();
				if (hdr.find("\r\n\r\n") != std::string::npos)
				{
					found = true;
					break;
				}

				if (hdr.find("\r\r") != std::string::npos)
				{
					found = true;
					break;
				}

				if (hdr.find("\n\n") != std::string::npos)
				{
					found = true;
					break;
				}
			}

			begin = pos;
			pos++;
		}
	}
	else
	{
		processed = len;
		std::string str = std::string(begin, len);
		oss << str;
	}

	*processed_len = processed;
	*end_found = found;
	std::string retval = oss.str();

	return retval;
}

//
//  HTTPS related
//

void connection::ssl_start_connect()
{
	ba::ip::tcp::resolver::query query(req.get_hostname(), req.get_hostport());
	resolver_.async_resolve(query,
							boost::bind(&connection::ssl_handle_resolve, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::iterator));
}

void connection::ssl_handle_resolve(const boost::system::error_code& err, ba::ip::tcp::resolver::iterator endpoint_iterator)
{
	if (err)
	{
		shutdown(err);
		return;
	}

	ba::ip::tcp::endpoint endpoint = *endpoint_iterator;
	resp.server_addr = endpoint.address();

	srv_sock.async_connect(endpoint,
 				boost::bind(&connection::ssl_handle_connect, shared_from_this(),
				boost::asio::placeholders::error,
				++endpoint_iterator));
}

void connection::ssl_handle_connect(const boost::system::error_code& err, ba::ip::tcp::resolver::iterator endpoint_iterator)
{
	if (!err)
	{
		std::string resp = "HTTP/1.0 200 Connection established\r\n\r\n";
		std::string::size_type len = resp.length();

#if defined(_MSC_VER)
		char *srvbuf = srv_buff.begin();
		size_t srvbufsize = srv_buff.size();
		std::copy(resp.begin(), resp.end(), stdext::checked_array_iterator<char *>(srvbuf, srvbufsize));
#else
		std::copy(resp.begin(), resp.end(), srv_buff.begin());
#endif

		ba::async_write(brw_sock, ba::buffer(srv_buff, len),
					boost::bind(&connection::ssl_start_reading,
					shared_from_this(),
					ba::placeholders::error,
					ba::placeholders::bytes_transferred));
	}
	else
	if (endpoint_iterator != ba::ip::tcp::resolver::iterator()) 
	{
		srv_sock.close();
		ba::ip::tcp::endpoint endpoint = *endpoint_iterator;
		srv_sock.async_connect(endpoint,
					boost::bind(&connection::ssl_handle_connect, shared_from_this(),
					boost::asio::placeholders::error,
					++endpoint_iterator));
	
	} 
	else 
	{
		shutdown(err);
	}
}

void connection::ssl_start_reading(const bs::error_code& err, size_t len)
{
	if (err)
	{
		shutdown(err);
		return;
	}

	stat.hdr_bytes_brw_write += len;

	async_read(brw_sock, ba::buffer(brw_buff), ba::transfer_at_least(1),
				boost::bind(&connection::ssl_browser_read,
				shared_from_this(),
				ba::placeholders::error,
				ba::placeholders::bytes_transferred));

	async_read(srv_sock, ba::buffer(srv_buff), ba::transfer_at_least(1),
				boost::bind(&connection::ssl_server_read,
				shared_from_this(),
				ba::placeholders::error,
				ba::placeholders::bytes_transferred));
}

void connection::ssl_browser_read(const bs::error_code& err, size_t len)
{
	if (debug_http)
	{
		record_data(brw_buff.data(), len, RECORD_DATA_FROMCLIENT);
		record_message("ssl_browser_read", len, err);
	}

	if (err)
	{
		shutdown(err);
		return;
	}

	stat.post_bytes_brw_read += len;

	ba::async_write(srv_sock, ba::buffer(brw_buff,len),
					boost::bind(&connection::ssl_server_write,
					shared_from_this(),
					ba::placeholders::error,
					ba::placeholders::bytes_transferred));
}

void connection::ssl_server_write(const bs::error_code& err, size_t len)
{
	if (err)
	{
		shutdown(err);
		return;
	}

	stat.post_bytes_srv_write += len;

	async_read(brw_sock, ba::buffer(brw_buff), ba::transfer_at_least(1),
				boost::bind(&connection::ssl_browser_read,
			    shared_from_this(),
			    ba::placeholders::error,
			    ba::placeholders::bytes_transferred));
}

void connection::ssl_server_read(const bs::error_code& err, size_t len)
{
	if (debug_http)
	{
		record_data(srv_buff.data(), len, RECORD_DATA_FROMSERVER);
		record_message("ssl_server_read", len, err);
	}

	if (err)
	{
		shutdown(err);
		return;
	}

	stat.body_bytes_srv_read += len;

	ba::async_write(brw_sock, ba::buffer(srv_buff, len),
					boost::bind(&connection::ssl_browser_write,
					shared_from_this(),
					ba::placeholders::error,
					ba::placeholders::bytes_transferred));
}

void connection::ssl_browser_write(const bs::error_code& err, size_t len)
{
	if (err)
	{
		shutdown(err);
		return;
	}

	stat.body_bytes_brw_write += len;

	async_read(srv_sock, ba::buffer(srv_buff), ba::transfer_at_least(1),
				boost::bind(&connection::ssl_server_read,
				shared_from_this(),
				ba::placeholders::error,
				ba::placeholders::bytes_transferred));
}

}

#endif