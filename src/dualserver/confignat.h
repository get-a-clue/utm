#pragma once

#if defined(_MSC_VER)
#define UTM_WIN
#endif

#ifdef UTM_WIN

#include <Windows.h>
#include <tchar.h>

#endif

#include <addrip_v4.h>
#include <stringtools.h>
#include <ubase.h>
#include <confignat_base.h>

#ifndef CONFIGNAT_XML_TAGS
#define CONFIGNAT_XML_TAGS

#define CONFIGNAT_REGISTRYKEY_ENABLED "Nat"
#define CONFIGNAT_REGISTRYKEY_EXCLUDEFIRST "NatFirst"
#define CONFIGNAT_REGISTRYKEY_AUTOPUBLIC "NatAutoPublic"
#define CONFIGNAT_REGISTRYKEY_PUBLICIP "NatPublicIp"
#define CONFIGNAT_REGISTRYKEY_PORTRDR "NatPortRedirect"

#endif

namespace utm {

class confignat : public confignat_base
{
public:
	static const char this_class_name[];

public:
	confignat(void);
	~confignat(void);

	const char *get_this_class_name() const { return this_class_name; };

	std::vector<std::string> create_portrdr_string() const;
	void parse_portrdr_string(const char *portrdr_string);

public:
#ifdef UTM_WIN
	bool has_registry() { return true; };
	LONG ReadFromRegistry(const TCHAR *pRegistryPath, const HKEY hkey = HKEY_LOCAL_MACHINE);
	LONG SaveToRegistry(const TCHAR *pRegistryPath, const HKEY hkey = HKEY_LOCAL_MACHINE);
#endif

#ifdef UTM_DEBUG
	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num);
#endif
};

}