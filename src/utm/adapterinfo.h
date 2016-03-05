#ifndef _UTM_ADAPTERINFO_H
#define _UTM_ADAPTERINFO_H

#pragma once
#include <utm.h>

#include <addrip_v4.h>
#include <addrmac.h>
#include <gstring.h>
#include <ubase.h>

#define NICALIAS_DEFAULT 0
#define NICALIAS_NDISWAN 1
#define NICALIAS_PUBLIC 2
#define NICALIAS_PRIVATE 3
#define NICALIAS_NETFLOW 4

#define ADAPTER_ETHERNET_MEDIUM 0
#define ADAPTER_RAS_MEDIUM 3
#define ADAPTER_NETFLOW_MEDIUM 5

#ifndef ADAPTERINFO_XML_TAGS
#define ADAPTERINFO_XML_TAGS
#define ADAPTERINFO_XMLTAG_NICLIST "NicList"
#define ADAPTERINFO_XMLTAG_CAPTUREMODE "CaptureMode"
#define ADAPTERINFO_XMLTAG_MAXSELECTION "MaxSelection"
#define ADAPTERINFO_XMLTAG_COUNT "Count"
#define ADAPTERINFO_XMLTAG_N "N"
#endif

#include <iostream>
#include <string>

#include <adapterinfo_base.h>

#define DEVICE_PREFIX_LEN 8  // sizeof("\\device\\") - 1;

namespace utm {

class adapterinfo : public adapterinfo_base
{
public:
	static const char this_class_name[];

	static const char promiscuous_on[];
	static const char promiscuous_off[];

	enum mediums { ADAPTER_MEDIUM_ETHERNET = 0, ADAPTER_MEDIUM_RAS = 3, ADAPTER_MEDIUM_NETFLOW = 5, ADAPTER_MEDIUM_UNKNOWN = 6 };

	adapterinfo();
	~adapterinfo();

#ifdef UTM_WIN
	HANDLE handle;
#endif

	std::string getstr_adapterinfo(const char *text) const;

	static bool has_device_prefix(const char *adaptername) 
	{ 
		return (_strnicmp(adaptername, "\\device\\", DEVICE_PREFIX_LEN) == 0); 
	};

#ifdef UTM_DEBUG
	void test_fillparams(int num);
	static int test_get_testcases_number() { return 1; };
	void test_all();
	
#endif
};

}

#endif // _UTM_ADAPTERINFO_H