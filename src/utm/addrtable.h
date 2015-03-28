#ifndef _UTM_ADDRTABLE_H
#define _UTM_ADDRTABLE_H

#pragma once
#include <utm.h>

#include <addrtablebase.h>
#include <gstring.h>
#include <utimestamp.h>
#include <ubase.h>

#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>

#include <boost/thread/mutex.hpp>

namespace utm {

template<class T>
class addrtable : public addrtablebase<T>
{
public:
	enum type { ADDR_TABLE_ORIGINAL = 0, ADDR_TABLE_EXTFILE = 1, ADDR_TABLE_PPP = 2 };

	addrtable()
	{
		clear();
	}

	~addrtable()
	{
	}

	void clear()
	{
		addrtablebase::clear();

		descr.clear();
		filename.clear();
		type_id = ADDR_TABLE_ORIGINAL;
		data_id = 0;
		sort_order = 0;
		last_file_write_time.clear();
	}

public:
	gstring descr;
	gstring filename;
	int type_id;
	int data_id;
	int sort_order;
	utimestamp last_file_write_time;

	int GetAddrTableType() const
	{
		return type_id;
	}

	void SetAddrTableType(int value)
	{
		type_id = value;
	}

	int GetAddrTableDataId() const
	{
		return data_id;
	}

	void SetAddrTableDataId(int value)
	{
		data_id = value;
	}

	const gchar* GetDescr() const
	{
		return descr.c_str();
	}

	void SetDescr(const gchar* p)
	{
		descr.assign(p);
	}

	const gchar* GetFile() const
	{
		return filename.c_str();
	}

	void SetFile(const gchar* p)
	{
		filename.assign(p);
	}

	bool export_to_txt(const gstring& output_filename, const char* file_header)
	{
		boost::mutex::scoped_lock lock(guard);

		bool success = false;
		AddrTableConstIter iter;

		std::ofstream f(output_filename.c_str());

		if (f)
		{
			if (file_header != NULL) 
				f << file_header;

			f << "// Number of records: " << items.size() << std::endl << "//" << std::endl;
			for (iter = items.begin(); iter != items.end(); ++iter)
			{
				f << iter->first << " " << iter->second << std::endl;
			};

			f.close();
			success = true;
		};

		return success;
	}
};

}

#endif // _UTM_ADDRTABLE_H