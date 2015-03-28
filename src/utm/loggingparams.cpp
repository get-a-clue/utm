#include "stdafx.h"

#include "loggingparams.h"

namespace utm {

gstring loggingparamsbase::common_directory("");
const char loggingparams::this_class_name[] = "loggingparams";

loggingparamsbase::loggingparamsbase(void) : 
   enable_file(false), enable_console(false)
{
}


loggingparamsbase::~loggingparamsbase(void)
{
}

bool loggingparamsbase::operator==(const loggingparamsbase& rhs) const
{
	if (facility != rhs.facility)
		return false;

	if (enable_file != rhs.enable_file)
		return false;

	if (enable_console != rhs.enable_console)
		return false;

	if (directory != rhs.directory)
		return false;

	if (filename != rhs.filename)
		return false;

	if (maxlogsize != rhs.maxlogsize)
		return false;

	if (maxlogrotate != rhs.maxlogrotate)
		return false;

	return true;
}

void loggingparamsbase::clear()
{
	facility.clear();
	enable_file = false;
	enable_console = false; 
	directory.clear();
	filename.clear();
	maxlogsize = 10;
	maxlogrotate = 8;
}

void loggingparamsbase::copy_params_from(const loggingparamsbase& params)
{
	set_directory(params.get_directory());
	set_filename(params.get_filename());
	set_maxlogsize(params.get_maxlogsize());
	set_maxlogrotate(params.get_maxlogrotate());
	enable_file = params.enable_file;
	enable_console = params.enable_console;
}

const gstring& loggingparamsbase::get_commondirectory() const
{
	return common_directory;
}

const gstring& loggingparamsbase::get_directory() const
{
	if (directory.empty())
		return common_directory;

	return directory;
}

const gstring& loggingparamsbase::get_filename() const
{
	return filename;
}

const gstring& loggingparamsbase::get_fullpath() const
{
	return fullpath;
}

void loggingparamsbase::set_commondirectory(const gstring& dir)
{
	ufs::make_strong_directory_path(dir, common_directory);
	make_fullpath();
}

void loggingparamsbase::set_directory(const gstring& dir)
{
	ufs::make_strong_directory_path(dir, directory);
	make_fullpath();
}

void loggingparamsbase::set_filename(const gstring& fname)
{
	filename = fname;
	make_fullpath();
}

void loggingparamsbase::set_params(const gstring& _directory, const gstring& _filename, size_t _maxlogsize, size_t _maxlogrotate)
{
	set_directory(_directory);
	set_filename(_filename);
	set_maxlogsize(_maxlogsize);
	set_maxlogrotate(_maxlogrotate);
}

void loggingparamsbase::make_fullpath()
{
	fullpath.assign(get_directory().c_str());
	fullpath.append(filename.c_str());
}

loggingparams::loggingparams()
{
	clear();
}

loggingparams::~loggingparams()
{

}

void loggingparams::xml_create()
{
	xml_append_root(LOGGINGPARAMS_XMLTAG_PARAMS);
	xml_append_node(LOGGINGPARAMS_XMLTAG_FACILITY, facility);
	xml_append_node(LOGGINGPARAMS_XMLTAG_CONSOLEENABLE, enable_console);
	xml_append_node(LOGGINGPARAMS_XMLTAG_FILEENABLE, enable_file);
	xml_append_node(LOGGINGPARAMS_XMLTAG_FILEDIRECTORY, directory);
	xml_append_node(LOGGINGPARAMS_XMLTAG_FILENAME, filename);
	xml_append_node(LOGGINGPARAMS_XMLTAG_FILEMAXSIZE, maxlogsize);
	xml_append_node(LOGGINGPARAMS_XMLTAG_FILEMAXROTATE, maxlogrotate);
}

void loggingparams::xml_catch_value(const char *keyname, const char *keyvalue)
{
	while(1)
	{
		if (xml_check_value(keyname, LOGGINGPARAMS_XMLTAG_FACILITY, keyvalue, facility)) break;
		if (xml_check_value(keyname, LOGGINGPARAMS_XMLTAG_CONSOLEENABLE, keyvalue, enable_console)) break;
		if (xml_check_value(keyname, LOGGINGPARAMS_XMLTAG_FILEENABLE, keyvalue, enable_file)) break;
		if (xml_check_value(keyname, LOGGINGPARAMS_XMLTAG_FILEDIRECTORY, keyvalue, directory)) break;
		if (xml_check_value(keyname, LOGGINGPARAMS_XMLTAG_FILENAME, keyvalue, filename)) break;
		if (xml_check_value(keyname, LOGGINGPARAMS_XMLTAG_FILEMAXSIZE, keyvalue, maxlogsize)) break;
		if (xml_check_value(keyname, LOGGINGPARAMS_XMLTAG_FILEMAXROTATE, keyvalue, maxlogrotate)) break;

		break;
	}
}

void loggingparams::test_fillparams(int test_num)
{
	loggingparamsbase::clear();

	facility = "squid";
	enable_file = true;
	enable_console = true;
	directory = "1";
	filename = "s";
	maxlogsize = 999;
	maxlogrotate = 98;
}

}