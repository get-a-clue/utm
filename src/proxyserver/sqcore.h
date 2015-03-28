#ifndef SQCORE_H
#define SQCORE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <vector>

#define SQCORE_OK 1
#define SQCORE_ERROR 0
#define SQCORE_NOTFOUND -1
#define SQCORE_BADSITE 1
#define SQCORE_SUSPICIOSSITE 2
#define SQCORE_GOODSITE 3

#define SQCORE_DBFILE_EXTENSION "db"
#define SQCORE_DBFILE_MAXCHARS 260

#define SQCORE_TABLE_NAME "sites"

namespace utm {

class sqcore
{
public:
	static sqcore* instance()
	{
		if (!self) self = new sqcore();
		return self;
	}

	void (*report_problem_ptr)(const char *msg);
	std::string dbdirectory;
	std::string dbprefix;

	int get_dbmask();
	void calc_dbmask(int value, int max_dbmask);

	void report_problem(const char *problem, const char *dbpath);

	std::string get_dbfile(const char *host);
	std::string get_dbfile(int hostkey);

	int select_site(const char *site);
	int update_site(const char *site, int status);

protected:
	int executequery(const char *dbfile, const char *query);

	int dbmask;

	static sqcore* self;
	sqcore();
};

}

#endif // SQCORE_H
