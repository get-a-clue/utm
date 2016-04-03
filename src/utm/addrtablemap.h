#ifndef _UTM_ADDRTABLEMAP_H
#define _UTM_ADDRTABLEMAP_H

#pragma once
#include <utm.h>

#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>

#include <boost/thread/mutex.hpp>
#include <boost/typeof/typeof.hpp>

#include <ubase.h>
#include <addrpair.h>
#include <ubaselist.h>

#define ADDRTABLEMAP_XMLTAG_MAT "MAT"
#define ADDRTABLEMAP_XMLTAG_KEY "Key"
#define ADDRTABLEMAP_XMLTAG_TYPE "Type"
#define ADDRTABLEMAP_XMLTAG_DESCR "Descr"
#define ADDRTABLEMAP_XMLTAG_FILE "File"
#define ADDRTABLEMAP_XMLTAG_DATAID "DataId"
#define ADDRTABLEMAP_XMLTAG_ADDR "Addr"

namespace utm {

template<class T>
class addrtablemaprec : public ubase
{
public:
	addrtablemaprec() {	};
	~addrtablemaprec() { };

	unsigned int get_id() const { return key; };
	void set_id(unsigned int id) { key = id; };
	const gstring& get_name() const { return addrtable.descr; };

	addrtablemaprec(unsigned int _key, const T& _addrtable)
	{
		key = _key;
		addrtable = _addrtable;
	}

	virtual bool equals(const ubase* rhs) const
	{
		const addrtablemaprec<T>* p = dynamic_cast<const addrtablemaprec<T> *>(rhs);
		if (p == NULL)
		{
			return false;
		}

		return operator==(*p);
	}

	bool operator==(const addrtablemaprec& rhs) const
	{
		if (key != rhs.key) return false;
		return addrtable.itemsref() == rhs.addrtable.itemsref();
	}

	virtual const char *get_this_class_name() const { return "addrtablemaprec"; };

	unsigned int key;
	T addrtable;

	void clear()
	{
		key = 0;
		addrtable.clear();
	}

	void xml_create() 
	{
		addrtablemaprec<T> orig;

		xml_append_root(ADDRTABLEMAP_XMLTAG_MAT);
		xml_append_node(ADDRTABLEMAP_XMLTAG_KEY, key);
		xml_append_node(ADDRTABLEMAP_XMLTAG_TYPE, addrtable.type_id, orig.addrtable.type_id);
		xml_append_node(ADDRTABLEMAP_XMLTAG_DESCR, addrtable.descr, orig.addrtable.descr);
		xml_append_node(ADDRTABLEMAP_XMLTAG_FILE, addrtable.filename, orig.addrtable.filename);
		xml_append_node(ADDRTABLEMAP_XMLTAG_DATAID, addrtable.data_id, orig.addrtable.data_id);

		std::string s;
		s.reserve(100);

		for (BOOST_AUTO(iter, addrtable.itemsref().begin()); iter != addrtable.itemsref().end(); ++iter)
		{
			s.clear();
			s.append(iter->first.to_string());
			s.append(",");
			s.append(iter->second.to_string());

			xml_append_node(ADDRTABLEMAP_XMLTAG_ADDR, s);
		}
	};
	
	void xml_catch_value(const char *keyname, const char *keyvalue)
	{
		std::string tmp;

		while(1)
		{
			if (xml_check_value(keyname, ADDRTABLEMAP_XMLTAG_KEY, keyvalue, key)) break;
			if (xml_check_value(keyname, ADDRTABLEMAP_XMLTAG_TYPE, keyvalue, addrtable.type_id)) break;
			if (xml_check_value(keyname, ADDRTABLEMAP_XMLTAG_DESCR, keyvalue, addrtable.descr)) break;
			if (xml_check_value(keyname, ADDRTABLEMAP_XMLTAG_FILE, keyvalue, addrtable.filename)) break;
			if (xml_check_value(keyname, ADDRTABLEMAP_XMLTAG_DATAID, keyvalue, addrtable.data_id)) break;
			if (xml_check_value(keyname, ADDRTABLEMAP_XMLTAG_ADDR, keyvalue, tmp))
			{
				std::string first;
				size_t pos = tmp.find_first_of(",");
				if (pos != std::string::npos)
				{
					first = tmp.substr(0, pos);
					addrtable.AddAddrPair(first.c_str(), tmp.c_str() + pos + 1, false /* WORKAROUND: can be 'true', but we can read network table */);
				}
				break;
			};

			break;
		}
	};
	
	ubase* xml_catch_subnode(const char *tag_name, const char *class_name) { return NULL; };
};

class addrtablemaprec_v4 : public addrtablemaprec<utm::addrtable_v4>
{
public:
	addrtablemaprec_v4() 
	{
		clear();
	};
	
	addrtablemaprec_v4(const addrtable_v4& at)
	{
		key = 0;
		addrtable = at;
	}

	virtual ~addrtablemaprec_v4() { };
};

template<class T>
class addrtablemaplist : public ubaselist<addrtablemaprec<T>>
{
public:
	addrtablemaplist() { };
	virtual ~addrtablemaplist() { };

	virtual const char *get_this_class_name() const { return "addrtablemaplist"; };

	void clear()
	{
		items.clear();
	}

    void xml_create()
	{
		xml_append_root("MatList");
	    xml_append_node(ADDRTABLEMAP_XMLTAG_MAT, items);
	}

    void xml_catch_value(const char *keyname, const char *keyvalue)
	{
	}

	ubase* xml_catch_subnode(const char *keyname, const char *class_name)
	{
		ubase *u = NULL;

		if (strcmp(keyname, ADDRTABLEMAP_XMLTAG_MAT) == 0)
		{
			u = dynamic_cast<addrtablemaprec<T> *>(new addrtablemaprec<T>());
			add_element(u);
		}

		return u;
	}
};

}

#endif // _UTM_ADDRTABLEMAP_H