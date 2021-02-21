#include "trie.h"

extern inline void trie_init(trie* t) {
	t->type = 0;
	for (int i = 0; i < 256; i++) {
		t->children[i] = 0;
	}
}

extern inline void trie_del(trie* t) {
	for (int i = 0; i < 256; i++) {
		if (t->children[i]) {
			trie_del(t->children[i]);
			free(t->children[i]);
		}
	}
}

trie* trie_insert(trie* t, const char* key, uint8_t type) {
	if (!(*key)) {
		t->type = type;
		return t;
	}
	if (!(t->children[ (uint8_t) *key])) {
		t->children[ (uint8_t) *key] = (trie*) malloc(sizeof(trie));
		if (!(t->children[ (uint8_t) *key])) {
			return NULL;
		}
		trie_init(t->children[ (uint8_t) *key]);
	}
	return trie_insert(t->children[ (uint8_t) *key], key + 1, type);
}

trie* trie_find(trie* t, const char* key) {
	if (!(*key)) {
		if (t->type)
			return t;
		else
			return NULL;
	}
	if (!(t->children[ (uint8_t) *key])) {
		return NULL;
	}
	return trie_find(t->children[ (uint8_t) *key], key + 1);
}