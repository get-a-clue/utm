#include "stdafx.h"
#include "http_client.h"

//#include "base64.h"

#include <boost/asio.hpp>
#include <boost/array.hpp>

namespace ba=boost::asio;
namespace bs=boost::system;

#define WEBSERVER_LOGROTATION_PATH "/admin/logrotate.html"

namespace utm {

http_client::http_client(void)
{
}


http_client::~http_client(void)
{
}

int http_client::do_query(const char *host, const int port, const char *httprequest, 
	std::string *response_header, std::string *response_data)
{
    ba::io_service io_service;
	ba::ip::tcp::resolver resolver(io_service);

	std::ostringstream ossport;
	ossport << port;

	ba::ip::tcp::resolver::query query(host, ossport.str());
    ba::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    ba::ip::tcp::resolver::iterator end;
 
    ba::ip::tcp::socket socket(io_service);
    bs::error_code error = ba::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
		socket.close();
		socket.connect(*endpoint_iterator++, error);
    }

    if (error)
		throw bs::system_error(error);
 
    ba::streambuf request;
	std::ostream request_stream(&request);
	request_stream << httprequest;

    ba::write(socket, request);
 
	// read the first line of HTTP header
    ba::streambuf responsebuf;
    ba::read_until(socket, responsebuf, "\r\n");
 
    std::istream response_stream(&responsebuf);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
 
	if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
		return 0;
    }
/*   
	if (status_code != 200)
    {
		// any reply code except 200 will be considered as error
		return 0;
    }
*/ 
    // Read until end of HTTP header
    ba::read_until(socket, responsebuf, "\r\n\r\n");

    std::string header;
    while (std::getline(response_stream, header) && header != "\r")
	{
		response_header->append(header);
	}

    // Read body of HTTP document
	int i = 0;
    while (ba::read(socket, responsebuf, ba::transfer_at_least(1), error))
	{
		i++;
	}

	size_t datasize = responsebuf.size();
	response_data->assign(ba::buffer_cast<const char*>(responsebuf.data()), datasize);

    if (error != ba::error::eof)
		throw bs::system_error(error);

	return status_code;
}

std::string http_client::rotate_logs(const int port, const std::string& webserver_hostname, const std::string& webserver_password)
{
	std::string retval;

	try
	{
		std::ostringstream ossauth;
		ossauth << "admin:" << webserver_password;

//		base64 b64;
//		std::istringstream ins(ossauth.str());
//		std::ostringstream ots;

//		b64.encode(ins, ots);

		std::ostringstream query;
		query << "GET http://" << webserver_hostname << ".com" << WEBSERVER_LOGROTATION_PATH;
		query << " HTTP/1.0\r\n";
//		query << "Authorization: Basic " << ots.str();
		query << "\r\n\r\n";

		std::string querystr = query.str();
		std::string header, body;

		int status_code = http_client::do_query("localhost", port, querystr.c_str(), &header, &body);
		std::ostringstream oss;
		oss << status_code;

		retval = oss.str();
	}
	catch(std::exception& ex)
	{
		retval = ex.what();
	}

	return retval;
}

}