#include "stdafx.h"

#include <sstream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "ubase.h"
#include "utime.h"
#include "ufs.h"
#ifdef UTM_WIN
#include "RegistryHelper.h"
#endif

namespace utm {

const char ubase::xml_true[] = "1";
const char ubase::xml_false[] = "0";

ubase::ubase(void)
{
}


ubase::~ubase(void)
{
}

void ubase::xml_init()
{
	if (xdoc.get() != NULL)
		xdoc.reset();

	xdoc = std::unique_ptr<pugi::xml_document>(new pugi::xml_document());
}

bool ubase::xml_parse(const char* xml, bool appendmode)
{
	if (!appendmode)
		clear();

	xml_init();

	pugi::xml_parse_result parse_result = xdoc->load(xml);
	if (parse_result.status != pugi::status_ok)
		return false;

	pugi::xml_node root = xdoc->root();
	if (root)
	{
		pugi::xml_attribute_iterator xaiter;
		for (xaiter = root.first_child().attributes_begin(); xaiter != root.first_child().attributes_end(); ++xaiter)
		{
			xml_catch_rootnode_attribute(xaiter->name(), xaiter->value());
		}

		xml_parse_allnodes(root.first_child().first_child());
	}

	xdoc.reset();
	return true;
}

void ubase::xml_parse(pugi::xml_node& node, bool appendmode)
{
	if (!appendmode)
		clear();

	xml_parse_allnodes(node.first_child());
}

void ubase::xml_parse_allnodes(pugi::xml_node& node)
{
	if (!node)
		return;

	const char* keyname;
	const char* keyvalue;
	bool is_keyvalue_empty;

	while (node)
	{
		keyname = node.name();
		keyvalue = node.text().get();

		is_keyvalue_empty = false;

		if (keyvalue == NULL)
			is_keyvalue_empty = true;
		else
		{
			if (*keyvalue == 0)
				is_keyvalue_empty = true;
		}

		if (is_keyvalue_empty)
		{
			std::string class_name;
			bool has_attributes = false;

			{
				// Extract class name from attributes
				for (auto xaiter = node.attributes_begin(); xaiter != node.attributes_end(); ++xaiter)
				{
					const char* attr_name = xaiter->name();
					const char* attr_value = xaiter->value();
					if ((attr_name != NULL) && (attr_value != NULL) && (strcmp(attr_name, "CN") == 0))
					{
						class_name.assign(attr_value);
						break;
					}
					else
					{
						has_attributes = true;
					}
				}
			}

			ubase* u = xml_catch_subnode(keyname, class_name.c_str());
			if (u != NULL)
			{
				u->xml_parse(node);

				if (has_attributes)
				{
					for (auto xaiter = node.attributes_begin(); xaiter != node.attributes_end(); ++xaiter)
					{
						u->xml_catch_subnode_attribute(xaiter->name(), xaiter->value());
					}
				}

				xml_catch_subnode_finished(keyname);
				if (xml_catch_subnode_callback != NULL)
				{
					xml_catch_subnode_callback(keyname, u);
				}
			}
		}
		else
		{
			xml_catch_value(keyname, keyvalue);
			if (xml_catch_value_callback != NULL)
			{
				xml_catch_value_callback(keyname, keyvalue);
			}
		}

		node = node.next_sibling();
	}
}

void ubase::xml_catch_subnode_finished(const char *name)
{
	return;
}

void ubase::xml_catch_subnode_attribute(const char *attrname, const char* attrvalue)
{
	return;
}

void ubase::xml_catch_rootnode_attribute(const char *attrname, const char* attrvalue)
{
	return;
}

void ubase::xml_get_string(std::string& xml, bool include_xml_declaration) const
{
	if (xdoc.get() == NULL)
		return;

	std::ostringstream oss;
	xdoc->save(oss);

	if (include_xml_declaration)
		xml = oss.str();
	else
	{
		std::string str = oss.str();
		size_t pos = str.find("<", 5);
		const char *p = str.c_str();
		if (pos != std::string::npos)
			xml.assign(p + pos);
		else
			xml.assign(p);
	}
};

std::string ubase::xml_createstring(bool include_xml_declaration)
{
	std::string retval;
	xml_create();
	xml_get_string(retval, include_xml_declaration);
	return retval;
}

void ubase::xml_append_root(const char* keyname)
{
	xml_append_root(keyname, NULL);
}

void ubase::xml_append_root(const char* keyname, const char* text)
{
	if (keyname == NULL)
		return;

	xml_init();

	const char* stylesheet = xml_get_stylesheet();
	if (stylesheet != NULL)
	{
		xml_append_stylesheet(stylesheet);
	}

	pugi::xml_node root = xdoc->root().append_child(keyname);

	if (text != NULL)
	{
		root.append_child(pugi::node_pcdata).set_value(text);
	}
	rootname.assign(keyname);
}

void ubase::xml_append_stylesheet(const char *value)
{
	pugi::xml_node xmlStylesheetElement = xdoc->append_child(pugi::node_pi);
	xmlStylesheetElement.set_name("xml-stylesheet");
	xmlStylesheetElement.set_value(value);
}

void ubase::xml_append_child(pugi::xml_node& n)
{
	check_xdoc();

	pugi::xml_node node = xdoc->root().child(rootname.c_str());
	node.append_copy(n);
}

void ubase::xml_append_node(const char* keyname, const char* value)
{
	check_xdoc();

	xml_append_data(keyname, value);
}

void ubase::xml_append_node(const char* keyname, const char* value, const char* default_value)
{
	if ((value == NULL) || (default_value == NULL))
		return;

	if (strcmp(value, default_value) == 0)
		return;

	xml_append_node(keyname, value);
}

void ubase::xml_append_node(const char* keyname, const gstring& value)
{
	check_xdoc();

	std::string value_utf8 = value.getutf8();
	xml_append_data(keyname, value_utf8.c_str());
}

void ubase::xml_append_node(const char* keyname, const gstring& value, const gstring& default_value)
{
	if (value == default_value)
		return;

	xml_append_node(keyname, value);
}

void ubase::xml_append_node(const char* keyname, bool value)
{
	check_xdoc();

	const char *p = value ? ubase::xml_true : ubase::xml_false;
	xml_append_data(keyname, p);
}

void ubase::xml_append_node(const char* keyname, bool value, bool default_value)
{
	if (value == default_value)
		return;

	xml_append_node(keyname, value);
}

void ubase::xml_append_node(const char* keyname, const addrip_v4& value)
{
	check_xdoc();

	std::string addr = value.to_string();
	xml_append_data(keyname, addr.c_str());
}

void ubase::xml_append_node(const char* keyname, const addrip_v4& value, const addrip_v4& default_value)
{
	if (value == default_value)
		return;

	xml_append_node(keyname, value);
}

void ubase::xml_append_node(const char* keyname, const std::vector<std::string>& value)
{
	check_xdoc();

	std::vector<std::string>::const_iterator iter;
	for (iter = value.begin(); iter != value.end(); ++iter)
	{
		xml_append_data(keyname, (*iter).c_str());
	}
}

void ubase::xml_append_node(const char* keyname, const std::list<std::string>& value)
{
	check_xdoc();

	std::list<std::string>::const_iterator iter;
	for (iter = value.begin(); iter != value.end(); ++iter)
	{
		xml_append_data(keyname, (*iter).c_str());
	}
}

bool ubase::xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, std::string& wanted_keyvalue)
{
	if ((current_keyname == NULL) || (wanted_keyname == NULL))
		return false;

	if (strcmp(current_keyname, wanted_keyname) != 0)
		return false;

	if (current_keyvalue != NULL)
		wanted_keyvalue.assign(current_keyvalue);
	else
		wanted_keyvalue.clear();

	return true;
}

