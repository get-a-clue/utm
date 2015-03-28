#ifndef _UTM_MD5_H
#define _UTM_MD5_h

#pragma once
#include <utm.h>

#include <string>

#define MD5_HASH_STRING_SIZE 32

namespace utm {

#ifndef  uint8
#define uint8   unsigned char
#endif

#ifndef  uint32
#define uint32  unsigned int
#endif

class md5_context
{
public:
	md5_context(void);
	~md5_context(void);

    uint32 total[2];
    uint32 state[4];
    uint8 buffer[64];
};

class md5_result
{
public:
	md5_result();
	~md5_result();

	uint8 digest[16];
	char as_string[33];
};

namespace md5 {
	void md5_update(md5_context *ctx, const uint8 *input, uint32 length);
	void md5_finish(md5_context *ctx, md5_result* result);
	void md5_string(const char* input, std::string& result);
	void md5_random(std::string& result);

#ifdef UTM_DEBUG
	void test_all();
#endif
}

}

#endif // _UTM_MD5_h