#include "rbt.h"

rbt_node* rbt_node_new(size_t index) {
	rbt_node* n = (rbt_node*) malloc(sizeof(rbt_node));
	if (!n) return NULL;
	n->left = NULL;
	n->right = NULL;
	n->parent = NULL;
	n->color = RBT_RED;
	n->index = index;
	n->data = 0;
	n->type = 0;
	return n;
}

rbt* rbt_new() {
	rbt* t = (rbt*) malloc(sizeof(rbt));
	if (!t) return NULL;
	rbt_node* nil_node = (rbt_node*) malloc(sizeof(rbt_node));
	if (!nil_node) { free(t); return NULL; }
	nil_node->left = NULL;
	nil_node->right = NULL;
	nil_node->parent = NULL;
	nil_node->color = RBT_BLACK;
	nil_node->index = 0;
	t->nil = nil_node;
	t->root = t->nil;
	return t;
}

void rbt_rotate_left(rbt* t, rbt_node* x) {
	rbt_node* y = x->right;
	x->right = y->left;
	if (y->left != t->nil) y->left->parent = x;

	y->parent = x->parent;
	if (x->parent == t->nil) { t->root = y; }
	else if (x == x->parent->left) { x->parent->left = y; }
	else { x->parent->right = y; }

	y->left = x;
	x->parent = y;
}

void rbt_rotate_right(rbt* t, rbt_node* x) {
	rbt_node* y = x->left;
	x->left = y->right;
	if (y->right != t->nil) y->right->parent = x;

	y->parent = x->parent;
	if (x->parent == t->nil) { t->root = y; }
	else if (x == x->parent->right) { x->parent->right = y; }
	else { x->parent->left = y; }

	y->right = x;
	x->parent = y;
}

void rbt_insertion_fixup(rbt* t, rbt_node* z) {
	while (z->parent->color == RBT_RED) {
		if (z->parent == z->parent->parent->left) {
			rbt_node* y = z->parent->parent->right;

			if (y->color == RBT_RED) {
				y->color = RBT_BLACK;
				z->parent->color = RBT_BLACK;
				z->parent->parent->color = RBT_RED;
				z = z->parent->parent;
			}
			else {
				if (z == z->parent->right) {
					z = z->parent;
					rbt_rotate_left(t, z);
				}
				z->parent->color = RBT_BLACK;
				z->parent->parent->color = RBT_RED;
				rbt_rotate_right(t, z->parent->parent);
			}
		}
		else {
			rbt_node* y = z->parent->parent->left;

			if (y->color == RBT_RED) {
				y->color = RBT_BLACK;
				z->parent->color = RBT_BLACK;
				z->parent->parent->color = RBT_RED;
				z = z->parent->parent;
			}
			else {
				if (z == z->parent->left) {
					z = z->parent;
					rbt_rotate_right(t, z);
				}
				z->parent->color = RBT_BLACK;
				z->parent->parent->color = RBT_RED;
				rbt_rotate_left(t, z->parent->parent);
			}
		}
	}
	t->root->color = RBT_BLACK;
}

void rbt_insert(rbt* t, rbt_node* z) {
	rbt_node* y = t->nil;
	rbt_node* temp = t->root;

	while (temp != t->nil) {
		y = temp;
		if (z->index < temp->index) temp = temp->left;
		else temp = temp->right;
	}
	z->parent = y;

	if (y == t->nil) t->root = z;
	else if (z->index < y->index) y->left = z;
	else y->right = z;

	z->right = t->nil;
	z->left = t->nil;

	rbt_insertion_fixup(t, z);
}

void rbt_transplant(rbt* t, rbt_node* u, rbt_node* v) {
	if (u->parent = t->nil) t->root = v;
	else if (u == u->parent->left) u->parent->left = v;
	else u->parent->right = v;
	v->parent = u->parent;
}

rbt_node* rbt_minimum(rbt* t, rbt_node* x) {
	while (x->left != t->nil) x = x->left;
	return x;
}

void rbt_delete_fixup(rbt* t, rbt_node* x) {
	while (x != t->root && x->color == RBT_BLACK) {
		if (x == x->parent->left) {
			rbt_node* w = x->parent->right;
			if (w->color == RBT_RED) {
				w->color = RBT_BLACK;
				x->parent->color = RBT_RED;
				rbt_rotate_left(t, x->parent);
				w = x->parent->right;
			}
			if (w->left->color == RBT_BLACK && w->right->color == RBT_BLACK) {
				w->color = RBT_RED;
				x = x->parent;
			}
			else {
				if (w->right->color == RBT_BLACK) {
					w->left->color = RBT_BLACK;
					w->color = RBT_RED;
					rbt_rotate_right(t, w);
					w = x->parent->right;
				}
				w->color = x->parent->color;
				x->parent->color = RBT_BLACK;
				w->right->color = RBT_BLACK;
				rbt_rotate_left(t, x->parent);
				x = t->root;
			}
		}
		else {
			rbt_node* w = x->parent->left;
			if (w->color == RBT_RED) {
				w->color = RBT_BLACK;
				x->parent->color = RBT_RED;
				rbt_rotate_right(t, x->parent);
				w = x->parent->left;
			}
			if (w->right->color == RBT_BLACK && w->left->color == RBT_BLACK) {
				w->color = RBT_RED;
				x = x->parent;
			}
			else {
				if (w->left->color == RBT_BLACK) {
					w->right->color = RBT_BLACK;
					w->color = RBT_RED;
					rbt_rotate_left(t, w);
					w = x->parent->left;
				}
				w->color = x->parent->color;
				x->parent->color = RBT_BLACK;
				w->left->color = RBT_BLACK;
				rbt_rotate_right(t, x->parent);
				x = t->root;
			}
		}
	}
	x->color = RBT_BLACK;
}

void rbt_delete(rbt* t, rbt_node* z) {
	rbt_node* y = z;
	rbt_node* x;
	uint8_t y_original_color = y->color;
	if (z->left == t->nil) {
		x = z->right;
		rbt_transplant(t, z, z->right);
	}
	else if (z->right == t->nil) {
		x = z->left;
		rbt_transplant(t, z, z->left);
	}
	else {
		y = rbt_minimum(t, z->right);
		y_original_color = y->color;
		x = y->right;
		if (y->parent == z) x->parent = z;
		else {
			rbt_transplant(t, y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}
		rbt_transplant(t, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}
	if (y_original_color == RBT_BLACK) rbt_delete_fixup(t, x);
}

rbt_node* rbt_search_recurse(rbt* t, rbt_node* n, size_t index) {
	rbt_node* r = NULL;
	if (n != t->nil) {
		if (n->index == index) return n;
		r = rbt_search_recurse(t, n->left, index);
		if (!r) r = rbt_search_recurse(t, n->right, index);
	}
	return r;
}

extern inline rbt_node* rbt_search(rbt* t, size_t index) {
	return rbt_search_recurse(t, t->root, index);
}

void rbt_free_recurse(rbt* t, rbt_node* n) {
	if (n != t->nil) {
		rbt_free_recurse(t, n->left);
		rbt_free_recurse(t, n->right);
		free(n);
	}
}

extern inline void rbt_free(rbt* t) {
	rbt_free_recurse(t, t->root);
	free(t->nil);
	free(t);
}