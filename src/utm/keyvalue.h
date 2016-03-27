#ifndef _UTM_KEYVALUE_H
#define _UTM_KEYVALUE_H

#pragma once

#include <string>
#include <ubase.h>

#define  KEYVALUE_XMLTAG_ROOT "Item"

namespace utm {

template<typename T>
class keyvalue : public ubase
{
public:
	keyvalue() {};
	~keyvalue() {};

	bool operator==(const keyvalue& rhs) const
	{
		if (key != rhs.key) return false;
		return (value == rhs.value);
	}

	virtual const char *get_this_class_name() const { return "keyvalue"; };

	unsigned int id;
	T key;
	T value;

	unsigned int get_id() const { return id; };
	void set_id(unsigned int _id) { id = _id; }
	const T& get_name() const { return key; }

public:
	void clear()
	{
		key.clear();
		value.clear();
	}

	void xml_create()
	{
		keyvalue orig;
		xml_append_root(KEYVALUE_XMLTAG_ROOT);
		xml_append_node("Id", get_id(), orig.get_id());
		xml_append_node("Key", key, orig.key);
		xml_append_node("Value", value, orig.value);
	}

	void xml_catch_value(const char *keyname, const char *keyvalue)
	{
		if (xml_check_value(keyname, "Id", keyvalue, id)) return;
		if (xml_check_value(keyname, "Key", keyvalue, key)) return;
		if (xml_check_value(keyname, "Value", keyvalue, value)) return;
	}

	virtual ubase* xml_catch_subnode(const char *name) { return NULL; };
};

}

#endif // _UTM_KEYVALUE_H