bool ubase::xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, gstring& wanted_keyvalue)
{
	if (!xml_check_args(current_keyname, wanted_keyname))
		return false;

	if (current_keyvalue != NULL)
		wanted_keyvalue.assign_fromutf8(current_keyvalue);
	else
		wanted_keyvalue.clear();

	return true;
}

bool ubase::xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, bool& wanted_keyvalue)
{
	if (!xml_check_args(current_keyname, wanted_keyname))
		return false;

	if (current_keyvalue != NULL)
	{
		if (strcmp(current_keyvalue, ubase::xml_true) == 0)
			wanted_keyvalue = true;
		else
			wanted_keyvalue = false;
	}
	else
		wanted_keyvalue = false;

	return true;
}

bool ubase::xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, addrip_v4& wanted_keyvalue)
{
	if (!xml_check_args(current_keyname, wanted_keyname))
		return false;

	if (current_keyvalue != NULL)
	{
		wanted_keyvalue.from_string(current_keyvalue);
	}
	else
		wanted_keyvalue.clear();

	return true;
}

bool ubase::xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, short& wanted_keyvalue)
{
	if (!xml_check_args(current_keyname, wanted_keyname))
		return false;

	wanted_keyvalue = fastformat::parse_short(current_keyvalue);

	return true;
}

bool ubase::xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, unsigned short& wanted_keyvalue)
{
	if (!xml_check_args(current_keyname, wanted_keyname))
		return false;

	wanted_keyvalue = fastformat::parse_ushort(current_keyvalue);

	return true;
}

