/*
 *  Project: mapc - File: mapc.h
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

#ifndef MAPC_MAPC_H_
#define MAPC_MAPC_H_

/** @file */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
 * @brief Node of a map.
 * To allow your structs to be stored in a mapc add a struct mapc_node as a
 * member to your struct.
 */
struct mapc_node {
	void             *key;    /**< Node key.                               */
	struct mapc_node *left;   /**< Nodes that are "less" than this one.    */
	struct mapc_node *right;  /**< Nodes that are "greater" than this one. */
	int               bal;    /**< Helper value for balancing the tree.    */
};

/**
 * @brief Compare function for the map.
 * @param o1 Operand 1 for the compare.
 * @param o2 Operand 2 for the compare.
 * @return Value less than 0, when o1 < o2, 0 when they are equal and
 *         value greater than 0, when o1 > o2.
 */
typedef int (*f_compare)(void *o1, void *o2);

/**
 * @brief Root level struct of a mapc.
 */
struct mapc {
	f_compare         compare; /**< Pointer to the compare function. */
	struct mapc_node *root;    /**< Pointer to the root node.        */
};

/**
 * @brief Initialize a mapc.
 * @param map Pointer to struct mapc to initialize.
 * @param compare Compare function for this map.
 */
extern void mapc_init(struct mapc *map, f_compare compare);

/**
 * @brief Initialize a mapc.
 * @param map Struct mapc to initialize.
 * @param compare Compare function for this map.
 */
#define MAPC_INIT(map, compare) mapc_init(&map, compare)

/**
 * @brief Allocate and initialize a new mapc.
 * @param compare Compare function for the new map.
 * @return New map.
 */
extern struct mapc *mapc_new(f_compare compare);

/**
 * @brief Free a dynamically allocated map.
 * @param map Pointer to the map to free.
 * @note Content of the map is not destroyed by this call.
 */
extern void mapc_free(struct mapc *map);

/**
 * @brief Function for mapc_foreach and mapc_foreach_reverse.
 * @param elem Current element.
 * @param user_data Transparent pointer passed to each invocation of f_oper.
 */
typedef void (*f_oper)(struct mapc_node *elem, void *user_data);

/**
 * @brief Call function f for each element in the map in lexical order.
 * @param map Pointer to the map.
 * @param f Function to call for each member.
 * @param user_data Pointer to pass to each invocation of f.
 */
extern void mapc_foreach(struct mapc *map, f_oper f, void *user_data);

/**
 * @brief Call function f for each element in the map in lexical order.
 * @param map Map.
 * @param f Function to call for each member.
 * @param user_data Pointer to pass to each invocation of f.
 */
#define MAPC_FOREACH(map, f, user_data) mapc_foreach(&map, f, user_data);

/**
 * @brief Call function f for each element in the map in reverse order.
 * @param map Pointer to the map.
 * @param f Function to call for each member.
 * @param user_data Pointer to pass to each invocation of f.
 */
extern void mapc_foreach_reverse(struct mapc *map, f_oper f, void *user_data);

/**
 * @brief Call function f for each element in the map in reverse order.
 * @param map Map.
 * @param f Function to call for each member.
 * @param user_data Pointer to pass to each invocation of f.
 */
#define MAPC_FOREACH_REVERSE(map, f, user_data) mapc_foreach_reverse(&map, f, user_data);

/**
 * @brief Insert a node into a map.
 * @param map Pointer to map to insert into.
 * @param node Pointer to node to insert.
 * @param key Key for insertion of the node.
 * @return True, if the node could be inserted into the map, false otherwise.
 */
extern bool mapc_insert(struct mapc *map, struct mapc_node *node, void *key);

/**
 * @brief Insert a node into a map.
 * @param map Map to insert into.
 * @param node Node to insert.
 * @param key Key for insertion of the node.
 * @return True, if the node could be inserted into the map, false otherwise.
 */
#define MAPC_INSERT(map, node, key) mapc_insert(&map, node, key)

/**
 * @brief Remove a node from a map.
 * @param map Pointer to map to remove the node.
 * @param key Key to look up for.
 * @return Pointer of the removed node or NULL if not found.
 */
extern struct mapc_node *mapc_remove(struct mapc *map, void *key);

/**
 * @brief Remove a node from a map.
 * @param map Pointer to map to remove the node.
 * @param key Key to look up for.
 * @return Pointer of the removed node or NULL if not found.
 */
#define MAPC_REMOVE(map, key) mapc_remove(&map, key)

/**
 * @brief Clear the map.
 * @param map Pointer to the map.
 */
extern void mapc_clear(struct mapc *map);

/**
 * @brief Clear the map.
 * @param map Pointer to the map.
 */
#define MAPC_CLEAR(map) mapc_clear(&map)

/**
 * @brief Lookup map for a key.
 * @param map Pointer to the map to lookup.
 * @param key Key to look up for.
 * @return Pointer to mapc_node or NULL, when not found.
 */
extern struct mapc_node *mapc_lookup(struct mapc *map, void *key);

/**
 * @brief Lookup map for a key.
 * @param map Map to lookup.
 * @param key Key to look up for.
 * @return Pointer to mapc_node or NULL, when not found.
 */
#define MAPC_LOOKUP(map, key) mapc_lookup(&map, key)

/**
 * @brief Lookup map for a key.
 * @param map Pointer to the map to lookup.
 * @param key Key to look up for.
 * @return True if the map contains this key.
 */
static inline bool mapc_contains(struct mapc *map, void *key)
{
	return (mapc_lookup(map, key));
}

/**
 * @brief Lookup map for a key.
 * @param map The map to lookup.
 * @param key Key to look up for.
 * @return True if the map contains this key.
 */
#define MAPC_CONTAINS(map, key) mapc_contains(&map, key)

/**
 * @brief Get number of elements in the tree.
 * @param map Pointer to the map.
 * @return Number of elements in the map.
 */
extern size_t mapc_size(struct mapc *map);

/**
 * @brief Get number of elements in the tree.
 * @param map The map.
 * @return Number of elements in the map.
 */
#define MAPC_SIZE(map) mapc_size(&map)

/**
 * @brief Get height of the tree.
 * @param map Pointer to the map.
 * @return Height of the tree.
 */
extern size_t mapc_height(struct mapc *map);

/**
 * @brief Get height of the tree.
 * @param map The map.
 * @return Height of the tree.
 */
#define MAPC_HEIGHT(map) mapc_height(&map)

#endif /* MAPC_MAPC_H_ */
