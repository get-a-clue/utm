#include "StdAfx.h"
#include "punycoder.h"

#include <ubase_test.h>
#include <stringtools.h>

#include <vector>
#include <string>

namespace utm {

const char punycoder::this_class_name[] = "punycoder";

punycoder::punycoder(void)
{
}

punycoder::~punycoder(void)
{
}

punycode_uint punycoder::decode_digit(punycode_uint cp)
{
	return  cp - 48 < 10 ? cp - 22 :  cp - 65 < 26 ? cp - 65 :
			cp - 97 < 26 ? cp - 97 :  base;
}

char punycoder::encode_digit(punycode_uint d, int flag)
{
	return d + 22 + 75 * (d < 26) - ((flag != 0) << 5);
		/*  0..25 map to ASCII a..z or A..Z */
		/* 26..35 map to ASCII 0..9         */
}

char punycoder::encode_basic(punycode_uint bcp, int flag)
{
		bcp -= (bcp - 97 < 26) << 5;
		return bcp + ((!flag && (bcp - 65 < 26)) << 5);
}

punycode_uint punycoder::adapt (punycode_uint delta, punycode_uint numpoints, int firsttime)
{
	punycode_uint k;

	delta = firsttime ? delta / damp : delta >> 1;
	/* delta >> 1 is a faster way of doing delta / 2 */
	delta += delta / numpoints;

	for (k = 0;  delta > ((base - tmin) * tmax) / 2;  k += base) {
		delta /= base - tmin;
	}

	return k + (base - tmin + 1) * delta / (delta + skew);
}

enum punycode_status punycoder::punycode_encode(
  punycode_uint input_length,
  const punycode_uint input[],
  const unsigned char case_flags[],
  punycode_uint *output_length,
  char output[] )
{
  punycode_uint n, delta, h, b, out, max_out, bias, j, m, q, k, t;

  /* Initialize the state: */

  n = initial_n;
  delta = out = 0;
  max_out = *output_length;
  bias = initial_bias;

  /* Handle the basic code points: */

  for (j = 0;  j < input_length;  ++j) {
    if (basic(input[j])) {
      if (max_out - out < 2) return punycode_big_output;
      output[out++] =
        case_flags ?  encode_basic(input[j], case_flags[j]) : input[j];
    }
    /* else if (input[j] < n) return punycode_bad_input; */
    /* (not needed for Punycode with unsigned code points) */
  }

  h = b = out;

  /* h is the number of code points that have been handled, b is the  */
  /* number of basic code points, and out is the number of characters */
  /* that have been output.                                           */

  if (b > 0) output[out++] = delimiter;

  /* Main encoding loop: */

  while (h < input_length) {
    /* All non-basic code points < n have been     */
    /* handled already.  Find the next larger one: */

    for (m = maxint, j = 0;  j < input_length;  ++j) {
      /* if (basic(input[j])) continue; */
      /* (not needed for Punycode) */
      if (input[j] >= n && input[j] < m) m = input[j];
    }

    /* Increase delta enough to advance the decoder's    */
    /* <n,i> state to <m,0>, but guard against overflow: */

    if (m - n > (maxint - delta) / (h + 1)) return punycode_overflow;
    delta += (m - n) * (h + 1);
    n = m;

    for (j = 0;  j < input_length;  ++j) {
      /* Punycode does not need to check whether input[j] is basic: */
      if (input[j] < n /* || basic(input[j]) */ ) {
        if (++delta == 0) return punycode_overflow;
      }

      if (input[j] == n) {
        /* Represent delta as a generalized variable-length integer: */

        for (q = delta, k = base;  ;  k += base) {
          if (out >= max_out) return punycode_big_output;

          t = k <= bias /* + tmin */ ? tmin :     /* +tmin not needed */
              k >= bias + tmax ? tmax : k - bias;
          if (q < t) break;
          output[out++] = encode_digit(t + (q - t) % (base - t), 0);
          q = (q - t) / (base - t);
        }

        output[out++] = encode_digit(q, case_flags && case_flags[j]);
        bias = adapt(delta, h + 1, h == b);
        delta = 0;
        ++h;
      }
    }

    ++delta, ++n;
  }

  *output_length = out;
  return punycode_success;
}

/*** Main decode function ***/

enum punycode_status punycoder::punycode_decode(
  punycode_uint input_length,
  const char input[],
  punycode_uint *output_length,
  punycode_uint output[],
  unsigned char case_flags[] )
{
  punycode_uint n, out, i, max_out, bias,
                 b, j, in, oldi, w, k, digit, t;

  /* Initialize the state: */

  n = initial_n;
  out = i = 0;
  max_out = *output_length;
  bias = initial_bias;

  /* Handle the basic code points:  Let b be the number of input code */
  /* points before the last delimiter, or 0 if there is none, then    */
  /* copy the first b code points to the output.                      */

  for (b = j = 0;  j < input_length;  ++j) if (delim(input[j])) b = j;
  if (b > max_out) return punycode_big_output;

  for (j = 0;  j < b;  ++j) {

    if (case_flags) case_flags[out] = flagged(input[j]);
    if (!basic(input[j])) return punycode_bad_input;
    output[out++] = input[j];
  }

  /* Main decoding loop:  Start just after the last delimiter if any  */
  /* basic code points were copied; start at the beginning otherwise. */

  for (in = b > 0 ? b + 1 : 0;  in < input_length;  ++out) {

    /* in is the index of the next character to be consumed, and */
    /* out is the number of code points in the output array.     */

    /* Decode a generalized variable-length integer into delta,  */
    /* which gets added to i.  The overflow checking is easier   */
    /* if we increase i as we go, then subtract off its starting */
    /* value at the end to obtain delta.                         */

    for (oldi = i, w = 1, k = base;  ;  k += base) {
      if (in >= input_length) return punycode_bad_input;
      digit = decode_digit(input[in++]);
      if (digit >= base) return punycode_bad_input;
      if (digit > (maxint - i) / w) return punycode_overflow;
      i += digit * w;
      t = k <= bias /* + tmin */ ? tmin :     /* +tmin not needed */
          k >= bias + tmax ? tmax : k - bias;
      if (digit < t) break;
      if (w > maxint / (base - t)) return punycode_overflow;
      w *= (base - t);
    }

    bias = adapt(i - oldi, out + 1, oldi == 0);

    /* i was supposed to wrap around from out+1 to 0,   */
    /* incrementing n each time, so we'll fix that now: */

    if (i / (out + 1) > maxint - n) return punycode_overflow;
    n += i / (out + 1);
    i %= (out + 1);

    /* Insert n at position i of the output: */

    /* not needed for Punycode: */
    /* if (decode_digit(n) <= base) return punycode_invalid_input; */
    if (out >= max_out) return punycode_big_output;

    if (case_flags) {
      memmove(case_flags + i + 1, case_flags + i, out - i);

      /* Case of last character determines uppercase flag: */
      case_flags[i] = flagged(input[in - 1]);
    }

    memmove(output + i + 1, output + i, (out - i) * sizeof *output);
    output[i++] = n;
  }

  *output_length = out;
  return punycode_success;
}

enum punycode_status punycoder::punycode_decode(const char *input_str, gstring& output_str)
{
	if (input_str == NULL)
		return punycode_success;

