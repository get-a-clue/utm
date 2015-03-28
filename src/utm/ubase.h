#ifndef _UTM_UBASE_H
#define _UTM_UBASE_H

#pragma once
#include <utm.h>

#include <vector>
#include <list>
#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <cstdint>
#include <atomic>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <pugiconfig.hpp>
#include <pugixml.hpp>
#include <gstring.h>
#include <addrip_v4.h>

typedef std::map<std::string, std::string> xmlattr_container;

namespace utm {

class ubase
{
public:
	ubase(void);
	virtual ~ubase(void);

	ubase(const ubase& r) { };
	ubase& operator=(const ubase& r) { return *this; };

	virtual const char *get_this_class_name() { return "ubase"; };

public:
	virtual void clear() { xml_catch_value_callback = NULL; xml_catch_subnode_callback = NULL; };
	virtual void xml_create() = 0;
	virtual void xml_catch_value(const char *name, const char *value) = 0;
	virtual ubase* xml_catch_subnode(const char *name) = 0;
	virtual void xml_catch_subnode_finished(const char *name);
	virtual void xml_catch_subnode_attribute(const char *attrname, const char* attrvalue);
	virtual void xml_catch_rootnode_attribute(const char *attrname, const char* attrvalue);
	virtual bool xml_has_root_attr() const { return false; };
	virtual void xml_get_root_attr(xmlattr_container& attr) { };
	virtual const char* xml_get_stylesheet() const { return NULL; };
	void xml_get_string(std::string& xml, bool include_xml_declaration = true) const;
	std::string xml_createstring(bool include_xml_declaration = true);
	bool xml_parse(const char* xml, bool appendmode = false);
	void xml_save(const gstring& filename);
	void xml_save(const std::string& filename);
	void xml_save(const char *filename);
	void xml_save_debug(const gstring& folder, const char *id = NULL, bool include_timestamp = false);
	void xml_save_debug(const char* folder, const char *id = NULL, bool include_timestamp = false);
	void xml_load(const gstring& filename);
	void xml_load(const std::string& filename);
	void xml_load(const char *filename);

	void xml_parse(pugi::xml_node& node, bool appendmode = false);
	pugi::xml_node get_first_child() { return xdoc->root().first_child(); };

	boost::function<void(const char *name, const char *value)> xml_catch_value_callback;
	boost::function<void(const char *name, const ubase *u)> xml_catch_subnode_callback;

public:
#ifdef UTM_WIN
	virtual bool has_registry() { return false; };
	virtual LONG CreateRegistry(const TCHAR* pRegistryPath, const HKEY hk = HKEY_LOCAL_MACHINE);
	virtual LONG ReadFromFileOrRegistry(const std::string& configpath, const TCHAR *pRegistryPath, const HKEY hkey = HKEY_LOCAL_MACHINE);
	virtual LONG ReadFromRegistry(const TCHAR *pRegistryPath, const HKEY hkey = HKEY_LOCAL_MACHINE) { return ERROR_SUCCESS; };
	virtual LONG SaveToFileOrRegistry(const std::string& configpath, const TCHAR *pRegistryPath, const HKEY hkey = HKEY_LOCAL_MACHINE);
	virtual LONG SaveToRegistry(const TCHAR *pRegistryPath, const HKEY hkey = HKEY_LOCAL_MACHINE) { return ERROR_SUCCESS; };

	LONG RegistrySave(HKEY hk, const char* keyname, const char* value) const;
	LONG RegistrySave(HKEY hk, const char* keyname, const std::string& value) const;
	LONG RegistrySave(HKEY hk, const char* keyname, const gstring& value) const;
	LONG RegistrySave(HKEY hk, const char* keyname, bool value) const;
	LONG RegistrySave(HKEY hk, const char* keyname, int value) const;
	LONG RegistrySave(HKEY hk, const char* keyname, unsigned int value) const;

	LONG RegistryLoad(HKEY hk, const char* keyname, std::string& value);
	LONG RegistryLoad(HKEY hk, const char* keyname, gstring& value);
	LONG RegistryLoad(HKEY hk, const char* keyname, bool& value);
	LONG RegistryLoad(HKEY hk, const char* keyname, int& value);
	LONG RegistryLoad(HKEY hk, const char* keyname, unsigned int& value);
#endif

private:
	std::unique_ptr<pugi::xml_document> xdoc;
	std::string rootname;