bool ubase::xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, int& wanted_keyvalue)
{
	if (!xml_check_args(current_keyname, wanted_keyname))
		return false;

	wanted_keyvalue = fastformat::parse_int(current_keyvalue);

	return true;
}

bool ubase::xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, unsigned int& wanted_keyvalue)
{
	if (!xml_check_args(current_keyname, wanted_keyname))
		return false;

	wanted_keyvalue = fastformat::parse_uint(current_keyvalue);

	return true;
}

bool ubase::xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, std::int64_t& wanted_keyvalue)
{
	if (!xml_check_args(current_keyname, wanted_keyname))
		return false;

	wanted_keyvalue = fastformat::parse_int64(current_keyvalue);

	return true;
}

bool ubase::xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, std::uint64_t& wanted_keyvalue)
{
	if (!xml_check_args(current_keyname, wanted_keyname))
		return false;

	wanted_keyvalue = fastformat::parse_uint64(current_keyvalue);

	return true;
}

bool ubase::xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, std::atomic_uint_fast32_t& wanted_keyvalue)
{
	if (!xml_check_args(current_keyname, wanted_keyname))
		return false;

	wanted_keyvalue.store(fastformat::parse_uint(current_keyvalue));

	return true;
}

bool ubase::xml_check_value(const char* current_keyname, const char* wanted_keyname, const char* current_keyvalue, std::atomic_uint_fast64_t& wanted_keyvalue)
{
	if (!xml_check_args(current_keyname, wanted_keyname))
		return false;

	wanted_keyvalue.store(fastformat::parse_uint64(current_keyvalue));

	return true;
}

inline bool ubase::xml_check_args(const char* current_keyname, const char* wanted_keyname)
{
	if ((current_keyname == NULL) || (wanted_keyname == NULL))
		return false;

	if (strcmp(current_keyname, wanted_keyname) != 0)
		return false;

	return true;
}

void ubase::xml_save(const gstring& filename)
{
	std::string str = filename.getmb();
	xml_save(str.c_str());
}

void ubase::xml_save(const std::string& filename)
{
	xml_save(filename.c_str());
}

void ubase::xml_save(const char *filename)
{
	std::string xml;
	xml_create();
	xml_get_string(xml, true);

	boost::filesystem::path path(filename);
	boost::filesystem::path dir = path.parent_path();

	if (!boost::filesystem::is_directory(dir))
		boost::filesystem::create_directories(dir);

	std::fstream f;
	f.open(filename, std::fstream::out);
	if (f)
	{
		f << xml << std::endl;
		f.close();
	}
	else
	{
		std::string e("Unable to open for writing file: ");
		e.append(filename);
		throw std::exception(e.c_str());
	}
}

void ubase::xml_save_debug(const gstring& folder, const char *id, bool include_timestamp)
{
	gstring fullpath;
	gstring classname(get_this_class_name());

	if (id != NULL)
	{
		classname.append("_");
		classname.append(id);
	}

	if (include_timestamp)
	{
		utime now(true);
		classname.append("_");
		classname.append(now.to_string(utime::format_yyyymmddhhmmss));
	}

	classname.append(".xml");

	xml_save(ufs::make_full_filepath(folder, classname));
}

void ubase::xml_save_debug(const char* folder, const char *id, bool include_timestamp)
{
	xml_save_debug(gstring(folder), id, include_timestamp);
}

void ubase::xml_load(const gstring& filename)
{
	xml_load(filename.getmb().c_str());
}

void ubase::xml_load(const std::string& filename)
{
	xml_load(filename.c_str());
}

void ubase::xml_load(const char *filename)
{
	std::string xml;
	std::ifstream infile (filename, std::ios_base::in);
	if (infile)
	{
		std::ostringstream content;
		std::string line;

		while (getline(infile, line, '\n'))
		{
			content << line;
		}
		infile.close();

		xml = content.str();
	}
	else
	{
		std::ostringstream oss;
		oss << "Unable to open file \"" << filename << "\" for reading";
		std::string what = oss.str();
		throw std::exception(what.c_str());
	}

	bool ok = xml_parse(xml.c_str(), false);
	if (!ok)
	{
		std::ostringstream oss;
		oss << "Unable to parse file \"" << filename << "\". Possible, corrupted xml file.";
		std::string what = oss.str();
		throw std::exception(what.c_str());
	}
}

void ubase::add_class_name()
{
	pugi::xml_node root = xml_get_root();
	pugi::xml_attribute attr = root.append_attribute("CN");
	attr.set_value(get_this_class_name());
}

