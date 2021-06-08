#ifndef CLIST_H_
#define CLIST_H_

#include <stdbool.h>
#include <stddef.h>
#include "dlist.h"

typedef dlist_node_t clist_node_t;

// TODO: list tail and list head are adjacent here so don't need the vars for this

#define CLIST_HEAD(list) ((list)->head)
#define CLIST_TAIL(list) ((list)->head->prev)

typedef struct
{
	clist_node_t *head;

	size_t		count;
} clist_t;

static inline bool clist_is_empty(const clist_t *list)
{
    return list->count == 0;
}

static inline bool clist_exactly_one(const clist_t *list)
{
    return !clist_is_empty(list) && (CLIST_HEAD(list) == CLIST_TAIL(list));
}

static inline bool clist_more_than_one(const clist_t *list)
{
    return !clist_is_empty(list) && (CLIST_HEAD(list) != CLIST_TAIL(list));
}

static inline void clist_rpush(clist_t *list, clist_node_t *new_node)
{
	/* 0 elements - need to establish an initial node */
	if (clist_is_empty(list))
	{
		CLIST_HEAD(list) = new_node;
		CLIST_TAIL(list) = new_node;

		list->count = 1;
	}

	/* append to tail normally once we have a node */
	else if (clist_exactly_one(list) || clist_more_than_one(list))
	{
		dlist_add_after(CLIST_TAIL(list), new_node);
		CLIST_TAIL(list) = new_node;

		list->count++;
	}

}

static inline void clist_lpush(clist_t *list, clist_node_t *new_node)
{
	/* 0 elements - need to establish an initial node */
	if (clist_is_empty(list))
	{
		CLIST_HEAD(list) = new_node;
		CLIST_HEAD(list)->next = new_node;
		CLIST_HEAD(list)->prev = new_node;

		CLIST_TAIL(list) = new_node;
        CLIST_TAIL(list)->next = new_node;
        CLIST_TAIL(list)->prev = new_node;

		list->count = 1;
	}

	/* append to tail normally once we have a node */
	else if (clist_exactly_one(list) || clist_more_than_one(list))
	{
		dlist_add_before(CLIST_HEAD(list), new_node);
		CLIST_HEAD(list) = new_node;

		list->count++;
	}
}

static inline clist_node_t *clist_lpop(clist_t *list)
{
	/* 0 is empty */
	if (clist_is_empty(list))
	{
		return NULL;
	}

	/* 1 WILL become empty */
	else if (clist_exactly_one(list))
	{
		clist_node_t *head = CLIST_HEAD(list);
		dlist_remove(head);

		memset(list, 0, sizeof(*list));
		return head;
	}

	/* Otherwise: head slides forward */
	else 
	{
		clist_node_t *head = CLIST_HEAD(list);
		clist_node_t *new_head = head->next;

		dlist_remove(head);
		CLIST_HEAD(list) = new_head;

		return head;
	}
}

static inline void clist_lpoprpush(clist_t *list)
{
    if (CLIST_HEAD(list) != NULL) 
	{
        CLIST_HEAD(list) = CLIST_HEAD(list)->next;
    }
}

static inline clist_node_t *clist_lpeek(const clist_t *list)
{
	return CLIST_HEAD(list);
}

static inline clist_node_t *clist_rpeek(const clist_t *list)
{
    return CLIST_TAIL(list);
}

static inline clist_node_t *clist_rpop(clist_t *list)
{
	/* 0 is empty */
	if (clist_is_empty(list))
	{
		return NULL;
	}

	/* 1 WILL become empty */
	else if (clist_exactly_one(list))
	{
		clist_node_t *tail = CLIST_TAIL(list);
		dlist_remove(tail);

		memset(list, 0, sizeof(*list));
		return tail;
	}

	/* Otherwise: tail slides backward */
	else 
	{
		clist_node_t *tail = CLIST_TAIL(list);
		clist_node_t *new_tail = tail->prev;

		dlist_remove(tail);
		CLIST_TAIL(list) = new_tail;

		return tail;
	}
}

static inline clist_node_t *clist_find(const clist_t *list,
                                       const clist_node_t *node)
{

	if (clist_is_empty(list))
	{
		return NULL;
	}

	else if (clist_exactly_one(list))
	{
		if (node == CLIST_HEAD(list))
		{
			return CLIST_HEAD(list);
		}

		else 
		{
			return NULL;
		}
	}

	else 
	{
		clist_node_t *pos = CLIST_HEAD(list);
		while (pos != CLIST_TAIL(list))
		{
			if (pos == node)
			{
				return pos;
			}

			pos = pos->next;
		}

	}

    return NULL;
}

static inline clist_node_t *clist_find_before(const clist_t *list,
                                              const clist_node_t *node)
{
	return clist_find(list, node)->prev;
}

static inline clist_node_t *clist_remove_at(clist_t *list, clist_node_t *node)
{
	if (node == CLIST_HEAD(list))
	{
		return clist_lpop(list);
	}

	else if (node == CLIST_TAIL(list))
	{
		return clist_rpop(list);
	}

	else 
	{
		dlist_remove(node);
		list->count--;
		
		return node;
	}
}

static inline clist_node_t *clist_remove(clist_t *list, clist_node_t *node)
{
	clist_node_t *to_remove = clist_find(list, node);
	if (to_remove == NULL)
	{
		return NULL;
	}

	else 
	{
		return clist_remove_at(list, to_remove);
	}
}

static inline clist_node_t *clist_foreach(clist_t *list, int (*func)(const clist_node_t *,  void *), void *arg)
{
    clist_node_t *node = CLIST_TAIL(list);

    if (node != NULL) 
	{
        do
        {
            if (func(node, arg))
            {
                return node;
            }

            node = node->next;
        } while (node != CLIST_TAIL(list));
	}

    return NULL;
}

static inline size_t clist_count(const clist_t *list)
{
    return list->count;
}



#endif