	if (strncmp(input_str, "xn--", 4) == 0)
	{
		input_str += 4;
	}

	unsigned char case_flags[256];
	memset(case_flags, 0, sizeof(case_flags));

	unsigned int output_length = 256;
	punycode_uint output[256];
	memset(output, 0, sizeof(output));

	punycode_status retval = punycode_decode(strlen(input_str), input_str, &output_length, output, case_flags);

	if (retval == punycode_success)
	{
		WCHAR wsz[256];

		unsigned int i;
		for (i = 0; i < output_length; i++)
		{
			wsz[i] = (unsigned short)output[i];
			if (i >= 255)
				break;
		}
		wsz[i] = 0;

		output_str.assign(wsz);
	}

	return retval;
}

enum punycode_status punycoder::punycode_encode(const gstring& input_str, std::string& output_str)
{
	output_str.clear();

	std::vector<utm::ugstring> parts;
	utm::stringtools::split(parts, input_str.c_str(), '.');

	punycode_status retval;
	unsigned char case_flags[256];
	punycode_uint input[256];
	char output[256];

	for (std::vector<utm::ugstring>::iterator iter = parts.begin(); iter != parts.end(); ++iter)
	{
		memset(case_flags, 0, sizeof(case_flags));
		memset(input, 0, sizeof(input));

		gstring gsz(*iter);
		std::wstring wsz = gsz.getw();

		size_t len = wsz.size();
		if (len > 255) len = 255;
		for (size_t i = 0; i < len; i++)
		{
			input[i] = wsz[i];
		}

		memset(output, 0, sizeof(output));
		punycode_uint output_len = 256;

		retval = punycode_encode(len, input, case_flags, &output_len, output);
		if (retval != punycode_success)
			break;

		if (!output_str.empty())
			output_str.append(".");

		output_str.append("xn--");
		output_str.append(output);
	}


	return retval;
}

void punycoder::test_all()
{
	test_case::classname.assign(this_class_name);
	test_case::testcase_num = 1;

	{
		std::string d1("המלום");
		std::string r1;
		utm::punycoder::punycode_encode(gstring(d1), r1);
		TEST_CASE_CHECK(r1, std::string("xn--d1acufc"));
	}

	{
		std::string d1("המלום.נפ");
		std::string r1;
		utm::punycoder::punycode_encode(gstring(d1), r1);
		TEST_CASE_CHECK(r1, std::string("xn--d1acufc.xn--p1ai"));
	}
/*
	{
		std::string d1("www.המלום.נפ");
		std::string r1;
		utm::punycoder::punycode_encode(gstring(d1), r1);
		TEST_CASE_CHECK(r1, std::string("www.xn--d1acufc.xn--p1ai"));
	}
*/
	{
		std::string d1("xn--d1acufc");
		gstring u1;
		utm::punycoder::punycode_decode(d1.c_str(), u1);
		TEST_CASE_CHECK(u1.getmb(), std::string("המלום"));
	}
}

}
