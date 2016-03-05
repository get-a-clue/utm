#ifndef _UTM_BLOB_H
#define _UTM_BLOB_H

#pragma once
#include <utm.h>

#include <memory>
#include <string>

namespace utm {

class blob
{
public:
	static const char this_class_name[];

	blob();
	blob(const blob& rhs);
	~blob();

	blob& operator=(const blob& rhs);
	bool operator==(const blob& rhs) const;

	void clear();

	void set(unsigned char* data, size_t len);
	unsigned char* get() const;
	size_t size() const { return datasize; };

	void to_string(std::string& str) const;
	void from_string(const char* str);

	friend std::ostream&  operator<<(std::ostream& s, const blob& rhs)
	{
		std::string str;
		rhs.to_string(str);
		s << str;
		return s;
	}

	friend std::istream& operator>>(std::istream& s, blob& obj)
	{
		char ch;
		std::string str;

		while (!s.eof() && !s.fail())
		{
			// read characters from the stream
			s.get(ch);

			// EOF of stream?
			if (s.rdstate() & (std::ios::eofbit | std::ios::badbit | std::ios::failbit))
				break;

			str.push_back(ch);
		}

		obj.from_string(str.c_str());

		return s;
	}

private:
	size_t datasize;
	std::unique_ptr<unsigned char> dataptr;

public:
#ifdef UTM_DEBUG
	
#endif
};

}

#endif // _UTM_BLOB_H