#ifdef UTM_WIN
LONG ubase::CreateRegistry(const TCHAR* pRegistryPath, const HKEY hk)
{
	return RegistryHelper::CreateKey(hk, pRegistryPath);
}

LONG ubase::RegistrySave(HKEY hk, const char* keyname, const char* value) const
{
	return RegistryHelper::SetValueString(hk, keyname, value);
}

LONG ubase::RegistrySave(HKEY hk, const char* keyname, const std::string& value) const
{
	return RegistryHelper::SetValueString(hk, keyname, value);
}

LONG ubase::RegistrySave(HKEY hk, const char* keyname, const utm::gstring& value) const
{
	return RegistryHelper::SetValueString(hk, keyname, value);
}

LONG ubase::RegistrySave(HKEY hk, const char* keyname, bool value) const
{
	DWORD dw = value ? 1 : 0;
	return RegistryHelper::SetValueDword(hk, keyname, dw);
}

LONG ubase::RegistrySave(HKEY hk, const char* keyname, int value) const
{
	DWORD dw = static_cast<DWORD>(value);
	return RegistryHelper::SetValueDword(hk, keyname, dw);
}

LONG ubase::RegistrySave(HKEY hk, const char* keyname, unsigned int value) const
{
	return RegistryHelper::SetValueDword(hk, keyname, value);
}

LONG ubase::RegistryLoad(HKEY hk, const char* keyname, std::string& value)
{
	gstring p(keyname);
	std::unique_ptr<TCHAR> sz(new TCHAR[MAX_REGISTRY_VALUE]);
	LONG retval = RegistryHelper::QueryValueString(hk, p.c_str(), sizeof(TCHAR)*MAX_REGISTRY_VALUE, sz.get());
	if (retval == ERROR_SUCCESS)
	{
		value = gstring(sz.get()).getmb();
	};
	return retval;
}

LONG ubase::RegistryLoad(HKEY hk, const char* keyname, gstring& value)
{
	gstring p(keyname);
	std::unique_ptr<TCHAR> sz(new TCHAR[MAX_REGISTRY_VALUE]);
	LONG retval = RegistryHelper::QueryValueString(hk, p.c_str(), sizeof(TCHAR)*MAX_REGISTRY_VALUE, sz.get());
	if (retval == ERROR_SUCCESS)
	{
		value.assign(sz.get());
	};
	return retval;
}

LONG ubase::RegistryLoad(HKEY hk, const char* keyname, bool& value)
{
	gstring p(keyname);
	DWORD dw;
	LONG retval = RegistryHelper::QueryValueDword(hk, p.c_str(), dw);
	if (retval == ERROR_SUCCESS)
	{
		value = dw > 0 ? true : false;
	};
	return retval;
}

LONG ubase::RegistryLoad(HKEY hk, const char* keyname, int& value)
{
	gstring p(keyname);
	DWORD dw;
	LONG retval = RegistryHelper::QueryValueDword(hk, p.c_str(), dw);
	if (retval == ERROR_SUCCESS)
	{
		value = static_cast<DWORD>(dw);
	};
	return retval;
}

LONG ubase::RegistryLoad(HKEY hk, const char* keyname, unsigned int& value)
{
	gstring p(keyname);
	DWORD dw;
	LONG retval = RegistryHelper::QueryValueDword(hk, p.c_str(), dw);
	if (retval == ERROR_SUCCESS)
	{
		value = dw;
	};
	return retval;
}

LONG ubase::ReadFromFileOrRegistry(const std::string& configpath, const TCHAR *pRegistryPath, const HKEY hkey)
{
	if ((configpath.empty()) || (!boost::filesystem::exists(configpath)))
	{
		LONG retval = ERROR_SUCCESS;
		if (pRegistryPath != NULL)
		{
			retval = ReadFromRegistry(pRegistryPath, hkey);
			if (retval != ERROR_SUCCESS)
			{
				throw std::exception("Unable to open registry path for reading.");
			}
		}
		return retval;
	}

	xml_load(configpath);

	return ERROR_SUCCESS;
}

LONG ubase::SaveToFileOrRegistry(const std::string& configpath, const TCHAR *pRegistryPath, const HKEY hkey)
{
	if ((configpath.empty()) || (!boost::filesystem::exists(configpath)))
	{
		LONG retval = ERROR_SUCCESS;
		if (pRegistryPath != NULL)
		{
			retval = SaveToRegistry(pRegistryPath, hkey);
			if (retval != ERROR_SUCCESS)
			{
				throw std::exception("Unable to open registry path for writing.");
			}
		}
		return retval;
	}

	xml_save(configpath);

	return ERROR_SUCCESS;
}
#endif // UTM_WIN
}