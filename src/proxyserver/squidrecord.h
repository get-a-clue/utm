#pragma once

#include <ubase.h>
#include <event_defs.h>
#include <event_msg_base.h>

#include <boost/date_time/posix_time/posix_time.hpp>
namespace bt = boost::posix_time;

#include <string>
#include <iostream>
#include <iomanip>

namespace utm {

class squidrecord : public event_msg_base, public ubase
{
public:
	squidrecord();
	~squidrecord();

	// Squid native format
	// Timestamp Elapsed Client Action/Code Size Method URI Ident Hierarchy/From Content

	bt::ptime timestamp;		// The time when the request is completed (socket closed). The format is "Unix time" (seconds since Jan 1, 1970) with millisecond resolution.
	bt::time_duration elapsed;	// The elapsed time of the request, in milliseconds. This is the time between the accept() and close() of the client socket
	std::string client;			// The IP address of the connecting client
	std::string action;			// The Action describes how the request was treated locally (hit, miss, etc). 
	int code;					// The HTTP reply code taken from the first line of the HTTP reply header.
	size_t size;				// For TCP requests, the amount of data written to the client. (in bytes) 
	std::string method;			// The HTTP request method (GET, POST, etc)
	std::string uri;			// The requested URI.
	std::string ident;			// The result of the RFC931/ident lookup of the client username. If RFC931/ident lookup is disabled (default: `ident_lookup off'), it is logged as - .
	std::string hierarchy;		// A description of how and where the requested object was fetched.
	std::string from;			// Hostname of the machine where we got the object.
	std::string content;		// Content-type of the Object (from the HTTP reply header).

	std::string to_string(bool make_localization = false);

	void clear() { };
	void xml_create() { };
	void xml_catch_value(const char *keyname, const char *keyvalue) { };
	ubase* xml_catch_subnode(const char *name) { return NULL; };
};

}