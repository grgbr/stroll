/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/pprheap.h"

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_pprheap_assert_intern(_expr) \
	stroll_assert("stroll:pprheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_pprheap_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_pprheap_assert_root(_root) \
	({ \
		stroll_pprheap_assert_api(_root); \
		stroll_pprheap_assert_api(!(_root)->parent); \
	 })

static struct stroll_pprheap_node stroll_pprheap_plug;

#if defined(CONFIG_STROLL_UTEST) || defined(CONFIG_STROLL_PTEST)

const struct stroll_pprheap_node * const stroll_pprheap_tail =
	&stroll_pprheap_plug;

#endif /* defined(CONFIG_STROLL_UTEST) || defined(CONFIG_STROLL_PTEST) */

static __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_pprheap_attach_child(struct stroll_pprheap_node * __restrict node,
                            struct stroll_pprheap_node * __restrict parent)
{
	struct stroll_pprheap_node * child = parent->children[0];

	node->children[1] = child;
	node->parent = parent;
	parent->children[0] = node;
	child->parent = node;
}

static __stroll_nonull(1) __stroll_nothrow
void
stroll_pprheap_detach_node(const struct stroll_pprheap_node * __restrict node)
{
	stroll_pprheap_assert_intern(node);
	stroll_pprheap_assert_intern(node->parent);
	stroll_pprheap_assert_intern((node == node->parent->children[0]) ^
	                             (node == node->parent->children[1]));

	struct stroll_pprheap_node * parent = node->parent;
	int                          spine = node == parent->children[1];
	struct stroll_pprheap_node * sibling = node->children[1];

	parent->children[spine] = sibling;
	sibling->parent = parent;
}

static __stroll_nonull(1, 2, 3)
struct stroll_pprheap_node *
stroll_pprheap_join(struct stroll_pprheap_node * __restrict first,
                    struct stroll_pprheap_node * __restrict second,
                    stroll_pprheap_cmp_fn  *                compare,
                    void *                                  data)
{
	stroll_pprheap_assert_intern(first);
	stroll_pprheap_assert_intern(second);
	stroll_pprheap_assert_intern(first != second);
	stroll_pprheap_assert_intern(compare);

	struct stroll_pprheap_node * parent;
	struct stroll_pprheap_node * child;

	if (compare(first, second, data) <= 0) {
		parent = first;
		child = second;
	}
	else {
		parent = second;
		child = first;
	}

	stroll_pprheap_attach_child(child, parent);

	return parent;
}

void
stroll_pprheap_base_insert(struct stroll_pprheap_base * __restrict heap,
                           struct stroll_pprheap_node * __restrict node,
                           stroll_pprheap_cmp_fn   *               compare,
                           void *                                  data)
{
	stroll_pprheap_assert_api(heap);
	stroll_pprheap_assert_api(node);
	stroll_pprheap_assert_api(node != heap->root);
	stroll_pprheap_assert_api(compare);

	node->parent = NULL;
	node->children[0] = &stroll_pprheap_plug;

	if (heap->root) {
		stroll_pprheap_assert_root(heap->root);

		heap->root = stroll_pprheap_join(heap->root,
		                                 node,
		                                 compare,
		                                 data);
	}
	else
		heap->root = node;
}

void
stroll_pprheap_base_merge(struct stroll_pprheap_base * __restrict       result,
                          const struct stroll_pprheap_base * __restrict source,
                          stroll_pprheap_cmp_fn *                       compare,
                          void *                                        data)
{
	stroll_pprheap_assert_api(result);
	stroll_pprheap_assert_api(source);
	stroll_pprheap_assert_api(result != source);
	stroll_pprheap_assert_api(compare);

	if (!source->root)
		return;

	if (result->root)
		result->root = stroll_pprheap_join(result->root,
		                                   source->root,
		                                   compare,
		                                   data);
	else
		result->root = source->root;
}

static __stroll_nonull(2) __stroll_nothrow
struct stroll_pprheap_node *
stroll_pprheap_merge_nodes(struct stroll_pprheap_node * __restrict nodes,
                           stroll_pprheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_pprheap_assert_intern(compare);

	if (nodes != &stroll_pprheap_plug) {
		stroll_pprheap_assert_intern(nodes ==
		                             nodes->parent->children[0]);

		struct stroll_pprheap_node * root = &stroll_pprheap_plug;
		struct stroll_pprheap_node * next;
		struct stroll_pprheap_node * twin = NULL;

		/*
		 * Merging pass 1: merge pairs of sub-trees from left to right.
		 */
		do {
			next = nodes->children[1];

			if (twin) {
				twin = stroll_pprheap_join(twin,
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
		} while (nodes != &stroll_pprheap_plug);

		if (twin) {
			twin->children[1] = root;
			root = twin;
		}

		stroll_pprheap_assert_intern(root != &stroll_pprheap_plug);

		/*
		 * Merging pass 2: merge remaining sub-trees into a single one
		 * from right to left.
		 */
		twin = root->children[1];
		while (twin != &stroll_pprheap_plug) {
			next = twin->children[1];
			root = stroll_pprheap_join(root, twin, compare, data);
			twin = next;
		}

		return root;
	}

	return NULL;
}

static __stroll_nonull(1, 2)
struct stroll_pprheap_node *
stroll_pprheap_remove_root(struct stroll_pprheap_node * __restrict root,
                           stroll_pprheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_pprheap_assert_intern(root);
	stroll_pprheap_assert_intern(compare);

	struct stroll_pprheap_node * nevv;

	nevv = stroll_pprheap_merge_nodes(root->children[0], compare, data);
	if (nevv) {
		nevv->children[1] = &stroll_pprheap_plug;
		nevv->parent = NULL;
	}

	return nevv;
}

static __stroll_nonull(1, 2, 3)
void
stroll_pprheap_remove_node(struct stroll_pprheap_node * __restrict root,
                           struct stroll_pprheap_node * __restrict node,
                           stroll_pprheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_pprheap_assert_intern(root);
	stroll_pprheap_assert_intern(node);
	stroll_pprheap_assert_intern(node->parent);
	stroll_pprheap_assert_intern(compare);

	struct stroll_pprheap_node * nevv;

	stroll_pprheap_detach_node(node);

	nevv = stroll_pprheap_merge_nodes(node->children[0], compare, data);
	if (nevv)
		stroll_pprheap_attach_child(nevv, root);
}

struct stroll_pprheap_node *
stroll_pprheap_base_extract(struct stroll_pprheap_base * __restrict heap,
                            stroll_pprheap_cmp_fn *                 compare,
                            void *                                  data)
{
	stroll_pprheap_assert_api(heap);
	stroll_pprheap_assert_root(heap->root);
	stroll_pprheap_assert_api(compare);

	struct stroll_pprheap_node * node = heap->root;

	heap->root = stroll_pprheap_remove_root(node, compare, data);

	return node;
}

void
stroll_pprheap_base_remove(struct stroll_pprheap_base * __restrict heap,
                           struct stroll_pprheap_node * __restrict node,
                           stroll_pprheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_pprheap_assert_api(heap);
	stroll_pprheap_assert_root(heap->root);
	stroll_pprheap_assert_api(node);
	stroll_pprheap_assert_api(compare);

	if (!node->parent) {
		stroll_pprheap_assert_intern(node == heap->root);

		heap->root = stroll_pprheap_remove_root(node, compare, data);
	}
	else {
		stroll_pprheap_assert_intern(node != heap->root);

		stroll_pprheap_remove_node(heap->root, node, compare, data);
	}
}

static __stroll_nonull(1, 2, 3)
void
stroll_pprheap_update_node(struct stroll_pprheap_base * __restrict heap,
                           struct stroll_pprheap_node * __restrict node,
                           stroll_pprheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_pprheap_assert_intern(heap);
	stroll_pprheap_assert_intern(heap->root);
	stroll_pprheap_assert_intern(node);
	stroll_pprheap_assert_intern(node != heap->root);
	stroll_pprheap_assert_intern(node->parent);
	stroll_pprheap_assert_intern(compare);

	stroll_pprheap_remove_node(heap->root, node, compare, data);
	node->parent = NULL;
	node->children[0] = &stroll_pprheap_plug;

	heap->root = stroll_pprheap_join(heap->root, node, compare, data);
}

void
stroll_pprheap_base_promote(struct stroll_pprheap_base * __restrict heap,
                            struct stroll_pprheap_node * __restrict node,
                            stroll_pprheap_cmp_fn *                 compare,
                            void *                                  data)
{
	stroll_pprheap_assert_api(heap);
	stroll_pprheap_assert_root(heap->root);
	stroll_pprheap_assert_api(node);
	stroll_pprheap_assert_api(compare);

	if (!node->parent) {
		stroll_pprheap_assert_intern(node == heap->root);

		return;
	}

	stroll_pprheap_assert_intern(node->parent);
	if (compare(node->parent, node, data) <= 0)
		return;

	stroll_pprheap_update_node(heap, node, compare, data);
}

void
stroll_pprheap_base_demote(struct stroll_pprheap_base * __restrict heap,
                           struct stroll_pprheap_node * __restrict node,
                           stroll_pprheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_pprheap_assert_api(heap);
	stroll_pprheap_assert_root(heap->root);
	stroll_pprheap_assert_api(node);
	stroll_pprheap_assert_api(compare);

	if (!node->parent) {
		stroll_pprheap_assert_intern(node == heap->root);

		struct stroll_pprheap_node * nevv;

		nevv = stroll_pprheap_remove_root(node, compare, data);
		if (nevv) {
			node->children[0] = &stroll_pprheap_plug;
			heap->root = stroll_pprheap_join(nevv,
			                                 node,
			                                 compare,
			                                 data);
		}

		return;
	}

	stroll_pprheap_update_node(heap, node, compare, data);
}

void
stroll_pprheap_insert(struct stroll_pprheap * __restrict      heap,
                      struct stroll_pprheap_node * __restrict node,
                      void *                                  data)
{
	stroll_pprheap_assert_heap_api(heap);
	stroll_pprheap_assert_api(heap->cnt < heap->nr);

	stroll_pprheap_base_insert(&heap->base, node, heap->compare, data);

	heap->cnt++;
}

void
stroll_pprheap_merge(struct stroll_pprheap * __restrict       result,
                     const struct stroll_pprheap * __restrict source,
                     void *                                   data)
{
	stroll_pprheap_assert_heap_api(result);
	stroll_pprheap_assert_heap_api(source);
	stroll_pprheap_assert_api(result != source);
	stroll_pprheap_assert_api(result->compare == source->compare);
	stroll_pprheap_assert_api((result->cnt + source->cnt) <= result->nr);

	stroll_pprheap_base_merge(&result->base,
	                          &source->base,
	                          result->compare,
	                          data);

	result->cnt += source->cnt;
}

struct stroll_pprheap_node *
stroll_pprheap_extract(struct stroll_pprheap * __restrict heap,
                       void *                             data)
{
	stroll_pprheap_assert_heap_api(heap);
	stroll_pprheap_assert_api(heap->cnt);

	struct stroll_pprheap_node * node;

	node = stroll_pprheap_base_extract(&heap->base, heap->compare, data);

	heap->cnt--;

	return node;
}

void
stroll_pprheap_remove(struct stroll_pprheap * __restrict      heap,
                      struct stroll_pprheap_node * __restrict node,
                      void *                                  data)
{
	stroll_pprheap_assert_heap_api(heap);
	stroll_pprheap_assert_api(heap->cnt);

	stroll_pprheap_base_remove(&heap->base, node, heap->compare, data);

	heap->cnt--;
}

void
stroll_pprheap_setup(struct stroll_pprheap * __restrict heap,
                     unsigned int                       nr,
                     stroll_pprheap_cmp_fn *            compare)
{
	stroll_pprheap_assert_api(heap);
	stroll_pprheap_assert_api(nr);
	stroll_pprheap_assert_api(compare);

	heap->cnt = 0;
	heap->nr = nr;
	stroll_pprheap_base_setup(&heap->base);
	heap->compare = compare;
}

struct stroll_pprheap *
stroll_pprheap_create(unsigned int nr, stroll_pprheap_cmp_fn * compare)
{
	stroll_pprheap_assert_api(nr);
	stroll_pprheap_assert_api(compare);

	struct stroll_pprheap * heap;

	heap = malloc(sizeof(*heap));
	if (!heap)
		return NULL;

	stroll_pprheap_setup(heap, nr, compare);

	return heap;
}
