#ifndef HTTP_HEADERS_H
#define HTTP_HEADERS_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <vector>
#include <map>
#include <list>

#include <boost/asio/ip/address.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <stringtools.h>

#include "addrurl.h"
#include "html_document.h"
//#include "proxyrequest_params.h"

#define HTTP_RESPONSE_200 200
#define HTTP_RESPONSE_401 401
#define HTTP_RESPONSE_404 404
#define HTTP_RESPONSE_500 500
#define HTTP_RESPONSE_501 501

namespace utm {

//typedef bool (__cdecl * process_request_ptr)(const boost::asio::ip::address &,const addrurl &,const html_document &);

struct chunk
{
	unsigned int datasize;
	unsigned int headersize;
	unsigned int footersize;
};

typedef std::vector<chunk> chunk_container;

struct header_field
{
public:
	header_field(const std::string& _key, const std::string& _name, const std::string& _value) : key(_key), name(_name), value(_value) { };

	header_field(const char* _name, const char* _value)
	{
		key.assign(_name);
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);

		name.assign(_name);
		value.assign(_value);
	}

	header_field(const std::string& _name, const std::string& _value)
	{
		key = _name;
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);

		name = _name;
		value = _value;
	};

	std::string key;
	std::string name;
	std::string value;
};

typedef std::list<header_field> header_fields_container;
typedef std::list<header_field>::iterator header_fields_iterator;

class http_headers
{
public:
	http_headers();
	virtual ~http_headers();

	void clear();

	size_t original_header_size() const { return original.size(); };
	std::string original;		// original http request or response header
	std::string first_line;		// the first line of request or response header
	std::string version;		// both for request and response

	header_fields_container headers_set;	// set of header like "Key/Value"

	std::string find_header(const char *header_key, bool tolower) const;
	const char* find_header(const char *header_key) const;

	template<typename T>
	T get_header(const char* header_key, const T& default_value) const
	{
		const char *p = find_header(header_key);
		if (p != NULL)
		{
			try
			{
				T retval = boost::lexical_cast<T>(p);
				return retval;
			}
			catch (const boost::bad_lexical_cast&)
			{
			}
		}

		return default_value;
	}

	void set_header(const header_field& hf);
	void remove_header(const char *header_key);

	int get_contentlen() const { return contentlen; };

protected:
	bool parse_headers();
	int contentlen;
};

class request_http_headers : public http_headers
{
public:
	request_http_headers();

	enum request_types { request_unknown = 0, http_thru_proxy = 1, https_thru_proxy = 2, direct_http = 3 };
	enum connection_types { connection_unknown = 0, connection_close = 1, connection_keepalive = 2 };

	request_types reqtype;
	connection_types conntype;
	boost::asio::ip::address client_addr;
	unsigned short client_port;

	std::string method;
	std::string original_url;
	std::string version;
	addrurl url;
	addrurl referer;
	std::vector<unsigned char> postdata_raw;

	std::string request_for_server;

	bool parse();
	void clear();
	const char *get_hostname();
	const char *get_hostport();
	
	static void decode_basic_authorization(const char *auth_header, std::string& username, std::string& password);

private:
	void make_request_for_server();
};

class response_http_headers : public http_headers
{
private:
	static const std::string chunk_header_allowed_symbols;

public:
	response_http_headers();

	enum content_transfer_encodings { encoding_none = 0, encoding_chunked = 1, 
		encoding_compress = 2, encoding_deflate = 3, encoding_gzip = 4, 
		encoding_identity = 5, encoding_unknown = 6 };

	content_transfer_encodings transfer_encoding;
	content_transfer_encodings content_encoding;

	boost::asio::ip::address server_addr;
	int response_code;

	std::string mime_type;
	std::string charset;
	bool is_html;
	bool keep_connection;

	request_http_headers *request;

//	boost::function<bool (proxyrequest_params*)> func_process_request;

	void clear();
	bool parse();
	bool parse_raw_response(const std::string& response);
	bool parse_raw_response(const char* response, size_t len);

	bool process_rawbody(const char *begin, size_t len);
	bool add_rawbody(const char *body, size_t len, std::vector<unsigned char>& data);
	int uncompress_gzip(const std::vector<unsigned char>& data);
	bool parse_html();

	bool in_chunk;
	size_t current_chunk_pos;
	std::string current_chunk_header;
	bool got_last_chunk;

	chunk_container chunks;
	unsigned int chunks_total_length;
	void addchunk(unsigned int datasize, unsigned int headersize, unsigned int footersize);

	std::vector<unsigned char> response_raw;
	std::vector<unsigned char> response_unchunked;
	std::string response_uncompressed;
	html_document doc;

	template<typename T>
	static void create_http_response(const char *response_code, const char *content_type, const char *wwwauth, bool keep_connection, const char *data, size_t datalen, T& response);
	static void create_http_response(int response_code, response_http_headers *req, const char *wwwauth = NULL);

#ifdef UTM_DEBUG
	static void test_all();
#endif

private:
	static content_transfer_encodings parse_content_transfer_encoding(const std::string& encoding)
	{
		const char *p = encoding.c_str();

		if (strcmp(p, "") == 0)
			return response_http_headers::encoding_none;

		if (strcmp(p, "chunked") == 0)
			return response_http_headers::encoding_chunked;

		if (strcmp(p, "compress") == 0)
			return response_http_headers::encoding_compress;

		if (strcmp(p, "deflate") == 0)
			return response_http_headers::encoding_deflate;

		if (strcmp(p, "gzip") == 0)
			return response_http_headers::encoding_gzip;

		if (strcmp(p, "identity") == 0)
			return response_http_headers::encoding_identity;

		return response_http_headers::encoding_unknown;
	}
};

}

#endif // HTTP_HEADERS_H