	void xml_parse_allnodes(pugi::xml_node& node);
	inline void check_xdoc() { if (xdoc.get() == NULL) xml_init(); };
	inline void xml_append_data(const char* keyname, const char* value)
	{
		pugi::xml_node node = xdoc->root().child(rootname.c_str()).append_child(keyname);
		node.append_child(pugi::node_pcdata).set_value(value);
	}
protected:
	void xml_init();

	void xml_append_root(const char* keyname);
	void xml_append_root(const char* keyname, const char* text);
	pugi::xml_node xml_get_root() { return xdoc->root().child(rootname.c_str()); };
	void xml_append_stylesheet(const char *value);
	void xml_append_child(pugi::xml_node& node);
	void xml_append_node(const char* keyname, const char* value);
	void xml_append_node(const char* keyname, const char* value, const char* default_value);
	void xml_append_node(const char* keyname, const gstring& value);
	void xml_append_node(const char* keyname, const gstring& value, const gstring& default_value);
	void xml_append_node(const char* keyname, bool value);
	void xml_append_node(const char* keyname, bool value, bool default_value);
	void xml_append_node(const char* keyname, const addrip_v4& value);
	void xml_append_node(const char* keyname, const addrip_v4& value, const addrip_v4& default_value);
	void xml_append_node(const char* keyname, const std::vector<std::string>& value);
	void xml_append_node(const char* keyname, const std::list<std::string>& value);

	template<typename T>
	void xml_append_node(const char* keyname, const std::list<T>& value)
	{
		for(std::list<T>::const_iterator iter = value.begin(); iter != value.end(); ++iter)
		{
			T t(*iter);
			t.xml_create();
			xml_append_child(t.get_first_child());
		}
	}

	template<typename T>
	void xml_append_node(const char* keyname, T value)
	{
		check_xdoc();
	    std::ostringstream oss;
		oss << value;
		xml_append_data(keyname, oss.str().c_str());
	}

	template<typename T>
	void xml_append_node(const char* keyname, T value, T default_value)
	{
		if (value == default_value) return;
		xml_append_node(keyname, value);
	}

	bool xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, std::string& wanted_keyvalue);
	bool xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, gstring& wanted_keyvalue);
	bool xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, bool& wanted_keyvalue);
	bool xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, addrip_v4& wanted_keyvalue);

	bool xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, std::atomic_uint_fast32_t& wanted_keyvalue)
	{
		if (!xml_check_args(current_keyname, wanted_keyname))
			return false;

		std::uint32_t v = 0;
		if (current_keyvalue != NULL)
		{
			std::istringstream iss(current_keyvalue);
			iss >> v;
		}
		wanted_keyvalue = v;

		return true;
	}

	bool xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, std::atomic_uint_fast64_t& wanted_keyvalue)
	{
		if (!xml_check_args(current_keyname, wanted_keyname))
			return false;

		std::uint64_t v = 0;
		if (current_keyvalue != NULL)
		{
			std::istringstream iss(current_keyvalue);
			iss >> v;
		}
		wanted_keyvalue = v;

		return true;
	}

	bool xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, boost::function<void (const char *)> parser)
	{
		if (!xml_check_args(current_keyname, wanted_keyname))
			return false;

		parser(current_keyvalue);

		return true;
	};

	template<typename T>
	bool xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, T& wanted_keyvalue)
	{
		if (!xml_check_args(current_keyname, wanted_keyname))
			return false;

		if (current_keyvalue != NULL)
		{
			std::istringstream iss(current_keyvalue);
			iss >> wanted_keyvalue;
		}
//		else
//			wanted_keyvalue = 0;

		return true;
	}

	inline bool xml_check_args(const char* current_keyname, const char* wanted_keyname);

	template<typename ContainerT>
	ubase* insert_element_and_get_lastptr(ContainerT& cont)
	{
		typedef typename ContainerT::value_type ElementT;
		ElementT item;
		cont.push_back(item);
		ContainerT::reverse_iterator riter = cont.rbegin(); 
		ContainerT::iterator iter = --riter.base();
		ubase* u = (ubase *)&(*iter);
		return u;
	}

protected:
	static const char xml_true[];
	static const char xml_false[];
};

}

#endif // _UTM_UBASE_H