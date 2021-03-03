#ifndef __ENCODING_H__
#define __ENCODING_H__

#include <stdbool.h>
#include <uchar.h>
#include <wchar.h>

bool is_surrogate(char16_t wc);
bool is_high_surrogate(char16_t wc);
bool is_low_surrogate(char16_t wc);
char32_t surrogates_to_utf32(char16_t high, char16_t low);

#endif