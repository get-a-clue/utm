#include "stdafx.h"
#include "blob.h"

#include <ubase_test.h>
#include <base64.h>

namespace utm {

const char blob::this_class_name[] = "blob";

blob::blob()
{
	clear();
}

blob::blob(const blob& rhs)
{
	operator=(rhs);
}

blob::~blob()
{
}

blob& blob::operator=(const blob& rhs)
{
	if ((rhs.datasize > 0) && (rhs.dataptr.get() != NULL))
	{
		unsigned char* p = new unsigned char[rhs.datasize];
		memcpy(p, rhs.get(), rhs.datasize);
		dataptr.reset(p);
		datasize = rhs.datasize;
	}
	else
	{
		dataptr.reset(NULL);
		datasize = 0;
	}

	return *this;
}

bool blob::operator==(const blob& rhs) const
{
	if (datasize != rhs.datasize)
		return false;

	if (datasize == 0)
		return true;

	return (memcmp(dataptr.get(), rhs.dataptr.get(), datasize) == 0);
}

void blob::clear()
{
	datasize = 0;
	if (dataptr.get() != NULL)
	{
		dataptr.reset(NULL);
	}
}

void blob::set(unsigned char* data, size_t len)
{
	if ((len > 0) && (data != NULL))
	{
		unsigned char* p = new unsigned char[len];
		memcpy(p, data, len);
		dataptr.reset(p);
		datasize = len;
	}
	else
	{
		dataptr.reset(NULL);
		datasize = 0;
	}
}

unsigned char* blob::get() const
{
	return dataptr.get();
}

void blob::to_string(std::string& str) const
{
	if ((datasize > 0) && (dataptr.get() != NULL))
	{
		base64::encode(dataptr.get(), datasize, str);
	}
}

void blob::from_string(const char* str)
{
	if (str != NULL)
	{
		std::vector<unsigned char> v;
		base64::decode(str, v);
		if (v.size() > 0)
		{
			unsigned char* p = new unsigned char[v.size()];
			memcpy(p, v.data(), v.size());
			dataptr.reset(p);
			datasize = v.size();
		}
	}
}

#ifdef UTM_DEBUG
void blob::test_all()
{
	test_report tr(this_class_name);

	unsigned char r[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };
	blob b;
	b.set(&r[0], 10);
	
	std::string s;
	b.to_string(s);

	blob b2;
	b2.from_string(s.c_str());

	TEST_CASE_CHECK(b == b2, true);

	for (int i = 0; i < 10; i++)
	{
		unsigned char ch = static_cast<unsigned char>(i);
		TEST_CASE_CHECK(*(b2.get() + i), ch);
	}

}
#endif


}