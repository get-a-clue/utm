#include "stdafx.h"
#include "sqcore.h"

#include <iostream>
#include <sstream>
#include <iomanip>

#include <utime.h>
#include "sqlite3.h"

#include <boost/crc.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace utm {

sqcore* sqcore ::self = NULL;

sqcore::sqcore()
{
	dbmask = 0;
	report_problem_ptr = NULL;
}

void sqcore::report_problem(const char *problem, const char *dbpath)
{
	std::ostringstream oss;
	oss << problem << " " << dbpath;

	std::string msg = oss.str();

	if (report_problem_ptr != NULL)
	{
		(*report_problem_ptr)(msg.c_str());
	}
}

void sqcore::calc_dbmask(int value, int max_dbmask)
{
	if (value <= 0)
	{
		dbmask = 0;
		return;
	}

	int dbm = value;

	if (dbm > max_dbmask)
		dbm = max_dbmask;

	int mask = 1;
	mask = mask << dbm;
	mask = mask - 1;

	dbmask = mask;
}

int sqcore::get_dbmask()
{
	return dbmask;
}

std::string sqcore::get_dbfile(const char *host)
{
	boost::crc_32_type result;
	result.process_bytes(host, strlen(host));

	unsigned int r = result.checksum();
	int hostkey = r & get_dbmask();

	return get_dbfile(hostkey);
}

std::string sqcore::get_dbfile(int hostkey)
{
	std::ostringstream oss;

#ifdef _WIN32
	oss << dbdirectory;
#endif // _WIN32

	oss << dbprefix << "_" << std::hex << std::setfill('0') << std::setw(2) << hostkey << "." << SQCORE_DBFILE_EXTENSION;
	std::string filename = oss.str();

	boost::filesystem::path path(filename);
	boost::filesystem::path dir = path.parent_path();

	if (!boost::filesystem::is_directory(dir))
		boost::filesystem::create_directories(dir);

	return filename;
}

int sqcore::executequery(const char *dbfile, const char *query)
{
    sqlite3* db;

    if (sqlite3_open(dbfile, &db))
    {
		std::string errmsg = sqlite3_errmsg(db);
		report_problem(errmsg.c_str(), dbfile);
        sqlite3_close(db);
		return SQCORE_ERROR;
    }

	char* errs = NULL;
	if (sqlite3_exec(db, query, NULL, NULL, &errs) != SQLITE_OK)
	{
        if (errs != NULL)
        {
			report_problem(errs, dbfile);
            sqlite3_free(errs);
        }

		sqlite3_close(db);
		return SQCORE_ERROR;
	}

	sqlite3_close(db);

	return SQCORE_OK;
}

int sqcore::select_site(const char *site)
{
	int retval = SQCORE_NOTFOUND;

    sqlite3* db;
	std::string dbfile = get_dbfile(site);

    if (sqlite3_open(dbfile.c_str(), &db) != SQLITE_OK)
    {
		std::string errmsg = sqlite3_errmsg(db);
		report_problem(errmsg.c_str(), dbfile.c_str());
        sqlite3_close(db);
		return SQCORE_ERROR;
    }

	std::ostringstream os; 
	os << "SELECT site, status, insertdt FROM " << SQCORE_TABLE_NAME;
	os << " WHERE site='" << site << "'";
	os << " ORDER BY status DESC";
	std::string sql = os.str();

	bool expired = false;
	sqlite3_stmt *stmt;

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK)
	{
		int cols = sqlite3_column_count(stmt);
		
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			const char *result_site = (const char*)sqlite3_column_text(stmt, 0);
			int result_status = (int)sqlite3_column_int(stmt, 1);
			const char *insertdt = (const char*)sqlite3_column_text(stmt, 2);

			try
			{
				utime dt;
				dt.from_string(insertdt, utime::format_sql);
				if (!dt.is_valid())
				{
					expired = true;
				}
				else
				{
					utime now(true);
					boost::posix_time::time_duration diff = now.ptime - dt.ptime;
					if (diff.total_seconds() > 90*86400)
						expired = true;
				}
			}
			catch(std::exception&)
			{
				expired = true;
			}

			retval = result_status;
		};

		sqlite3_finalize(stmt);
	}

	if (expired)
	{
		std::ostringstream osdel; 
		osdel << "DELETE FROM " << SQCORE_TABLE_NAME;
		osdel << " WHERE site='" << site << "'";
		std::string sql_delete = osdel.str();

		char* errsdel = NULL;
		if (sqlite3_exec(db, sql_delete.c_str(), NULL, NULL, &errsdel) != SQLITE_OK)
		{
			if (errsdel != NULL)
			{
				report_problem(errsdel, dbfile.c_str());
				sqlite3_free(errsdel);
			}
		}
	}

    sqlite3_close(db);

	if (expired)
		return SQCORE_NOTFOUND;

	return retval;
}

int sqcore::update_site(const char *site, int status)
{
    sqlite3* db;
	std::string dbfile = get_dbfile(site);

    if (sqlite3_open(dbfile.c_str(), &db))
    {
		std::string errmsg = sqlite3_errmsg(db);
		report_problem(errmsg.c_str(), dbfile.c_str());
        sqlite3_close(db);
		return SQCORE_ERROR;
    }

	std::ostringstream osc; 
	osc << "CREATE TABLE IF NOT EXISTS " << SQCORE_TABLE_NAME;
	osc << " (id integer primary key, site text, status int, insertdt text)";
	std::string sqlc = osc.str();

	char* errc = NULL;
	if (sqlite3_exec(db, sqlc.c_str(), NULL, NULL, &errc) != SQLITE_OK)
	{
        if (errc != NULL)
        {
			report_problem(errc, dbfile.c_str());
            sqlite3_free(errc);
        }
		sqlite3_close(db);
		return SQCORE_ERROR;
	}

	utime now(true);

	std::ostringstream oss;
	oss << "INSERT INTO " << SQCORE_TABLE_NAME << " (site, status, insertdt) ";
	oss << "VALUES ('" << site << "', " << status << ", '" << now.to_string(utime::format_sql) << "')";
	std::string sqls = oss.str();

	char* errs = NULL;
	if (sqlite3_exec(db, sqls.c_str(), NULL, NULL, &errs) != SQLITE_OK)
	{
        if (errs != NULL)
        {
			report_problem(errs, dbfile.c_str());
            sqlite3_free(errs);
        }
		sqlite3_close(db);
		return SQCORE_ERROR;
	}

	sqlite3_close(db);

	return SQCORE_OK;
}

}