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
stroll_prheap_update_key(struct stroll_lcrs_node ** __restrict root,
                         struct stroll_lcrs_node * __restrict  key,
                         bool                                  isroot,
                         stroll_lcrs_cmp_fn *                  compare,
                         void *                                data)
{
	struct stroll_lcrs_node * node;

	node = stroll_prheap_remove_key(*root, key, isroot, compare, data);

	stroll_lcrs_init(key);
	*root = stroll_prheap_join(node, key, compare, data);
}

void
_stroll_prheap_insert(struct stroll_lcrs_node ** __restrict root,
                      struct stroll_lcrs_node * __restrict  node,
                      stroll_lcrs_cmp_fn *                  compare,
                      void *                                data)
{
	stroll_prheap_assert_api(root);
	stroll_prheap_assert_api(node);
	stroll_prheap_assert_api(node != *root);
	stroll_prheap_assert_api(compare);

	stroll_lcrs_init(node);

	if (!*root) {
		*root = node;

		return;
	}

	*root = stroll_prheap_join(*root, node, compare, data);
}

void
_stroll_prheap_merge(struct stroll_lcrs_node ** __restrict        result,
                     struct stroll_lcrs_node * const * __restrict source,
                     stroll_lcrs_cmp_fn *                         compare,
                     void *                                       data)
{
	stroll_prheap_assert_api(result);
	stroll_prheap_assert_api(source);
	stroll_prheap_assert_api(compare);

	if (!*source)
		return;

	if (!*result) {
		*result = *source;

		return;
	}

	*result = stroll_prheap_join(*result, *source, compare, data);
}

struct stroll_lcrs_node *
_stroll_prheap_extract(struct stroll_lcrs_node ** __restrict root,
                       stroll_lcrs_cmp_fn *                  compare,
                       void *                                data)
{
	stroll_prheap_assert_api(root);
	stroll_prheap_assert_api(*root);
	stroll_prheap_assert_api(compare);

	struct stroll_lcrs_node * node = *root;

	if (!stroll_lcrs_has_child(node)) {
		*root = NULL;

		return node;
	}

	*root = stroll_prheap_merge_roots(stroll_lcrs_youngest(node),
	                                  compare,
	                                  data);

	return node;
}

void
_stroll_prheap_remove(struct stroll_lcrs_node ** __restrict root,
                     struct stroll_lcrs_node * __restrict   node,
                     stroll_lcrs_cmp_fn *                   compare,
                     void *                                 data)
{
	stroll_prheap_assert_api(root);
	stroll_prheap_assert_api(*root);
	stroll_prheap_assert_api(node);
	stroll_prheap_assert_api(compare);

	*root = stroll_prheap_remove_key(*root,
	                                 node,
	                                 node == *root,
	                                 compare,
	                                 data);
}

void
_stroll_prheap_promote(struct stroll_lcrs_node ** __restrict root,
                       struct stroll_lcrs_node * __restrict  node,
                       stroll_lcrs_cmp_fn *                  compare,
                       void *                                data)
{
	stroll_prheap_assert_api(root);
	stroll_prheap_assert_api(*root);
	stroll_prheap_assert_api(node);
	stroll_prheap_assert_api(compare);

	if (node == *root)
		return;

	if (compare(stroll_lcrs_parent(node), node, data) <= 0)
		return;

	stroll_prheap_update_key(root, node, false, compare, data);
}

void
_stroll_prheap_demote(struct stroll_lcrs_node ** __restrict root,
                      struct stroll_lcrs_node * __restrict  node,
                      stroll_lcrs_cmp_fn *                  compare,
                      void *                                data)
{
	stroll_prheap_assert_api(root);
	stroll_prheap_assert_api(*root);
	stroll_prheap_assert_api(node);
	stroll_prheap_assert_api(compare);

	if (stroll_lcrs_has_child(*root))
		stroll_prheap_update_key(root,
		                         node,
		                         node == *root,
		                         compare,
		                         data);
}

void
stroll_prheap_insert(struct stroll_prheap * __restrict    heap,
                     struct stroll_lcrs_node * __restrict node,
                     stroll_lcrs_cmp_fn *                 compare,
                     void *                               data)
{
	stroll_prheap_assert_heap_api(heap);
	stroll_prheap_assert_api(heap->cnt < heap->nr);

	_stroll_prheap_insert(&heap->root, node, compare, data);

	heap->cnt++;
}

void
stroll_prheap_merge(struct stroll_prheap * __restrict       result,
                    const struct stroll_prheap * __restrict source,
                    stroll_lcrs_cmp_fn *                    compare,
                    void *                                  data)
{
	stroll_prheap_assert_heap_api(result);
	stroll_prheap_assert_heap_api(source);
	stroll_prheap_assert_api(result != source);
	stroll_prheap_assert_api((result->cnt + source->cnt) <= result->nr);

	_stroll_prheap_merge(&result->root, &source->root, compare, data);

	result->cnt += source->cnt;
}

struct stroll_lcrs_node *
stroll_prheap_extract(struct stroll_prheap * __restrict heap,
                      stroll_lcrs_cmp_fn *              compare,
                      void *                            data)
{
	stroll_prheap_assert_heap_api(heap);

	struct stroll_lcrs_node * node;

	node = _stroll_prheap_extract(&heap->root, compare, data);

	heap->cnt--;

	return node;
}

void
stroll_prheap_remove(struct stroll_prheap * __restrict    heap,
                     struct stroll_lcrs_node * __restrict node,
                     stroll_lcrs_cmp_fn *                 compare,
                     void *                               data)
{
	stroll_prheap_assert_heap_api(heap);
	stroll_prheap_assert_api(heap->cnt);

	_stroll_prheap_remove(&heap->root, node, compare, data);

	heap->cnt--;
}

void
stroll_prheap_setup(struct stroll_prheap * __restrict heap,
                    unsigned int                      nr,
                    stroll_lcrs_cmp_fn *              compare)
{
	stroll_prheap_assert_api(heap);
	stroll_prheap_assert_api(nr);
	stroll_prheap_assert_api(compare);

	heap->cnt = 0;
	heap->nr = nr;
	heap->root = NULL;
	heap->compare = compare;
}

struct stroll_prheap *
stroll_prheap_create(unsigned int nr, stroll_lcrs_cmp_fn * compare)
{
	stroll_prheap_assert_api(nr);
	stroll_prheap_assert_api(compare);

	struct stroll_prheap * heap;

	heap = malloc(sizeof(*heap));
	if (!heap)
		return NULL;

	stroll_prheap_setup(heap, nr, compare);

	return heap;
}
