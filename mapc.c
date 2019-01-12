/*
 *  Project: mapc - File: mapc.c
 *  Copyright (C) 2019 - Tania Hagn - tania@df9ry.de
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mapc/mapc.h"

#include <assert.h>

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

void mapc_init(struct mapc *map, f_compare compare)
{
	assert(map);
	assert(compare);
	map->compare = compare;
	map->root = NULL;
}

struct mapc *mapc_new(f_compare compare)
{
	struct mapc *map = malloc(sizeof(struct mapc));
	mapc_init(map, compare);
	return map;
}

void mapc_free(struct mapc *map)
{
	if (!map)
		return;
	assert(map->root == NULL);
}

struct mapc_node *mapc_lookup(struct mapc *map, const void *key)
{
	struct mapc_node *p;
	int c;

	assert(map);
	p = map->root;
	while (p) {
		c = map->compare(p->key, key);
		if (c < 0) {
			p = p->right;
		} else if (c > 0) {
			p = p->left;
		} else {
			return p;
		}
	} // end while //
	return p;
}

static bool ins(f_compare compare, struct mapc_node *x, struct mapc_node **p, bool *h)
{ /* h == false */
	struct mapc_node *p1, *p2;
	int c;
	bool result;

	if (!(*p)) {
		(*p) = x;
		(*h) = true;
		(*p)->left = NULL;
		(*p)->right = NULL;
		(*p)->bal = 0;
		return true;
	}
	c = compare(x->key, (*p)->key);
	if (c < 0) {
		result = ins(compare, x, &((*p)->left), h);
		if (*h) { /* left branch growth */
			switch ((*p)->bal) {
			case +1:
				(*p)->bal = 0;
				(*h) = false;
				break;
			case 0:
				(*p)->bal = -1;
				break;
			case -1: /* balance again */
				p1 = (*p)->left;
				if (p1->bal == -1) { /* single LL rotation */
					(*p)->left = (*p)->right; (*p)->right = (*p);
					(*p)->bal = 0; (*p) = p1;
				} else { /* double LR rotation */
					p2 = p1->right;
					(*p)->right = p2->left; p2->left = p1;
					(*p)->left = p2->right; p2->right = (*p);
					(*p)->bal = (p2->bal == -1)? +1 : 0;
					p1->bal = (p2->bal == +1)? -1 : 0;
					(*p) = p2;
				}
				(*p)->bal = 0; (*h) = false;
			} /* end switch */
		}
	} else if (c > 0) {
		result = ins(compare, x, &((*p)->right), h);
		if (*h) { /* right branch growth */
			switch ((*p)->bal) {
			case -1:
				(*p)->bal = 0;
				(*h) = false;
				break;
			case 0:
				(*p)->bal = +1;
				break;
			case +1: /* balance again */
				p1 = (*p)->right;
				if (p1->bal == +1) { /* single RR rotation */
					(*p)->right = p1->left; p1->left = (*p);
					(*p)->bal = 0; (*p) = p1;
				} else { /* double LR rotation */
					p2 = p1->left;
					p1->left = p2->right; p2->right = p1;
					(*p)->right = p2->left; p2->left = (*p);
					(*p)->bal = (p2->bal == +1)? -1 : 0;
					p1->bal = (p2->bal == -1)? +1 : 0;
					(*p) = p2;
				}
				(*p)->bal = 0;
				(*h) = false;
			} /* end switch */
		}
	} else {
		(*h) = false;
		result = false;
	}
	return result;
}

bool mapc_insert(struct mapc *map, struct mapc_node *node, const void *key)
{
	bool h;

	assert(map);
	assert(node);
	node->key = key;
	h = false;
	return ins(map->compare, node, &(map->root), &h);
}

static struct mapc_node *flatten(struct mapc_node *list, struct mapc_node *node)
{
	if (!node)
		return list;
	if (node->right)
		list = flatten(list, node->right);
	node->right = list;
	list = node;
	if (node->left)
		list = flatten(list, node->left);
	node->left = NULL;
	return list;
}

struct mapc_node *mapc_remove(struct mapc *map, const void *key)
{
	struct mapc_node *to_remove, *list, *next;

	assert(map);
	to_remove = mapc_lookup(map, key);
	if (!to_remove)
		return NULL;
	list = flatten(NULL, map->root);
	map->root = NULL;
	while (list != NULL) {
		next = list->right;
		if (list != to_remove)
			mapc_insert(map, list, list->key);
		list = next;
	} // end while //
	return to_remove;
}

void mapc_clear(struct mapc *map)
{
	assert(map);
	flatten(NULL, map->root);
	map->root = NULL;
}

static void __foreach(struct mapc_node *node, f_oper f, void *user_data, bool reverse)
{
	if (!node)
		return;
	if (reverse) {
		__foreach(node->right, f, user_data, true);
		f(node, user_data);
		__foreach(node->left, f, user_data, true);
	} else {
		__foreach(node->left, f, user_data, false);
		f(node, user_data);
		__foreach(node->right, f, user_data, false);
	}
}

void mapc_foreach(struct mapc *map, f_oper f, void *user_data)
{
	assert(map);
	assert(f);
	__foreach(map->root, f, user_data, false);
}

void mapc_foreach_reverse(struct mapc *map, f_oper f, void *user_data)
{
	assert(map);
	assert(f);
	__foreach(map->root, f, user_data, true);
}

static void __size(struct mapc_node *elem, void *user_data)
{
	(*((size_t *)user_data)) += 1;
}

size_t mapc_size(struct mapc *map)
{
	size_t size = 0;
	assert(map);
	mapc_foreach(map, __size, &size);
	return size;
}

size_t __height(struct mapc_node *node)
{
	if (node) {
		return max(__height(node->left), __height(node->right)) + 1;
	} else {
		return 0;
	}
}

size_t mapc_height(struct mapc *map)
{
	assert(map);
	return __height(map->root);
}
