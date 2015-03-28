#include "stdafx.h"
#include "http_headers.h"

#include <algorithm> 
#include <functional> 
#include <locale>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <base64.h>

#define GZIP_CHUNK 16384
#define ZLIB_WINAPI

#ifdef _MSC_VER
#define ZLIB_WINAPI
#endif

#include <zlib.h>

#define CRLF "\r\n"
#define LAST_CHUNK_SIGNATURE "0\r\n\r\n"
#define HEADER_DELIMITER ": "
#define HEADER_CONTENT_LENGTH "content-length"
#define HEADER_CONNECTION "connection"
#define HEADER_PROXY_CONNECTION "proxy-connection"
#define HEADER_HOST "host"
#define HEADER_REFERER "referer"
#define HEADER_USERAGENT "user-agent"
#define HTTP_PREFIX "HTTP/"
#define DEFAULT_HTTP_PORT "80"
#define DEFAULT_HTTPS_PORT "443"
#define HTTP_METHOD_CONNECT "CONNECT"

namespace utm {

//process_request_ptr response_http_headers::process_request = NULL;
const std::string response_http_headers::chunk_header_allowed_symbols = std::string("0123456789abcdefABCDEF\r\n");

http_headers::http_headers(void)
{
	clear();
}

http_headers::~http_headers(void)
{
}

void http_headers::clear()
{
	headers_set.clear();

	original.clear();
	first_line.clear();
	version.clear();

	contentlen = -1;
}

bool http_headers::parse_headers()
{
	std::vector<std::string> tokens;
	boost::split(tokens, original, boost::is_any_of(CRLF));

	std::vector<std::string>::iterator iter;
	for (iter = tokens.begin(); iter != tokens.end(); ++iter)
	{
		if ((*iter).empty())
			continue;

		if (iter == tokens.begin())
		{
			first_line = (*iter);
		}
		else
		{
			std::string::size_type delimiter_pos = iter->find(HEADER_DELIMITER);
			if (delimiter_pos != std::string::npos)
			{
				std::string name = iter->substr(0, delimiter_pos);
				std::string value = iter->substr(delimiter_pos + sizeof(HEADER_DELIMITER) - 1);

				boost::algorithm::trim(name);
				boost::algorithm::trim(value);

				if (!name.empty() && !value.empty())
				{
					header_field h(name, value);
					headers_set.push_back(h);

					if (h.key == "content-length")
						contentlen = atoi(h.value.c_str());
				}
			}
		}
	}

	return true;
}

std::string http_headers::find_header(const char *header_key, bool tolower) const
{
	std::string retval;

	const char *value = find_header(header_key);
	if (value != NULL)
	{
		retval.assign(value);
		if (tolower)
		{
			std::transform(retval.begin(), retval.end(), retval.begin(), ::tolower);
		}
	}

	return retval;
}

const char* http_headers::find_header(const char *header_key) const
{
	header_fields_container::const_iterator it;
	for (it = headers_set.begin(); it != headers_set.end(); ++it)
	{
		if (strcmp(it->key.c_str(), header_key) == 0)
		{
			return it->value.c_str();
		}
	}

	return NULL;
}

void http_headers::set_header(const header_field& hf)
{
	header_fields_container::iterator it;
	for (it = headers_set.begin(); it != headers_set.end(); ++it)
	{
		if (it->key == hf.key)
		{
			it->value = hf.value;
			return;
		}
	}

	headers_set.push_back(hf);
}

void http_headers::remove_header(const char *header_key)
{
	header_fields_container::iterator it;
	for (it = headers_set.begin(); it != headers_set.end(); ++it)
	{
		if (strcmp(it->key.c_str(), header_key) == 0)
		{
			headers_set.erase(it);
			break;
		}
	}
}

request_http_headers::request_http_headers()
{
	clear();
}

void request_http_headers::clear()
{
	http_headers::clear();

	reqtype = request_unknown;
	conntype = connection_unknown;

	client_port = 0;
	method.clear();
	original_url.clear();
	postdata_raw.clear();

	request_for_server.clear();
}

bool request_http_headers::parse()
{
	if (!parse_headers())
		return false;

	boost::regex regex_first_line("(\\w{1,10})\\s(.*?)\\sHTTP/(\\d.\\d)");
	boost::smatch match_first_line;

	if (boost::regex_search(first_line, match_first_line, regex_first_line, boost::match_extra)) 
	{
		method = match_first_line[1].str();
		original_url = match_first_line[2].str();
		version = match_first_line[3].str();
	}

	if (method != HTTP_METHOD_CONNECT)
	{
		if (strncmp(original_url.c_str(), "http://", 7) == 0)
		{
			reqtype = http_thru_proxy;
		}
		else
		{
			reqtype = direct_http;
		}
	}
	else
	{
		boost::regex regex_https("(.*?):443");
		boost::smatch m;
	
		if (boost::regex_search(original_url, m, regex_https, boost::match_extra)) 
		{
			url.parse(original_url);
			reqtype = https_thru_proxy;
		}
		else
			return false;
	}

	if (reqtype == http_thru_proxy || reqtype == https_thru_proxy)
	{
		url.parse(original_url);
	}

	if (reqtype == direct_http)
	{
		std::ostringstream oss;
		oss << "http://" << get_hostname() << original_url;
		url.parse(oss.str());
	}

	make_request_for_server();

	{
		std::string conn = find_header("connection", true);
		if (conn == "close")
			conntype = connection_close;

		if (conn == "keepalive")
			conntype = connection_keepalive;
	}

	{
		std::string ref = find_header("referer", true);
		referer.parse(ref);
	}

	return true;
}

const char *request_http_headers::get_hostname()
{
	if (reqtype == direct_http)
	{
		const char *h = find_header("host");
		return h == NULL ? "" : h;
	}

	return url.host.c_str();
}

const char *request_http_headers::get_hostport()
{
	if (method == HTTP_METHOD_CONNECT)
		return "443";

	return url.get_port();
}

void request_http_headers::decode_basic_authorization(const char *auth_header, std::string& username, std::string& password)
{
	if ((auth_header != NULL) && (strncmp(auth_header, "Basic ", 6) == 0))
	{
		size_t pos = 6;
		while (*(auth_header + pos) == ' ')
			pos++;

		std::string decoded_authdata;
		utm::base64::decode(auth_header + pos, decoded_authdata, false);
		size_t userpass_delimiter_pos = decoded_authdata.find(':');
		if (userpass_delimiter_pos != std::string::npos)
		{
			username = decoded_authdata.substr(0, userpass_delimiter_pos);
			password = decoded_authdata.substr(++userpass_delimiter_pos);
		}
	}
}

void request_http_headers::make_request_for_server()
{
	bool keep_connection = false;

	if (reqtype != https_thru_proxy)
	{
		if (!keep_connection)
		{
			header_field hc("Connection", "close");
			set_header(hc);
		}

		remove_header("proxy-connection");
	}

	std::ostringstream oss;
	oss << method << " ";

	if (reqtype == http_thru_proxy)
	{
		oss << url.path;
		if (!url.query.empty())
			oss << "?" << url.query;
			
		oss << " HTTP/1.1";
	}

	if (reqtype == direct_http)
	{
		oss << original_url << " HTTP/" << version;
	}

	oss << "\r\n";

	header_fields_container::iterator it;
	for (it = headers_set.begin(); it != headers_set.end(); ++it)
	{
		oss << it->name << ": " << it->value << "\r\n";
	}
	oss << "\r\n";

	request_for_server = oss.str();
}

response_http_headers::response_http_headers()
{
//	func_process_request = NULL;
	clear();
}

void response_http_headers::clear()
{
	http_headers::clear();

	response_code = 0;
	request = NULL;

	transfer_encoding = encoding_none;
	content_encoding = encoding_none;

	got_last_chunk = false;
	in_chunk = false;
	current_chunk_pos = 0;
	chunks.clear();
	chunks_total_length = 0;

	mime_type.clear();
	charset.clear();
	response_raw.clear();
	response_unchunked.clear();
	response_uncompressed.clear();
	is_html = false;
	keep_connection = false;
}

bool response_http_headers::parse()
{
	if (!parse_headers())
		return false;

	// Parse the first line of HTTP response ("HTTP/1.1 200 OK")

	boost::regex regex_first_line("HTTP/(\\d\\.\\d)\\s+(\\d\\d\\d)\\s+(.*)");
	boost::smatch match_first_line;

	if (!boost::regex_search(first_line, match_first_line, regex_first_line, boost::match_extra)) 
		return false;

	version = match_first_line[1].str();
	response_code = atoi(match_first_line[2].str().c_str());

	// Parse other HTTP response headers

	transfer_encoding = parse_content_transfer_encoding(find_header("transfer-encoding", true));
	content_encoding = parse_content_transfer_encoding(find_header("content-encoding", true));
	
	std::string content_type = find_header("content-type", true);
	boost::regex regex_content_type("(.*?);\\s*charset=(.*)");
	boost::smatch match_content_type;
	
	if (boost::regex_search(content_type, match_content_type, regex_content_type, boost::match_extra)) 
	{
		mime_type = match_content_type[1].str();
		charset = match_content_type[2].str();

		if (mime_type == "text/html")
			is_html = true;
	}
	else
	{
		mime_type = content_type;
	}

	return true;
}

bool response_http_headers::parse_raw_response(const std::string& response)
{
	return parse_raw_response(response.c_str(), response.size());
}

bool response_http_headers::parse_raw_response(const char* response, size_t len)
{
	const char *crlf = strstr(response, "\r\n\r\n");
	if (crlf == NULL)
	{
		return false;
	}

	size_t hdrlen = (crlf - response) + 4 /* 4 is sizeof "\r\n\r\n" */;
	original.resize(hdrlen);
	char *hdr = const_cast<char *>(original.c_str());
	strncpy(hdr, response, hdrlen);

	return parse();
}

void response_http_headers::addchunk(unsigned int datasize, unsigned int headersize, unsigned int footersize)
{
	chunk c;
	c.datasize = datasize;
	c.headersize = headersize;
	c.footersize = footersize;

	chunks.push_back(c);

	chunks_total_length += datasize;
	chunks_total_length += headersize;
	chunks_total_length += footersize;
}

bool response_http_headers::process_rawbody(const char *begin, size_t len)
{
	bool result_rawbody = add_rawbody(begin, len, response_raw);

	if (transfer_encoding != response_http_headers::encoding_chunked)
		return result_rawbody;

	size_t pos = 0;
	size_t crlf_pos;
	char c;

	while (pos < len)
	{
		if (in_chunk)
		{
			chunk current_chunk = chunks.back();
				
			if (current_chunk.datasize < current_chunk_pos + len)
			{
				in_chunk = false;
				current_chunk_header.clear();

				size_t chunk_lastsize = current_chunk.datasize - current_chunk_pos;
				
				size_t actual_datasize = len - pos;
				size_t copylen = chunk_lastsize;

				if (copylen > actual_datasize)
				{
					copylen = actual_datasize;
				}

				if (!add_rawbody(begin + pos, copylen, response_unchunked))
					return false;

				pos += copylen;
				continue;
			}
			else
			{
				current_chunk_pos += (len - pos);
				return add_rawbody(begin + pos, len - pos, response_unchunked);
				break;
			}
		}
		else
		{
			c = *(begin + pos);

			if ((current_chunk_header.size() != 0) || (c != '\r' && c != '\n'))
			{
				if (chunk_header_allowed_symbols.find(c) != std::string::npos)
				{
					current_chunk_header.push_back(c);
				}
			}

			pos++;

			crlf_pos = current_chunk_header.find(CRLF);
			if (crlf_pos != std::string::npos)
			{
				if (current_chunk_header.find(LAST_CHUNK_SIGNATURE) != std::string::npos)
				{
					got_last_chunk = true;
					addchunk(0, sizeof(LAST_CHUNK_SIGNATURE) - 1, 0);
					break;
				};

				// convert string with hex number into unsigned int

				unsigned int datasize;
				std::stringstream ss;
				ss << std::hex << current_chunk_header.substr(0, crlf_pos);
				ss >> datasize;

				if (datasize > 0)
				{
					addchunk(datasize, current_chunk_header.length(), (sizeof(CRLF) - 1));
					in_chunk = true;
					current_chunk_pos = 0;
				}
			}
		}
	}

	return true;
}

bool response_http_headers::add_rawbody(const char *body, size_t len, std::vector<unsigned char>& data)
{
	if ((body == NULL) || (len == 0) || (!is_html))
		return true;

	bool retval = true;

	{
		if (data.empty() && (contentlen > 0))
		{
			data.reserve(contentlen);
		}
		else
		{
			size_t datalen = data.size() + len;
			if (datalen > data.capacity())
			{
				data.reserve(datalen);
			}
		}

		std::copy(body,	body + len, std::back_inserter(data));

//		if ((data == &response_raw) && (transfer_encoding == response_http_headers::encoding_chunked))
		if ((&data == &response_raw) && (transfer_encoding == response_http_headers::encoding_chunked))
		{
			return true;
		}

		try
		{
			if (!doc.get_has_body_tag())
			{
				if ((content_encoding == encoding_gzip))
					uncompress_gzip(data);

				if ((content_encoding == encoding_none) || (content_encoding == encoding_identity))
					response_uncompressed.append(body, len);

				retval = parse_html();
			}
		}
		catch(std::exception&)
		{

		}
	}

	return retval;
}

int response_http_headers::uncompress_gzip(const std::vector<unsigned char>& data)
{
	response_uncompressed.clear();

    int ret;
    z_stream strm;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
	ret = inflateInit2(&strm, 16 + MAX_WBITS); 
	
	if (ret != Z_OK)
        return ret;

	size_t compressed_size = data.size();
	unsigned char *compressed_data = const_cast<unsigned char *>(data.data());
	unsigned char outbuf[GZIP_CHUNK];

	strm.avail_in = compressed_size;
	strm.next_in = compressed_data;

	do
	{
		strm.avail_out = GZIP_CHUNK;
		strm.next_out = outbuf;

		ret = inflate(&strm, Z_NO_FLUSH);
		if (ret == Z_STREAM_ERROR)
			break;

		size_t have = GZIP_CHUNK - strm.avail_out;

		std::string uncompressed((char *)outbuf, have);
		response_uncompressed += uncompressed;
	}
	while (strm.avail_out == 0);

	inflateEnd(&strm);

	return ret;
}

bool response_http_headers::parse_html()
{
	bool retval = true;
	doc.parse(charset, response_uncompressed);

	// TODO raise event
/*
	addrurl url(request->url_for_processing);

	if (func_process_request != NULL)
	{
		proxyrequest_params pp(request->client_addr, url, doc);
		retval = func_process_request(&pp);
	}
*/
	return retval;
}

template<typename T>
void response_http_headers::create_http_response(const char *response_code, const char *content_type, const char *wwwauth, bool keep_connection, const char *data, size_t datalen, T& response)
{
	std::ostringstream oss;
	oss << "HTTP/1.1 " << response_code << "\r\nContent type: " << content_type << "\r\n";

	if (wwwauth != NULL)
	{
		oss << "WWW-Authenticate: Basic realm=\"" << wwwauth << "\"\r\n";
	}

	oss << "Expires: Thu, 01 Dec 1994 16:00:00 GMT\r\n";
	oss << "Content-Length: " << datalen << "\r\n";

	if (!keep_connection)
		oss << "Connection: close\r\n\r\n";

	std::string hdr = oss.str();
	size_t hdrlen = hdr.size();

	response.reserve(hdrlen + datalen);
	response.resize(hdrlen + datalen);

	char *p = (char *)(response.data());
	memcpy(p, hdr.c_str(), hdrlen);

	if (datalen != 0 && data != NULL)
		memcpy(p + hdrlen, data, datalen);
}

void response_http_headers::create_http_response(int response_code, response_http_headers *req, const char *wwwauth)
{
	std::string codestr = boost::lexical_cast<std::string>(response_code);
	std::string bodystr;

	switch (response_code)
	{
		case HTTP_RESPONSE_401:
			codestr += " Authentication Required";
			bodystr = "<html><body><p>Valid username and password is needed.</p><p>HTTP 401 Authentication Required</p></body></html>";
			break;

		case HTTP_RESPONSE_404:
			codestr += " Not Found";
			bodystr = "<html><body><p>Unfortunately, the requested file is not found.</p><p>HTTP 404 Not Found</p></body></html>";
			break;

		case HTTP_RESPONSE_500:
			codestr += " Internal Server Error";
			bodystr = "<html><body><p>There are internal error on server side.</p><p>HTTP 500 Internal Server Error</p></body></html>";
			break;

		case HTTP_RESPONSE_501:
			codestr += " Not Implemented";
			bodystr = "<html><body><p>Method is not implemented.</p><p>HTTP 501 Not Implemented.</p></body></html>";
			break;
	}
	size_t bodystr_len = bodystr.size();

	create_http_response(codestr.c_str(), "text/html", wwwauth, req->keep_connection, bodystr.c_str(), bodystr_len, req->response_raw);
}

#ifdef UTM_DEBUG
void response_http_headers::test_all()
{
	std::string str;
	create_http_response("200 OK", "text/html", NULL, false, "testdata", 8, str);

	std::vector<char> str2;
	create_http_response("200 OK", "text/html", NULL, false, "testdata", 8, str2);

	std::vector<unsigned char> str3;
	create_http_response("200 OK", "text/html", NULL, false, "testdata", 8, str3);

	return;
}
#endif

}
