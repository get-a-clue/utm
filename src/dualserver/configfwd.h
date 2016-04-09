#pragma once

#if defined(_MSC_VER)
#define UTM_WIN
#endif

#include "configfwd_base.h"

#define CONFIGFWD_REGISTRYKEY_IP "FwdIp"
#define CONFIGFWD_REGISTRYKEY_PORT "FwdPort"

namespace utm {

class configfwd : public configfwd_base
{
public:
	static const char this_class_name[];

public:
	configfwd(void);
	~configfwd(void);

	const char *get_this_class_name() const { return "configfwd"; };

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
