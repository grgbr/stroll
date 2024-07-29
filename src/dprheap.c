/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/dprheap.h"

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_dprheap_assert_intern(_expr) \
	stroll_assert("stroll:dprheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_dprheap_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_dprheap_assert_root(_root) \
	({ \
		stroll_dprheap_assert_api(_root); \
		stroll_dprheap_assert_api(!(_root)->parent); \
	 })

static __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_dprheap_attach_child(struct stroll_dprheap_node * __restrict child,
                            struct stroll_dprheap_node * __restrict parent)
{
	child->parent = parent;
	stroll_dlist_insert(&parent->children, &child->siblings);
}

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_dprheap_detach_child(const struct stroll_dprheap_node * __restrict child)
{
	stroll_dlist_remove(&child->siblings);
}

static __stroll_nonull(1, 2, 3)
struct stroll_dprheap_node *
stroll_dprheap_join(struct stroll_dprheap_node * __restrict first,
                    struct stroll_dprheap_node * __restrict second,
                    stroll_dprheap_cmp_fn  *                compare,
                    void *                                  data)
{
	stroll_dprheap_assert_intern(first);
	stroll_dprheap_assert_intern(second);
	stroll_dprheap_assert_intern(first != second);
	stroll_dprheap_assert_intern(compare);

	struct stroll_dprheap_node * parent;
	struct stroll_dprheap_node * child;

	if (compare(first, second, data) <= 0) {
		parent = first;
		child = second;
	}
	else {
		parent = second;
		child = first;
	}

	stroll_dprheap_attach_child(child, parent);

	return parent;
}

void
stroll_dprheap_base_insert(struct stroll_dprheap_base * __restrict heap,
                           struct stroll_dprheap_node * __restrict node,
                           stroll_dprheap_cmp_fn   *               compare,
                           void *                                  data)
{
	stroll_dprheap_assert_api(heap);
	stroll_dprheap_assert_api(node);
	stroll_dprheap_assert_api(node != heap->root);
	stroll_dprheap_assert_api(compare);

	node->parent = NULL;
	stroll_dlist_init(&node->children);

	if (heap->root) {
		stroll_dprheap_assert_root(heap->root);

		heap->root = stroll_dprheap_join(heap->root,
		                                 node,
		                                 compare,
		                                 data);
	}
	else
		heap->root = node;
}

void
stroll_dprheap_base_merge(struct stroll_dprheap_base * __restrict       result,
                          const struct stroll_dprheap_base * __restrict source,
                          stroll_dprheap_cmp_fn *                       compare,
                          void *                                        data)
{
	stroll_dprheap_assert_api(result);
	stroll_dprheap_assert_api(source);
	stroll_dprheap_assert_api(result != source);
	stroll_dprheap_assert_api(compare);

	if (!source->root)
		return;

	if (result->root)
		result->root = stroll_dprheap_join(result->root,
		                                   source->root,
		                                   compare,
		                                   data);
	else
		result->root = source->root;
}

static __stroll_nonull(1, 2) __stroll_nothrow
struct stroll_dprheap_node *
stroll_dprheap_merge_nodes(struct stroll_dlist_node * __restrict nodes,
                           stroll_dprheap_cmp_fn *               compare,
                           void *                                data)
{
	stroll_dprheap_assert_intern(nodes);
	stroll_dprheap_assert_intern(compare);

	struct stroll_dprheap_node * root = NULL;

	if (!stroll_dlist_empty(nodes)) {
		struct stroll_dlist_node     head = STROLL_DLIST_INIT(head);
		struct stroll_dprheap_node * curr;
		struct stroll_dprheap_node * tmp;

		/*
		 * Merging pass 1: merge pairs of sub-trees from left to right.
		 */
		stroll_dlist_foreach_entry_safe(nodes, curr, siblings, tmp) {
			if (root) {
				root = stroll_dprheap_join(root,
				                           curr,
				                           compare,
				                           data);
				stroll_dlist_append(&head, &root->siblings);
				root = NULL;
			}
			else
				root = curr;
		}

		if (root)
			stroll_dlist_append(&head, &root->siblings);

		stroll_dprheap_assert_intern(!stroll_dlist_empty(&head));

		/*
		 * Merging pass 2: merge remaining sub-trees into a single one
		 * from right to left.
		 */
		root = stroll_dlist_entry(stroll_dlist_next(&head),
		                          struct stroll_dprheap_node,
		                          siblings);
		curr = root;
		stroll_dlist_continue_entry_safe(&head, curr, siblings, tmp)
			root = stroll_dprheap_join(root, curr, compare, data);
	}

	return root;
}

static __stroll_nonull(1, 2)
struct stroll_dprheap_node *
stroll_dprheap_remove_root(struct stroll_dprheap_node * __restrict root,
                           stroll_dprheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_dprheap_assert_intern(root);
	stroll_dprheap_assert_intern(compare);

	struct stroll_dprheap_node * nevv;

	nevv = stroll_dprheap_merge_nodes(&root->children, compare, data);
	if (nevv)
		nevv->parent = NULL;

	return nevv;
}

static __stroll_nonull(1, 2, 3)
void
stroll_dprheap_remove_node(struct stroll_dprheap_node * __restrict root,
                           struct stroll_dprheap_node * __restrict node,
                           stroll_dprheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_dprheap_assert_intern(root);
	stroll_dprheap_assert_intern(node);
	stroll_dprheap_assert_intern(!stroll_dlist_empty(&node->siblings));
	stroll_dprheap_assert_intern(node->parent);
	stroll_dprheap_assert_intern(compare);

	struct stroll_dprheap_node * nevv;

	stroll_dprheap_detach_child(node);

	nevv = stroll_dprheap_merge_nodes(&node->children, compare, data);
	if (nevv)
		stroll_dprheap_attach_child(nevv, root);
}

struct stroll_dprheap_node *
stroll_dprheap_base_extract(struct stroll_dprheap_base * __restrict heap,
                            stroll_dprheap_cmp_fn *                 compare,
                            void *                                  data)
{
	stroll_dprheap_assert_api(heap);
	stroll_dprheap_assert_root(heap->root);
	stroll_dprheap_assert_api(compare);

	struct stroll_dprheap_node * node = heap->root;

	heap->root = stroll_dprheap_remove_root(node, compare, data);

	return node;
}

void
stroll_dprheap_base_remove(struct stroll_dprheap_base * __restrict heap,
                           struct stroll_dprheap_node * __restrict node,
                           stroll_dprheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_dprheap_assert_api(heap);
	stroll_dprheap_assert_root(heap->root);
	stroll_dprheap_assert_api(node);
	stroll_dprheap_assert_api(compare);

	if (!node->parent) {
		stroll_dprheap_assert_intern(node == heap->root);

		heap->root = stroll_dprheap_remove_root(node, compare, data);
	}
	else {
		stroll_dprheap_assert_intern(node != heap->root);

		stroll_dprheap_remove_node(heap->root, node, compare, data);
	}
}

static __stroll_nonull(1, 2, 3)
void
stroll_dprheap_update_node(struct stroll_dprheap_base * __restrict heap,
                           struct stroll_dprheap_node * __restrict node,
                           stroll_dprheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_dprheap_assert_intern(heap);
	stroll_dprheap_assert_intern(heap->root);
	stroll_dprheap_assert_intern(node);
	stroll_dprheap_assert_intern(node != heap->root);
	stroll_dprheap_assert_intern(!stroll_dlist_empty(&node->siblings));
	stroll_dprheap_assert_intern(node->parent);
	stroll_dprheap_assert_intern(compare);

	stroll_dprheap_remove_node(heap->root, node, compare, data);
	node->parent = NULL;
	stroll_dlist_init(&node->children);

	heap->root = stroll_dprheap_join(heap->root, node, compare, data);
}

void
stroll_dprheap_base_promote(struct stroll_dprheap_base * __restrict heap,
                            struct stroll_dprheap_node * __restrict node,
                            stroll_dprheap_cmp_fn *                 compare,
                            void *                                  data)
{
	stroll_dprheap_assert_api(heap);
	stroll_dprheap_assert_root(heap->root);
	stroll_dprheap_assert_api(node);
	stroll_dprheap_assert_api(compare);

	if (!node->parent) {
		stroll_dprheap_assert_intern(node == heap->root);

		return;
	}

	stroll_dprheap_assert_intern(node->parent);
	if (compare(node->parent, node, data) <= 0)
		return;

	stroll_dprheap_update_node(heap, node, compare, data);
}

void
stroll_dprheap_base_demote(struct stroll_dprheap_base * __restrict heap,
                           struct stroll_dprheap_node * __restrict node,
                           stroll_dprheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_dprheap_assert_api(heap);
	stroll_dprheap_assert_root(heap->root);
	stroll_dprheap_assert_api(node);
	stroll_dprheap_assert_api(compare);

	if (!node->parent) {
		stroll_dprheap_assert_intern(node == heap->root);

		struct stroll_dprheap_node * nevv;

		nevv = stroll_dprheap_remove_root(node, compare, data);
		if (nevv) {
			stroll_dlist_init(&node->children);
			heap->root = stroll_dprheap_join(nevv,
			                                 node,
			                                 compare,
			                                 data);
		}

		return;
	}

	stroll_dprheap_update_node(heap, node, compare, data);
}

void
stroll_dprheap_insert(struct stroll_dprheap * __restrict      heap,
                      struct stroll_dprheap_node * __restrict node,
                      void *                                  data)
{
	stroll_dprheap_assert_heap_api(heap);
	stroll_dprheap_assert_api(heap->cnt < heap->nr);

	stroll_dprheap_base_insert(&heap->base, node, heap->compare, data);

	heap->cnt++;
}

void
stroll_dprheap_merge(struct stroll_dprheap * __restrict       result,
                     const struct stroll_dprheap * __restrict source,
                     void *                                   data)
{
	stroll_dprheap_assert_heap_api(result);
	stroll_dprheap_assert_heap_api(source);
	stroll_dprheap_assert_api(result != source);
	stroll_dprheap_assert_api(result->compare == source->compare);
	stroll_dprheap_assert_api((result->cnt + source->cnt) <= result->nr);

	stroll_dprheap_base_merge(&result->base,
	                          &source->base,
	                          result->compare,
	                          data);

	result->cnt += source->cnt;
}

struct stroll_dprheap_node *
stroll_dprheap_extract(struct stroll_dprheap * __restrict heap,
                       void *                             data)
{
	stroll_dprheap_assert_heap_api(heap);
	stroll_dprheap_assert_api(heap->cnt);

	struct stroll_dprheap_node * node;

	node = stroll_dprheap_base_extract(&heap->base, heap->compare, data);

	heap->cnt--;

	return node;
}

void
stroll_dprheap_remove(struct stroll_dprheap * __restrict      heap,
                      struct stroll_dprheap_node * __restrict node,
                      void *                                  data)
{
	stroll_dprheap_assert_heap_api(heap);
	stroll_dprheap_assert_api(heap->cnt);

	stroll_dprheap_base_remove(&heap->base, node, heap->compare, data);

	heap->cnt--;
}

void
stroll_dprheap_setup(struct stroll_dprheap * __restrict heap,
                     unsigned int                       nr,
                     stroll_dprheap_cmp_fn *            compare)
{
	stroll_dprheap_assert_api(heap);
	stroll_dprheap_assert_api(nr);
	stroll_dprheap_assert_api(compare);

	heap->cnt = 0;
	heap->nr = nr;
	stroll_dprheap_base_setup(&heap->base);
	heap->compare = compare;
}

struct stroll_dprheap *
stroll_dprheap_create(unsigned int nr, stroll_dprheap_cmp_fn * compare)
{
	stroll_dprheap_assert_api(nr);
	stroll_dprheap_assert_api(compare);

	struct stroll_dprheap * heap;

	heap = malloc(sizeof(*heap));
	if (!heap)
		return NULL;

	stroll_dprheap_setup(heap, nr, compare);

	return heap;
}
