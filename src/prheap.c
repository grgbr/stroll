/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/prheap.h"

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_prheap_assert_intern(_expr) \
	stroll_assert("stroll:prheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_prheap_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static
struct stroll_lcrs_node *
stroll_prheap_join(struct stroll_lcrs_node * __restrict first,
                   struct stroll_lcrs_node * __restrict second,
                   stroll_lcrs_cmp_fn  *                compare,
                   void *                               data)
{
	stroll_prheap_assert_intern(first);
	stroll_prheap_assert_intern(second);
	stroll_prheap_assert_intern(compare);

	if (compare(first, second, data) <= 0) {
		stroll_lcrs_join(second, first);

		return first;
	}

	stroll_lcrs_join(first, second);

	return second;
}

static
struct stroll_lcrs_node *
stroll_prheap_merge_roots(struct stroll_lcrs_node * __restrict roots,
                          stroll_lcrs_cmp_fn *                 compare,
                          void *                               data)
{
	stroll_prheap_assert_api(roots);
	stroll_prheap_assert_api(!stroll_lcrs_istail(roots));
	stroll_prheap_assert_api(compare);

	struct stroll_lcrs_node * curr = roots;
	struct stroll_lcrs_node * head = stroll_lcrs_mktail(NULL);
	struct stroll_lcrs_node * tmp;

	/* First pass pairing: move left to right merging pairs of trees. */
	do {
		struct stroll_lcrs_node *nxt;

		nxt = curr->sibling;
		if (stroll_lcrs_istail(nxt)) {
			curr->sibling = head;
			head = curr;
			break;
		}

		tmp = nxt->sibling;

		curr = stroll_prheap_join(curr, nxt, compare, data);

		curr->sibling = head;
		head = curr;

		curr = tmp;
	} while (!stroll_lcrs_istail(curr));

	/*
	 * Second pass pairing: move right to left and merges the rightmost
	 * subtree with the remaining subtrees, one tree at a time.
	 */
	curr = head;
	head = head->sibling;
	while (!stroll_lcrs_istail(head)) {
		tmp = head->sibling;

		curr = stroll_prheap_join(curr, head, compare, data);

		head = tmp;
	}

	return curr;
}

static
struct stroll_lcrs_node *
stroll_prheap_remove_key(struct stroll_lcrs_node * __restrict root,
                         struct stroll_lcrs_node * __restrict key,
                         bool                                 isroot,
                         stroll_lcrs_cmp_fn *                 compare,
                         void *                               data)
{
	stroll_prheap_assert_intern(root);
	stroll_prheap_assert_intern(key);
	stroll_prheap_assert_intern(compare);

	if (!stroll_lcrs_has_child(key)) {
		if (!isroot) {
			stroll_lcrs_split(
				key,
				stroll_lcrs_youngest_ref(stroll_lcrs_parent(key)));

			return root;
		}

		return NULL;
	}

	if (!isroot) {
		stroll_lcrs_split(
			key,
			stroll_lcrs_youngest_ref(stroll_lcrs_parent(key)));

		key = stroll_prheap_merge_roots(stroll_lcrs_youngest(key),
		                                compare,
		                                data);

		return stroll_prheap_join(root,
		                          key,
		                          compare,
		                          data);
	}

	return stroll_prheap_merge_roots(stroll_lcrs_youngest(key),
	                                 compare,
	                                 data);
}

static void
stroll_prheap_update_key(struct stroll_prheap * __restrict    heap,
                         struct stroll_lcrs_node * __restrict key,
                         bool                                 isroot,
                         stroll_lcrs_cmp_fn *                 compare,
                         void *                               data)
{
	struct stroll_lcrs_node * node;

	node = stroll_prheap_remove_key(heap->root, key, isroot, compare, data);

	stroll_lcrs_init(key);
	heap->root = stroll_prheap_join(node, key, compare, data);
}

void
stroll_prheap_merge(struct stroll_prheap * __restrict result,
                    struct stroll_prheap * __restrict source,
                    stroll_lcrs_cmp_fn *              compare,
                    void *                            data)
{
	stroll_prheap_assert_api(result);
	stroll_prheap_assert_api(result->root);
	stroll_prheap_assert_api(source);
	stroll_prheap_assert_api(source->root);
	stroll_prheap_assert_api(compare);

	result->root = stroll_prheap_join(result->root,
	                                  source->root,
	                                  compare,
	                                  data);
}

void
stroll_prheap_insert(struct stroll_prheap * __restrict    heap,
                     struct stroll_lcrs_node * __restrict key,
                     stroll_lcrs_cmp_fn *                 compare,
                     void *                               data)
{
	stroll_prheap_assert_api(heap);
	stroll_prheap_assert_api(key);
	stroll_prheap_assert_api(compare);

	stroll_lcrs_init(key);

	if (!heap->root) {
		heap->root = key;

		return;
	}

	heap->root = stroll_prheap_join(heap->root, key, compare, data);
}

struct stroll_lcrs_node *
stroll_prheap_extract(struct stroll_prheap * __restrict heap,
                      stroll_lcrs_cmp_fn *              compare,
                      void *                            data)
{
	stroll_prheap_assert_api(heap);
	stroll_prheap_assert_api(heap->root);
	stroll_prheap_assert_api(compare);

	struct stroll_lcrs_node * root = heap->root;

	if (!stroll_lcrs_has_child(root)) {
		heap->root = NULL;

		return root;
	}

	heap->root = stroll_prheap_merge_roots(stroll_lcrs_youngest(root),
	                                       compare,
	                                       data);

	return root;
}

void
stroll_prheap_remove(struct stroll_prheap * __restrict    heap,
                     struct stroll_lcrs_node * __restrict key,
                     stroll_lcrs_cmp_fn *                 compare,
                     void *                               data)
{
	stroll_prheap_assert_api(heap);
	stroll_prheap_assert_api(heap->root);
	stroll_prheap_assert_api(key);
	stroll_prheap_assert_api(compare);

	heap->root = stroll_prheap_remove_key(heap->root,
	                                      key,
	                                      key == heap->root,
	                                      compare,
	                                      data);
}

void
stroll_prheap_promote(struct stroll_prheap * __restrict    heap,
                      struct stroll_lcrs_node * __restrict key,
                      stroll_lcrs_cmp_fn *                 compare,
                      void *                               data)
{
	stroll_prheap_assert_api(heap);
	stroll_prheap_assert_api(heap->root);
	stroll_prheap_assert_api(key);
	stroll_prheap_assert_api(compare);

	bool isroot = (key == heap->root);

	if (isroot)
		return;

	if (compare(stroll_lcrs_parent(key), key, data) <= 0)
		return;

	stroll_prheap_update_key(heap, key, isroot, compare, data);
}

void
stroll_prheap_demote(struct stroll_prheap * __restrict    heap,
                     struct stroll_lcrs_node * __restrict key,
                     stroll_lcrs_cmp_fn *                 compare,
                     void *                               data)
{
	stroll_prheap_assert_api(heap);
	stroll_prheap_assert_api(heap->root);
	stroll_prheap_assert_api(key);
	stroll_prheap_assert_api(compare);

#warning REMOVE ME!
#if 0
	if (heap->count == 1)
		return;
#endif
	stroll_prheap_update_key(heap, key, key == heap->root, compare, data);
}
