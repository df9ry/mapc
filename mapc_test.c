/*
 *  Project: mapc - File: mapc_test.c
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

#include <uki/kernel.h>
#include <stringc/stringc.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static struct mapc map;

static int compare(void *o1, void *o2)
{
	int i1 = *(int *)o1, i2 = *(int *)o2;

	if (i1 < i2)
		return -1;
	else if (i1 > i2)
		return +1;
	else
		return 0;
}

struct int_node {
	struct mapc_node node;
	int key;
	int val;
};

static void on_node(struct mapc_node *elem, void *user_data)
{
	struct int_node *in = container_of(elem, struct int_node, node);
	struct string *s = (struct string *) user_data;
	char buf[32];

	snprintf(buf, 32, "(%i/%i)", in->key, in->val);
	string_add_c(s, buf);
}

static struct int_node one   = { .key = 1, .val = 11 };
static struct int_node five  = { .key = 5, .val = 15 };
static struct int_node seven = { .key = 7, .val = 17 };
static struct int_node zero  = { .key = 0, .val = 10 };
static struct int_node nine  = { .key = 9, .val = 19 };
static struct int_node eight = { .key = 8, .val = 18 };
static struct int_node three = { .key = 3, .val = 13 };
static struct int_node six   = { .key = 6, .val = 16 };
static struct int_node two   = { .key = 2, .val = 12 };
static struct int_node four  = { .key = 4, .val = 14 };

int main(int argc, char *argv[]) {

	{
		printf("Testing static mapc allocation and free\n");
		MAPC_INIT(map, compare);
		assert(map.compare == compare);
		assert(map.root == NULL);
		printf ("OK\n");
	}

	{
		struct mapc *_map;

		printf("Testing dynamic mapc allocation and free\n");
		_map = mapc_new(compare);
		assert(_map->compare == compare);
		assert(_map->root == NULL);
		mapc_free(_map);
		printf ("OK\n");
	}

	{
		struct string s;
		STRING_INIT(s);
		int key;

		printf("Testing mapc_insert and mapc_remove\n");
		MAPC_INIT(map, compare);

		assert(MAPC_INSERT(map, &six.node,   &six.key  ));
		assert(MAPC_INSERT(map, &one.node,   &one.key  ));
		assert(MAPC_INSERT(map, &eight.node, &eight.key));
		assert(MAPC_INSERT(map, &four.node,  &four.key ));
		assert(MAPC_INSERT(map, &nine.node,  &nine.key ));
		assert(MAPC_INSERT(map, &three.node, &three.key));
		assert(MAPC_INSERT(map, &zero.node,  &zero.key ));
		assert(MAPC_INSERT(map, &two.node,   &two.key  ));
		assert(MAPC_INSERT(map, &five.node,  &five.key ));
		assert(MAPC_INSERT(map, &seven.node, &seven.key));
		assert(!MAPC_INSERT(map, &zero.node,  &zero.key ));
		assert(MAPC_SIZE(map) == 10);
		assert(MAPC_HEIGHT(map) == 4);
		MAPC_FOREACH(map, on_node, &s);
		assert(strcmp(STRING_C(s),
			"(0/10)(1/11)(2/12)(3/13)(4/14)(5/15)(6/16)(7/17)(8/18)(9/19)") == 0);
		STRING_RESET(s);
		MAPC_FOREACH_REVERSE(map, on_node, &s);
		assert(strcmp(STRING_C(s),
			"(9/19)(8/18)(7/17)(6/16)(5/15)(4/14)(3/13)(2/12)(1/11)(0/10)") == 0);
		STRING_RESET(s);

		key = 8;
		assert(MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == &eight.node);
		assert(!MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == NULL);
		MAPC_FOREACH(map, on_node, &s);
		assert(strcmp(STRING_C(s),
			"(0/10)(1/11)(2/12)(3/13)(4/14)(5/15)(6/16)(7/17)(9/19)") == 0);
		STRING_RESET(s);

		key = 7;
		assert(MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == &seven.node);
		assert(!MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == NULL);
		MAPC_FOREACH(map, on_node, &s);
		assert(strcmp(STRING_C(s),
			"(0/10)(1/11)(2/12)(3/13)(4/14)(5/15)(6/16)(9/19)") == 0);
		STRING_RESET(s);

		key = 3;
		assert(MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == &three.node);
		assert(!MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == NULL);
		MAPC_FOREACH(map, on_node, &s);
		assert(strcmp(STRING_C(s),
			"(0/10)(1/11)(2/12)(4/14)(5/15)(6/16)(9/19)") == 0);
		STRING_RESET(s);

		assert(MAPC_REMOVE(map, &key) == NULL);

		key = 6;
		assert(MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == &six.node);
		assert(!MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == NULL);
		MAPC_FOREACH(map, on_node, &s);
		assert(strcmp(STRING_C(s),
			"(0/10)(1/11)(2/12)(4/14)(5/15)(9/19)") == 0);
		STRING_RESET(s);

		key = 0;
		assert(MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == &zero.node);
		assert(!MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == NULL);
		MAPC_FOREACH(map, on_node, &s);
		assert(strcmp(STRING_C(s),
			"(1/11)(2/12)(4/14)(5/15)(9/19)") == 0);
		STRING_RESET(s);

		key = 9;
		assert(MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == &nine.node);
		assert(!MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == NULL);
		MAPC_FOREACH(map, on_node, &s);
		assert(strcmp(STRING_C(s),
			"(1/11)(2/12)(4/14)(5/15)") == 0);
		STRING_RESET(s);

		key = 1;
		assert(MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == &one.node);
		assert(!MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == NULL);
		MAPC_FOREACH(map, on_node, &s);
		assert(strcmp(STRING_C(s),
			"(2/12)(4/14)(5/15)") == 0);
		STRING_RESET(s);

		key = 4;
		assert(MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == &four.node);
		assert(!MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == NULL);
		MAPC_FOREACH(map, on_node, &s);
		assert(strcmp(STRING_C(s),
			"(2/12)(5/15)") == 0);
		STRING_RESET(s);

		key = 5;
		assert(MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == &five.node);
		assert(!MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == NULL);
		MAPC_FOREACH(map, on_node, &s);
		assert(strcmp(STRING_C(s),
			"(2/12)") == 0);
		STRING_RESET(s);

		key = 2;
		assert(MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == &two.node);
		assert(!MAPC_CONTAINS(map, &key));
		assert(MAPC_REMOVE(map, &key) == NULL);
		MAPC_FOREACH(map, on_node, &s);
		assert(strcmp(STRING_C(s),
			"") == 0);
		STRING_RESET(s);

		printf ("OK\n");
	}

	{
		struct string s;
		STRING_INIT(s);

		printf("Testing mapc_clear\n");
		MAPC_INIT(map, compare);

		assert(MAPC_INSERT(map, &six.node,   &six.key  ));
		assert(MAPC_INSERT(map, &one.node,   &one.key  ));
		assert(MAPC_INSERT(map, &eight.node, &eight.key));
		assert(MAPC_INSERT(map, &four.node,  &four.key ));
		assert(MAPC_INSERT(map, &nine.node,  &nine.key ));
		assert(MAPC_INSERT(map, &three.node, &three.key));
		assert(MAPC_INSERT(map, &zero.node,  &zero.key ));
		assert(MAPC_INSERT(map, &two.node,   &two.key  ));
		assert(MAPC_INSERT(map, &five.node,  &five.key ));
		assert(MAPC_INSERT(map, &seven.node, &seven.key));
		MAPC_CLEAR(map);
		assert(MAPC_SIZE(map) == 0);
		STRING_RESET(s);

		printf ("OK\n");
	}

	return EXIT_SUCCESS;
}
