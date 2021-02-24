#ifndef __TRIE_H__
#define __TRIE_H__

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "value.h"

typedef enum word_text_type_enum {
	WTT_NONE,
	WTT_OP,
	WTT_CTRL,
	WTT_BIF,
	WTT_KWRD,
	WTT_COMP
} word_text_type;

typedef struct trie_struct trie;

struct trie_struct {
	value data;
	trie* children[256];
	uint8_t type;
};

void trie_init(trie* t);
void trie_del(trie* t);
trie* trie_insert(trie* t, const char* key, uint8_t type);
trie* trie_find(trie* t, const char* key);

#endif