#ifndef _UTM_ADDRMAC_H
#define _UTM_ADDRMAC_H

#pragma once
#include <utm.h>

#include <string>

#ifndef MAX_ETHER_LENGTH
#define MAX_ETHER_LENGTH 6
#endif

namespace utm {
class addrmac
{
public:
	static const char this_class_name[];

	addrmac();
	~addrmac();
	addrmac(const char *p);
	addrmac(const std::string& str);

	bool operator <(const addrmac& rhs) const;
	bool operator ==(const addrmac& rhs) const;
	bool operator !=(const addrmac& rhs) const;
	bool is_equal(const unsigned char *p) const;

	friend std::ostream& operator<<(std::ostream& s, const addrmac& rhs);
	friend std::istream& operator>>(std::istream& s, addrmac& obj);

	void clear();
	bool is_zero() const;
	void set(const unsigned char *p);
	const unsigned char* get() const { return content; };

	const std::string to_string() const;
	inline void to_string(char* buffer) const;
	void as_hex(char* buffer) const;
	bool from_string(const char *str);
	bool from_string(const std::string& str);

private:
	unsigned char content[MAX_ETHER_LENGTH];

#ifdef UTM_DEBUG
public:
	static void test_all();
#endif
};

}

#endif // _UTM_ADDRMAC_H