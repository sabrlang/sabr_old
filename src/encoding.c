#include "encoding.h"

extern inline bool is_surrogate(char16_t wc) {
	return (wc - 0xd800u) < 2048u;
}

extern inline bool is_high_surrogate(char16_t wc) {
	return (wc & 0xfffffc00) == 0xd800;
}

extern inline bool is_low_surrogate(char16_t wc) {
	return (wc & 0xfffffc00) == 0xdc00;
}

extern inline char32_t surrogates_to_utf32(char16_t high, char16_t low) {
	return (high << 10) + low - 0x35fdc00;
} 