#ifndef _UTM_LOGGINGPARAMS_H
#define _UTM_LOGGINGPARAMS_H

#pragma once
#include <utm.h>

#include <gstring.h>
#include <ufs.h>
#include <ubase.h>

#define LOGGINGPARAMS_XMLTAG_PARAMS "Logging"
#define LOGGINGPARAMS_XMLTAG_FACILITY "Facility"
#define LOGGINGPARAMS_XMLTAG_CONSOLEENABLE "ConsoleEnable"
#define LOGGINGPARAMS_XMLTAG_FILEENABLE "FileEnable"
#define LOGGINGPARAMS_XMLTAG_FILEDIRECTORY "FileDirectory"
#define LOGGINGPARAMS_XMLTAG_FILENAME "FileName"
#define LOGGINGPARAMS_XMLTAG_FILEMAXSIZE "FileSize"
#define LOGGINGPARAMS_XMLTAG_FILEMAXROTATE "FileRotate"

namespace utm {

class loggingparamsbase
{
public:
	loggingparamsbase(void);
	virtual ~loggingparamsbase(void);

	bool operator==(const loggingparamsbase& rhs) const;

	const gstring& get_commondirectory() const;
	const gstring& get_directory() const;
	const gstring& get_filename() const;
	const gstring& get_fullpath() const;
	size_t get_maxlogsize() const { return maxlogsize; };
	size_t get_maxlogrotate() const { return maxlogrotate; };

	void set_commondirectory(const gstring& dir);
	void set_directory(const gstring& dir);
	void set_filename(const gstring& fname);
	void set_maxlogsize(size_t size) { maxlogsize = size; };
	void set_maxlogrotate(size_t size) { maxlogrotate = size; };
	void set_params(const gstring& _directory, const gstring& _filename, size_t _maxlogsize, size_t _maxlogrotate);

	void copy_params_from(const loggingparamsbase& params);
	void clear();

public:
	bool enable_file;
	bool enable_console;
	std::string facility;

protected:
	void make_fullpath();
	gstring fullpath;
	gstring directory;
	gstring filename;
	size_t maxlogsize;
	size_t maxlogrotate;

	static gstring common_directory;
};

class loggingparams : public loggingparamsbase, public ubase 
{
public:
	static const char this_class_name[];
	const char* get_this_class_name() { return loggingparams::this_class_name; };

	loggingparams(void);
	~loggingparams(void);

	void clear() { loggingparamsbase::clear(); };
	void xml_create();
	void xml_catch_value(const char *keyname, const char *keyvalue);
	ubase* xml_catch_subnode(const char *name) { return NULL; };

	static int test_get_testcases_number() { return 1; };
	void test_fillparams(int test_num);
};

}

#endif // _UTM_LOGGINGPARAMS_H