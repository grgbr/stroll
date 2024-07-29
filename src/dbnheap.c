/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/dbnheap.h"

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_dbnheap_assert_intern(_expr) \
	stroll_assert("stroll:dbnheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_dbnheap_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static inline __stroll_nonull(1) __pure __stroll_nothrow
struct stroll_dbnheap_node *
stroll_dbnheap_from_dlist(const struct stroll_dlist_node * __restrict node)
{
	stroll_dbnheap_assert_intern(node);

	return stroll_dlist_entry(node, struct stroll_dbnheap_node, siblings);
}

struct stroll_dbnheap_node *
stroll_dbnheap_base_peek(const struct stroll_dbnheap_base * __restrict heap,
                         stroll_dbnheap_cmp_fn *                       compare,
                         void *                                        data)
{
	stroll_dbnheap_assert_api(heap);
	stroll_dbnheap_assert_api(!stroll_dlist_empty(&heap->roots));
	stroll_dbnheap_assert_api(compare);

	const struct stroll_dbnheap_node * node;
	const struct stroll_dbnheap_node * curr;

	node = stroll_dbnheap_from_dlist(stroll_dlist_next(&heap->roots));
	curr = node;

	stroll_dlist_continue_entry(&heap->roots, curr, siblings) {
		if (compare(curr, node, data) < 0)
			node = curr;
	}

STROLL_IGNORE_WARN("-Wcast-qual")
	return (struct stroll_dbnheap_node *)node;
STROLL_RESTORE_WARN
}

static __stroll_nonull(1, 2)
void
stroll_dbnheap_siftup(struct stroll_dbnheap_node * __restrict node,
                      struct stroll_dbnheap_node * __restrict parent)
{
	stroll_dbnheap_assert_intern(node);
	stroll_dbnheap_assert_intern(parent);
	stroll_dbnheap_assert_intern(node != parent);
	stroll_dbnheap_assert_intern(node->parent == parent);
	stroll_dbnheap_assert_intern(!stroll_dlist_empty(&node->siblings));
	stroll_dbnheap_assert_intern(!stroll_dlist_empty(&parent->siblings));
	stroll_dbnheap_assert_intern(!stroll_dlist_empty(&parent->children));

	unsigned int                 order = parent->order;
	struct stroll_dlist_node     siblings = parent->siblings;
	struct stroll_dbnheap_node * grand = parent->parent;
	struct stroll_dlist_node     children = STROLL_DLIST_INIT(children);
	struct stroll_dbnheap_node * child;

	stroll_dlist_splice_before(&children,
	                           stroll_dlist_next(&parent->children),
	                           stroll_dlist_prev(&parent->children));

	parent->order = node->order;
	stroll_dlist_inject(node->siblings.prev,
	                    &parent->siblings,
	                    node->siblings.next);
	stroll_dlist_foreach_entry(&node->children, child, siblings)
		child->parent = parent;
	if (!stroll_dlist_empty(&node->children))
		stroll_dlist_splice_before(&parent->children,
		                           stroll_dlist_next(&node->children),
		                           stroll_dlist_prev(&node->children));
	else
		stroll_dlist_init(&parent->children);

	node->order = order;
	stroll_dlist_inject(siblings.prev, &node->siblings, siblings.next);
	node->parent = grand;
	stroll_dlist_foreach_entry(&children, child, siblings)
		child->parent = node;
	stroll_dlist_embed_before(&node->children,
	                          stroll_dlist_next(&children),
	                          stroll_dlist_prev(&children));
}

static __stroll_nonull(1, 2, 3)
struct stroll_dbnheap_node *
stroll_dbnheap_join(struct stroll_dbnheap_node * __restrict first,
                    struct stroll_dbnheap_node * __restrict second,
                    stroll_dbnheap_cmp_fn *                 compare,
                    void *                                  data)
{
	stroll_dbnheap_assert_intern(first);
	stroll_dbnheap_assert_intern(second);
	stroll_dbnheap_assert_intern(first != second);
	stroll_dbnheap_assert_intern(first->order == second->order);
	stroll_dbnheap_assert_intern(compare);

	struct stroll_dbnheap_node * parent;
	struct stroll_dbnheap_node * child;

	if (compare(first, second, data) <= 0) {
		parent = first;
		child = second;
	}
	else {
		parent = second;
		child = first;
	}

	child->parent = parent;
	parent->order++;
	stroll_dlist_insert(&parent->children, &child->siblings);

	return parent;
}

static __stroll_nonull(1, 2, 3)
void
stroll_dbnheap_merge_single(struct stroll_dlist_node * __restrict  roots,
                            struct stroll_dbnheap_node * __restrict node,
                            stroll_dbnheap_cmp_fn *                 compare,
                            void *                                  data)
{
	stroll_dbnheap_assert_intern(roots);
	stroll_dbnheap_assert_intern(node);
	stroll_dbnheap_assert_intern(!node->parent);
	stroll_dbnheap_assert_intern(compare);

	while (!stroll_dlist_empty(roots)) {
		struct stroll_dbnheap_node * next;

		next = stroll_dbnheap_from_dlist(stroll_dlist_next(roots));
		stroll_dbnheap_assert_intern(!next->parent);

		stroll_dbnheap_assert_intern(node->order <= next->order);
		if (node->order != next->order)
			break;

		stroll_dlist_remove(&next->siblings);
		node = stroll_dbnheap_join(node, next, compare, data);
	}

	stroll_dlist_append(roots, &node->siblings);
}

void
stroll_dbnheap_base_insert(struct stroll_dbnheap_base * __restrict heap,
                           struct stroll_dbnheap_node * __restrict node,
                           stroll_dbnheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_dbnheap_assert_api(heap);
	stroll_dbnheap_assert_api(node);
	stroll_dbnheap_assert_api(compare);

	node->order = 0;
	node->parent = NULL;
	stroll_dlist_init(&node->children);

	stroll_dbnheap_merge_single(&heap->roots, node, compare, data);
}

static __stroll_nonull(1, 2, 3)
struct stroll_dbnheap_node *
stroll_dbnheap_merge_pair(struct stroll_dlist_node * __restrict  first,
                          struct stroll_dlist_node  * __restrict second,
                          stroll_dbnheap_cmp_fn *                compare,
                          void *                                 data)
{
	stroll_dbnheap_assert_intern(!stroll_dlist_empty(first));
	stroll_dbnheap_assert_intern(!stroll_dlist_empty(second));
	stroll_dbnheap_assert_intern(compare);

	struct stroll_dbnheap_node * fst;
	struct stroll_dbnheap_node * snd;

	fst = stroll_dbnheap_from_dlist(stroll_dlist_next(first));
	snd = stroll_dbnheap_from_dlist(stroll_dlist_next(second));

	if (fst->order == snd->order) {
		stroll_dlist_remove(&fst->siblings);
		stroll_dlist_remove(&snd->siblings);

		return stroll_dbnheap_join(fst, snd, compare, data);
	}
	else if (fst->order < snd->order) {
		stroll_dlist_remove(&fst->siblings);

		return fst;
	}
	else {
		stroll_dlist_remove(&snd->siblings);

		return snd;
	}
}

static __stroll_nonull(1, 2, 3)
void
stroll_dbnheap_merge_trees(struct stroll_dlist_node * __restrict  result,
                           struct stroll_dlist_node  * __restrict source,
                           stroll_dbnheap_cmp_fn *                compare,
                           void *                                 data)
{
	stroll_dbnheap_assert_intern(result);
	stroll_dbnheap_assert_intern(!stroll_dlist_empty(result));
	stroll_dbnheap_assert_intern(source);
	stroll_dbnheap_assert_intern(!stroll_dlist_empty(source));
	stroll_dbnheap_assert_intern(result != source);
	stroll_dbnheap_assert_intern(compare);

	struct stroll_dlist_node     res = STROLL_DLIST_INIT(res);
	struct stroll_dbnheap_node * last;
	struct stroll_dlist_node *   left;

	stroll_dlist_splice_before(&res,
	                           stroll_dlist_next(result),
	                           stroll_dlist_prev(result));

	last = stroll_dbnheap_merge_pair(&res, source, compare, data);
	while (!(stroll_dlist_empty(&res) || stroll_dlist_empty(source))) {
		struct stroll_dbnheap_node * curr;

		curr = stroll_dbnheap_merge_pair(&res, source, compare, data);

		stroll_dbnheap_assert_intern(last->order <= curr->order);
		if (last->order != curr->order) {
			stroll_dlist_nqueue_back(result, &last->siblings);
			last = curr;
		}
		else
			last = stroll_dbnheap_join(last, curr, compare, data);
	}

	left = (!stroll_dlist_empty(&res)) ? &res : source;
	stroll_dbnheap_merge_single(left, last, compare, data);
	stroll_dlist_embed_before(result,
	                          stroll_dlist_next(left),
	                          stroll_dlist_prev(left));
}

void
stroll_dbnheap_base_merge(struct stroll_dbnheap_base * __restrict result,
                          struct stroll_dbnheap_base * __restrict source,
                          stroll_dbnheap_cmp_fn *                 compare,
                          void *                                  data)
{
	stroll_dbnheap_assert_api(result);
	stroll_dbnheap_assert_api(source);
	stroll_dbnheap_assert_api(result != source);
	stroll_dbnheap_assert_api(compare);

	if (stroll_dlist_empty(&source->roots))
		return;

	if (stroll_dlist_empty(&result->roots)) {
		stroll_dlist_embed_before(&result->roots,
		                          stroll_dlist_next(&source->roots),
		                          stroll_dlist_prev(&source->roots));

		return;
	}

	stroll_dbnheap_merge_trees(&result->roots,
	                           &source->roots,
	                           compare,
	                           data);
}

static __stroll_nonull(1, 2, 3)
void
stroll_dbnheap_remove_root(struct stroll_dlist_node * __restrict    roots,
                           struct stroll_dbnheap_node  * __restrict root,
                           stroll_dbnheap_cmp_fn *                  compare,
                           void *                                   data)
{
	stroll_dbnheap_assert_intern(roots);
	stroll_dbnheap_assert_intern(!stroll_dlist_empty(roots));
	stroll_dbnheap_assert_intern(root);
	stroll_dbnheap_assert_intern(!root->parent);
	stroll_dbnheap_assert_intern(compare);

	struct stroll_dbnheap_node * node;

	stroll_dlist_remove(&root->siblings);

	if (stroll_dlist_empty(&root->children))
		return;

	stroll_dlist_foreach_entry(&root->children, node, siblings)
		node->parent = NULL;

	if (stroll_dlist_empty(roots)) {
		stroll_dlist_embed_before(roots,
		                          stroll_dlist_next(&root->children),
		                          stroll_dlist_prev(&root->children));
		return;
	}

	stroll_dbnheap_merge_trees(roots, &root->children, compare, data);
}

struct stroll_dbnheap_node *
stroll_dbnheap_base_extract(struct stroll_dbnheap_base * __restrict heap,
                            stroll_dbnheap_cmp_fn *                 compare,
                            void *                                  data)
{
	stroll_dbnheap_assert_api(heap);
	stroll_dbnheap_assert_api(!stroll_dlist_empty(&heap->roots));
	stroll_dbnheap_assert_api(compare);

	struct stroll_dbnheap_node * node;

	node = stroll_dbnheap_base_peek(heap, compare, data);

	stroll_dbnheap_remove_root(&heap->roots, node, compare, data);

	return node;
}

static __stroll_nonull(1, 2, 3)
void
stroll_dbnheap_remove_node(struct stroll_dlist_node * __restrict   roots,
                           struct stroll_dbnheap_node * __restrict node,
                           stroll_dbnheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_dbnheap_assert_intern(roots);
	stroll_dbnheap_assert_intern(!stroll_dlist_empty(roots));
	stroll_dbnheap_assert_intern(node);
	stroll_dbnheap_assert_intern(compare);

	while (node->parent)
		stroll_dbnheap_siftup(node, node->parent);

	stroll_dbnheap_remove_root(roots, node, compare, data);
}

void
stroll_dbnheap_base_remove(struct stroll_dbnheap_base * __restrict heap,
                           struct stroll_dbnheap_node * __restrict node,
                           stroll_dbnheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_dbnheap_assert_api(heap);
	stroll_dbnheap_assert_api(!stroll_dlist_empty(&heap->roots));
	stroll_dbnheap_assert_api(node);
	stroll_dbnheap_assert_api(compare);

	stroll_dbnheap_remove_node(&heap->roots, node, compare, data);
}

void
stroll_dbnheap_base_promote(struct stroll_dbnheap_base * __restrict heap __unused,
                            struct stroll_dbnheap_node * __restrict node,
                            stroll_dbnheap_cmp_fn *                 compare,
                            void *                                  data)
{
	stroll_dbnheap_assert_api(heap);
	stroll_dbnheap_assert_api(!stroll_dlist_empty(&heap->roots));
	stroll_dbnheap_assert_api(node);
	stroll_dbnheap_assert_api(compare);

	while (node->parent && (compare(node, node->parent, data) < 0))
		stroll_dbnheap_siftup(node, node->parent);
}

void
stroll_dbnheap_base_demote(struct stroll_dbnheap_base * __restrict heap,
                           struct stroll_dbnheap_node * __restrict node,
                           stroll_dbnheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_dbnheap_assert_api(heap);
	stroll_dbnheap_assert_api(!stroll_dlist_empty(&heap->roots));
	stroll_dbnheap_assert_api(node);
	stroll_dbnheap_assert_api(compare);

	stroll_dbnheap_remove_node(&heap->roots, node, compare, data);
	stroll_dbnheap_assert_intern(!node->parent);

	node->order = 0;
	stroll_dlist_init(&node->children);
	stroll_dbnheap_merge_single(&heap->roots, node, compare, data);
}

void
stroll_dbnheap_insert(struct stroll_dbnheap * __restrict      heap,
                      struct stroll_dbnheap_node * __restrict node,
                      void *                                  data)
{
	stroll_dbnheap_assert_heap_api(heap);
	stroll_dbnheap_assert_api(heap->cnt < heap->nr);

	stroll_dbnheap_base_insert(&heap->base, node, heap->compare, data);

	heap->cnt++;
}

void
stroll_dbnheap_merge(struct stroll_dbnheap * __restrict result,
                     struct stroll_dbnheap * __restrict source,
                     void *                             data)
{
	stroll_dbnheap_assert_heap_api(result);
	stroll_dbnheap_assert_heap_api(source);
	stroll_dbnheap_assert_api(result != source);
	stroll_dbnheap_assert_api(result->compare == source->compare);
	stroll_dbnheap_assert_api((result->cnt + source->cnt) <= result->nr);

	stroll_dbnheap_base_merge(&result->base,
	                          &source->base,
	                          result->compare,
	                          data);

	result->cnt += source->cnt;
}


struct stroll_dbnheap_node *
stroll_dbnheap_extract(struct stroll_dbnheap * __restrict heap, void * data)
{
	stroll_dbnheap_assert_heap_api(heap);
	stroll_dbnheap_assert_api(heap->cnt);

	struct stroll_dbnheap_node * node;

	node = stroll_dbnheap_base_extract(&heap->base, heap->compare, data);

	heap->cnt--;

	return node;
}

void
stroll_dbnheap_remove(struct stroll_dbnheap * __restrict      heap,
                      struct stroll_dbnheap_node * __restrict node,
                      void *                                  data)
{
	stroll_dbnheap_assert_heap_api(heap);
	stroll_dbnheap_assert_api(heap->cnt);

	stroll_dbnheap_base_remove(&heap->base, node, heap->compare, data);

	heap->cnt--;
}

void
stroll_dbnheap_setup(struct stroll_dbnheap * __restrict heap,
                     unsigned int                       nr,
                     stroll_dbnheap_cmp_fn *            compare)
{
	stroll_dbnheap_assert_api(heap);
	stroll_dbnheap_assert_api(nr);
	stroll_dbnheap_assert_api(compare);

	heap->cnt = 0;
	heap->nr = nr;
	stroll_dbnheap_base_setup(&heap->base);
	heap->compare = compare;
}

struct stroll_dbnheap *
stroll_dbnheap_create(unsigned int nr, stroll_dbnheap_cmp_fn * compare)
{
	stroll_dbnheap_assert_api(nr);
	stroll_dbnheap_assert_api(compare);

	struct stroll_dbnheap * heap;

	heap = malloc(sizeof(*heap));
	if (!heap)
		return NULL;

	stroll_dbnheap_setup(heap, nr, compare);

	return heap;
}
