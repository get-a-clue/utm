#pragma once

#include <gstring.h>

namespace utm {

enum punycode_status {
  punycode_success,
  punycode_bad_input,   /* Input is invalid.                       */
  punycode_big_output,  /* Output would exceed the space provided. */
  punycode_overflow     /* Input needs wider integers to process.  */
};

typedef unsigned int punycode_uint;

enum { base = 36, tmin = 1, tmax = 26, skew = 38, damp = 700,
       initial_bias = 72, initial_n = 0x80, delimiter = 0x2D };

#define basic(cp) ((punycode_uint)(cp) < 0x80)
#define delim(cp) ((cp) == delimiter)
#define flagged(bcp) ((punycode_uint)(bcp) - 65 < 26)

class punycoder
{
public:
	static const char this_class_name[];

	punycoder(void);
	~punycoder(void);

	static punycode_status punycode_encode(const gstring& input_str, std::string& output_str);
	static punycode_status punycode_decode(const char *input_str, gstring& output_str);

private:
	static const punycode_uint maxint = -1;

	static enum punycode_status punycode_encode (punycode_uint input_length, const punycode_uint input[], const unsigned char case_flags[], punycode_uint *output_length, char output[] );
	static enum punycode_status punycode_decode (punycode_uint input_length, const char input[], punycode_uint *output_length, punycode_uint output[], unsigned char case_flags[] );

	static punycode_uint decode_digit(punycode_uint cp);
	static char encode_digit(punycode_uint d, int flag);
	static char encode_basic(punycode_uint bcp, int flag);
	static punycode_uint adapt (punycode_uint delta, punycode_uint numpoints, int firsttime);

public:
	static void test_all();
};

}
