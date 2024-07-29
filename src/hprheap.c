/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/hprheap.h"

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_hprheap_assert_intern(_expr) \
	stroll_assert("stroll:hprheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_hprheap_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_hprheap_attach(struct stroll_hprheap_node * __restrict node,
                      struct stroll_hprheap_node * __restrict parent)
{
	node->children[1] = parent->children[0];
	parent->children[0] = node;
}

static __stroll_nonull(1, 2, 3)
struct stroll_hprheap_node *
stroll_hprheap_join(struct stroll_hprheap_node * __restrict first,
                    struct stroll_hprheap_node * __restrict second,
                    stroll_hprheap_cmp_fn  *                compare,
                    void *                                  data)
{
	stroll_hprheap_assert_intern(first);
	stroll_hprheap_assert_intern(second);
	stroll_hprheap_assert_intern(first != second);
	stroll_hprheap_assert_intern(compare);

	struct stroll_hprheap_node * parent;
	struct stroll_hprheap_node * child;

	if (compare(first, second, data) <= 0) {
		parent = first;
		child = second;
	}
	else {
		parent = second;
		child = first;
	}

	stroll_hprheap_attach(child, parent);

	return parent;
}

void
stroll_hprheap_base_insert(struct stroll_hprheap_base * __restrict heap,
                           struct stroll_hprheap_node * __restrict node,
                           stroll_hprheap_cmp_fn   *               compare,
                           void *                                  data)
{
	stroll_hprheap_assert_api(heap);
	stroll_hprheap_assert_api(node);
	stroll_hprheap_assert_api(node != heap->root);
	stroll_hprheap_assert_api(compare);

	node->children[0] = NULL;

	if (heap->root)
		heap->root = stroll_hprheap_join(heap->root,
		                                 node,
		                                 compare,
		                                 data);
	else
		heap->root = node;
}

void
stroll_hprheap_base_merge(struct stroll_hprheap_base * __restrict       result,
                          const struct stroll_hprheap_base * __restrict source,
                          stroll_hprheap_cmp_fn *                       compare,
                          void *                                        data)
{
	stroll_hprheap_assert_api(result);
	stroll_hprheap_assert_api(source);
	stroll_hprheap_assert_api(result != source);
	stroll_hprheap_assert_api(compare);

	if (!source->root)
		return;

	if (result->root)
		result->root = stroll_hprheap_join(result->root,
		                                   source->root,
		                                   compare,
		                                   data);
	else
		result->root = source->root;
}

static __stroll_nonull(2)
struct stroll_hprheap_node *
stroll_hprheap_merge_nodes(struct stroll_hprheap_node * __restrict nodes,
                           stroll_hprheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_hprheap_assert_intern(compare);

	struct stroll_hprheap_node * root = NULL;

	if (nodes) {
		struct stroll_hprheap_node * next;
		struct stroll_hprheap_node * twin = NULL;

		/*
		 * Merging pass 1: merge pairs of sub-trees from left to right.
		 */
		do {
			next = nodes->children[1];

			if (twin) {
				twin = stroll_hprheap_join(twin,
				                           nodes,
				                           compare,
				                           data);
				twin->children[1] = root;
				root = twin;
				twin = NULL;
			}
			else
				twin = nodes;

			nodes = next;
		} while (nodes);

		if (twin) {
			twin->children[1] = root;
			root = twin;
		}

		stroll_hprheap_assert_intern(root);

		/*
		 * Merging pass 2: merge remaining sub-trees into a single one
		 * from right to left.
		 */
		twin = root->children[1];
		while (twin) {
			next = twin->children[1];
			root = stroll_hprheap_join(root, twin, compare, data);
			twin = next;
		}
	}

	return root;

}

static __stroll_nonull(1, 2)
struct stroll_hprheap_node *
stroll_hprheap_remove_root(struct stroll_hprheap_node * __restrict root,
                           stroll_hprheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_hprheap_assert_intern(root);
	stroll_hprheap_assert_intern(compare);

	return stroll_hprheap_merge_nodes(root->children[0], compare, data);
}

struct stroll_hprheap_node *
stroll_hprheap_base_extract(struct stroll_hprheap_base * __restrict heap,
                            stroll_hprheap_cmp_fn *                 compare,
                            void *                                  data)
{
	stroll_hprheap_assert_api(heap);
	stroll_hprheap_assert_api(heap->root);
	stroll_hprheap_assert_api(compare);

	struct stroll_hprheap_node * node = heap->root;

	heap->root = stroll_hprheap_remove_root(node, compare, data);

	return node;
}

void
stroll_hprheap_insert(struct stroll_hprheap * __restrict      heap,
                      struct stroll_hprheap_node * __restrict node,
                      void *                                  data)
{
	stroll_hprheap_assert_heap_api(heap);
	stroll_hprheap_assert_api(heap->cnt < heap->nr);

	stroll_hprheap_base_insert(&heap->base, node, heap->compare, data);

	heap->cnt++;
}

void
stroll_hprheap_merge(struct stroll_hprheap * __restrict       result,
                     const struct stroll_hprheap * __restrict source,
                     void *                                   data)
{
	stroll_hprheap_assert_heap_api(result);
	stroll_hprheap_assert_heap_api(source);
	stroll_hprheap_assert_api(result != source);
	stroll_hprheap_assert_api(result->compare == source->compare);
	stroll_hprheap_assert_api((result->cnt + source->cnt) <= result->nr);

	stroll_hprheap_base_merge(&result->base,
	                          &source->base,
	                          result->compare,
	                          data);

	result->cnt += source->cnt;
}

struct stroll_hprheap_node *
stroll_hprheap_extract(struct stroll_hprheap * __restrict heap,
                       void *                             data)
{
	stroll_hprheap_assert_heap_api(heap);
	stroll_hprheap_assert_api(heap->cnt);

	struct stroll_hprheap_node * node;

	node = stroll_hprheap_base_extract(&heap->base, heap->compare, data);

	heap->cnt--;

	return node;
}

void
stroll_hprheap_setup(struct stroll_hprheap * __restrict heap,
                     unsigned int                       nr,
                     stroll_hprheap_cmp_fn *            compare)
{
	stroll_hprheap_assert_api(heap);
	stroll_hprheap_assert_api(nr);
	stroll_hprheap_assert_api(compare);

	heap->cnt = 0;
	heap->nr = nr;
	stroll_hprheap_base_setup(&heap->base);
	heap->compare = compare;
}

struct stroll_hprheap *
stroll_hprheap_create(unsigned int nr, stroll_hprheap_cmp_fn * compare)
{
	stroll_hprheap_assert_api(nr);
	stroll_hprheap_assert_api(compare);

	struct stroll_hprheap * heap;

	heap = malloc(sizeof(*heap));
	if (!heap)
		return NULL;

	stroll_hprheap_setup(heap, nr, compare);

	return heap;
}
