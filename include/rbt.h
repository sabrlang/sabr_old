#ifndef __RBT_H__
#define __RBT_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum keyword_type_enum {
	KWRD_NONE,
	KWRD_FUNC,
	KWRD_MACRO,
	KWRD_VAR
} keyword_type;

typedef enum rbt_color_enum {
	RBT_RED,
	RBT_BLACK
} rbt_color;

typedef struct rbt_node_struct rbt_node;

struct rbt_node_struct {
	size_t index;
	rbt_node* right;
	rbt_node* left;
	rbt_node* parent;
	size_t data;
	uint8_t type;
	uint8_t color;
};

typedef struct rbt_struct {
	rbt_node* root;
	rbt_node* nil;
} rbt;

rbt_node* rbt_node_new(size_t index);
rbt* rbt_new();
void rbt_rotate_left(rbt* t, rbt_node* x);
void rbt_rotate_right(rbt* t, rbt_node* x);
void rbt_insertion_fixup(rbt* t, rbt_node* z);
void rbt_insert(rbt* t, rbt_node* z);
void rbt_transplant(rbt* t, rbt_node* u, rbt_node* v);
rbt_node* rbt_minimum(rbt* t, rbt_node* x);
void rbt_delete_fixup(rbt* t, rbt_node* x);
void rbt_delete(rbt* t, rbt_node* z);
rbt_node* rbt_search_recurse(rbt* t, rbt_node* n, size_t index);
rbt_node* rbt_search(rbt* t, size_t index);
void rbt_free_recurse(rbt* t, rbt_node* n);
void rbt_free(rbt* t);

#endif