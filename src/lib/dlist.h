/*
 * dlist.h
 *
 *  Created on: Jun 3, 2021
 *      Author: krad2
 */

#ifndef DLIST_H_
#define DLIST_H_

#include "attributes.h"
#include <string.h>

typedef struct dlist_node
{
	struct dlist_node *prev;
    struct dlist_node *next;
} dlist_node_t;

static inline void dlist_add_after(dlist_node_t *node, dlist_node_t *new_node)
{
    new_node->next = node->next;
    node->next = new_node;

	new_node->prev = node->prev;
	node->prev = new_node;
}

static inline void dlist_add_before(dlist_node_t *node, dlist_node_t *new_node)
{
	dlist_node_t *before_node = node->prev;
	dlist_add_after(before_node, new_node);
}

static inline void dlist_remove(dlist_node_t *node)
{
	dlist_node_t *prev = node->prev;
	dlist_node_t *next = node->next;

	if (prev != NULL)
	{
		prev->next = next;
	}
	
	if (next != NULL)
	{
		next->prev = prev;
	}

	memset(node, 0, sizeof(*node));
}

#endif
