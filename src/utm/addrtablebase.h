#ifndef _UTM_ADDRTABLEBASE_H
#define _UTM_ADDRTABLEBASE_H

#pragma once
#include <utm.h>

#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>

#include <boost/thread/mutex.hpp>

namespace utm {

template<class T>
class addrtablebase
{
public:
	typedef typename std::map<T, T>::iterator AddrTableIter;
	typedef typename std::map<T, T>::const_iterator AddrTableConstIter;

	addrtablebase()
	{
		clear();
	}

	virtual ~addrtablebase()
	{
	}

	addrtablebase& operator=(const addrtablebase& rhs)
	{
		copy(rhs);
		return *this;
	}

	bool operator==(const addrtablebase& rhs) const
	{
		return items == rhs.itemsref();
	}

	addrtablebase(const addrtablebase& r)
	{
		copy(r);
	}

	bool is_equal(const addrtablebase& r) const
	{
		boost::mutex::scoped_lock lock(guard);
		return items == r.itemsref();
	}

	void clear()
	{
		boost::mutex::scoped_lock lock(guard);
		items.clear();
	}

	void copy(const addrtablebase& r)
	{
		boost::mutex::scoped_lock lock(guard);
		items = r.items;
	}

protected:
	void Init()
	{
		boost::mutex::scoped_lock lock(guard);
		items.clear();
	}

	std::map<T, T> items;
	mutable boost::mutex guard;

public:

	const std::map<T,T>& itemsref() const
	{
		return items;
	}

	size_t GetAddrCount()
	{
		boost::mutex::scoped_lock lock(guard);
		return items.size();
	}

	bool GetAddrPair(size_t index, T& AddrStart, T& AddrEnd)
	{
		boost::mutex::scoped_lock lock(guard);

		bool retval = false;
		AddrTableConstIter iter;

		if (index < items.size())
		{
			iter = items.begin();
			while (index > 0)
			{
				index--;
				++iter;
			}

			AddrStart = iter->first;
			AddrEnd = iter->second;
			retval = true;
		}

		return retval;
	}

	bool AddAddrPair(const char* pAddrStart, const char* pAddrEnd, bool checkRange)
	{
		T AddrStart(pAddrStart);
		T AddrEnd(pAddrEnd);
		return AddAddrPair(AddrStart, AddrEnd, checkRange);
	}

	bool AddAddrPair(const T& AddrStart, const T& AddrEnd, bool checkRange)
	{
		if (checkRange)
		{
			if (CheckAddrRange(AddrStart))
				return false;

			if (CheckAddrRange(AddrEnd))
				return false;
		}

		{
			boost::mutex::scoped_lock lock(guard);
			items.insert(std::make_pair(AddrStart, AddrEnd));
		}

		return true;
	}

	bool CheckAddrRange(const T& Addr)
	{
		boost::mutex::scoped_lock lock(guard);

		bool retval = false;
		if (!items.empty())
		{
			AddrTableConstIter found = items.lower_bound(Addr);

			if (found == items.end())
			{
				if ((items.rbegin()->first <= Addr) && (items.rbegin()->second >= Addr))
					retval = true;
			}
			else
			{
				if ((found->first <= Addr) && (found->second >= Addr))
				{
					retval = true;
				}
				else
				{
					if (found != items.begin())
						--found;

					if ((found->first <= Addr) && (found->second >= Addr))
						retval = true;
				}
			}
		}

		return retval;
	}

	bool CheckAddrSingle(const T& Addr)
	{
		boost::mutex::scoped_lock lock(guard);
		AddrTableConstIter found = items.find(Addr);
		return (found != items.end());
	}

	bool DeleteAddr(size_t index)
	{
		boost::mutex::scoped_lock lock(guard);

		bool retval = false;
		AddrTableIter iter;

		if (index < items.size())
		{
			iter = items.begin();
			while (index > 0)
			{
				index--;
				iter++;
			}

			items.erase(iter);
			retval = true;
		};

		return retval;
	}
};

}

#endif // _UTM_ADDRTABLEBASE_H