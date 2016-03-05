#ifndef _UTM_ADAPTERLIST_H
#define _UTM_ADAPTERLIST_H

#pragma once
#include <utm.h>

#ifndef MAX_CAPTURE_THREADS
#define MAX_CAPTURE_THREADS 32
#endif

#define PUBLIC_NIC_NOT_FOUND -1

#ifdef UTM_WIN
#include <winpkfilter.h>
#include <sysnet.h>
#include <iphlpapi.h>
#include <winsock2.h>
#endif

#include <adapterinfo.h>

#include <iostream>
#include <string>
#include <vector>

namespace utm {

typedef std::vector<adapterinfo> adapters_container;

class adapterlist : public ubase
{
public:
	static const char this_class_name[];

	adapterlist();
	~adapterlist();

	bool operator==(const adapterlist& rhs) const;

	// class members
	std::vector<adapterinfo> adapters;
	unsigned int capture_mode;

	// class methods
	void assign(const adapterlist& adlist);
	size_t size() const { return adapters.size(); };
	size_t size_selected() const;
	size_t size_public() const;
	bool is_empty() const { bool retval = size() == 0 ? true : false; return retval; };
	int get_publicnic_index() const;
	std::vector<unsigned int> get_publicip(CSysNet *ps, DWORD desiredPublicIp);
	void AddNetflowAdapter();
	bool HasNetflowAdapter() const;
	void DeleteNetflowAdapter();

	// overloaded class methods from base class
	void clear();
	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue);
	ubase* xml_catch_subnode(const char *name);

#ifdef UTM_WIN
	bool FindAdapterInfoByName(PIP_ADAPTER_INFO ppai, const char* szAdapterName, adapterinfo& ai) const;
	void FillFromTcpAdapterList(TCP_AdapterList *pAdList, PIP_ADAPTER_INFO ppai);

	LONG ReadSelectedListFromRegistry(const TCHAR *pKey);
	LONG ReadSelectedListFromRegistry(HKEY hkey, const TCHAR *pRegistryPath);
	LONG SaveSelectedListToRegistry(const TCHAR *pKey);
	LONG SaveSelectedListToRegistry(HKEY hkey, const TCHAR *pRegistryPath);
#endif

	void RevalidateSelectedList(const utm::adapterlist& selected, utm::adapterlist& revalidated, bool include_only_selected) const;
	void ExtractSelectedList(adapterlist& selected) const;

#ifdef UTM_DEBUG
//	const char* xml_get_stylesheet() const { return "test"; };
	static int test_get_testcases_number() { return 3; };
	void test_fillparams(int test_num);
	void test_all();
	
#endif
};

}

#endif // _UTM_ADAPTERLIST_H