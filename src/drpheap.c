/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/drpheap.h"
#include "stroll/bmap.h"

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_drpheap_assert_intern(_expr) \
	stroll_assert("stroll:drpheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_drpheap_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static struct stroll_drpheap_node stroll_drpheap_plug = {
	.rank = -1
};

#if defined(CONFIG_STROLL_UTEST) || defined(CONFIG_STROLL_PTEST)

const struct stroll_drpheap_link * const stroll_drpheap_tail =
	&stroll_drpheap_plug.link;

#endif /* defined(CONFIG_STROLL_UTEST) || defined(CONFIG_STROLL_PTEST) */

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_drpheap_isroot_link(const struct stroll_drpheap_link * __restrict link)
{
	stroll_drpheap_assert_intern(link);

	return !!((unsigned long)link & 1UL);
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_drpheap_link *
stroll_drpheap_decode_root_link(
	const struct stroll_drpheap_link * __restrict link)
{
	stroll_drpheap_assert_intern(link);

	return (struct stroll_drpheap_link *)((unsigned long)link & ~(1UL));
}

static __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_drpheap_inject_root_link(struct stroll_drpheap_link *            prev,
                                struct stroll_drpheap_link * __restrict link,
                                struct stroll_drpheap_link *            next)
{
	stroll_drpheap_assert_intern(prev);
	stroll_drpheap_assert_intern(!stroll_drpheap_isroot_link(prev));
	stroll_drpheap_assert_intern(link);
	stroll_drpheap_assert_intern(!stroll_drpheap_isroot_link(link));
	stroll_drpheap_assert_intern(next);
	stroll_drpheap_assert_intern(!stroll_drpheap_isroot_link(next));
	stroll_drpheap_assert_intern(link != prev);
	stroll_drpheap_assert_intern(link != next);

	prev->edges[0] = link;

	link->edges[0] = next;
	link->edges[1] = stroll_drpheap_encode_root_link(prev);

	next->edges[1] = stroll_drpheap_encode_root_link(link);
}

static __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_drpheap_append_root_link(struct stroll_drpheap_link * __restrict at,
                                struct stroll_drpheap_link * __restrict link)
{
	stroll_drpheap_inject_root_link(at, link, at->edges[0]);
}

static __stroll_nonull(1) __stroll_nothrow
void
stroll_drpheap_remove_root_link(struct stroll_drpheap_link * __restrict link)
{
	stroll_drpheap_assert_intern(link);
	stroll_drpheap_assert_intern(!stroll_drpheap_isroot_link(link));

	struct stroll_drpheap_link * next = link->edges[0];
	struct stroll_drpheap_link * prev = link->edges[1];

	next->edges[1] = prev;
	stroll_drpheap_decode_root_link(prev)->edges[0] = next;
}

static __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_drpheap_attach_child(struct stroll_drpheap_node * __restrict node,
                            struct stroll_drpheap_node * __restrict parent)
{
	stroll_drpheap_assert_intern(node);
	stroll_drpheap_assert_intern(parent);

	struct stroll_drpheap_link * child = parent->child;

	/* Make former parent's first sibling a sibling of node. */
	node->link.edges[0] = child;
	/* Set node's parent. */
	node->link.edges[1] = &parent->link;
	/* Update parent's rank. */
	parent->rank = node->rank + 1;
	/* Make node the first child of parent. */
	parent->child = &node->link;
	/* Make node the parent of child. */
	child->edges[1] = &node->link;
}

static __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_drpheap_detach_child(
	const struct stroll_drpheap_link * __restrict link,
	struct stroll_drpheap_node * __restrict       parent)
{
	stroll_drpheap_assert_intern(link);
	stroll_drpheap_assert_intern(link->edges[0]);
	stroll_drpheap_assert_intern(link->edges[1]);
	stroll_drpheap_assert_intern(!stroll_drpheap_isroot_link(link->edges[1]));

	struct stroll_drpheap_link *  slink = link->edges[0];
	struct stroll_drpheap_link ** pref = (parent->child == link) ?
	                                     &parent->child :
	                                     &parent->link.edges[0];

	*pref = slink;
	slink->edges[1] = &parent->link;
}

static __stroll_nonull(1, 2, 3)
struct stroll_drpheap_node *
stroll_drpheap_join(struct stroll_drpheap_node * __restrict first,
                    struct stroll_drpheap_node * __restrict second,
                    stroll_drpheap_cmp_fn  *                compare,
                    void *                                  data)
{
	stroll_drpheap_assert_intern(first);
	stroll_drpheap_assert_intern(second);
	stroll_drpheap_assert_intern(first != second);
	stroll_drpheap_assert_intern(first->rank == second->rank);
	stroll_drpheap_assert_intern(compare);

	struct stroll_drpheap_node * parent;
	struct stroll_drpheap_node * child;

	if (compare(first, second, data) <= 0) {
		parent = first;
		child = second;
	}
	else {
		parent = second;
		child = first;
	}

	stroll_drpheap_attach_child(child, parent);

	return parent;
}

/*
 * Insertion an item into heap.
 *
 * Make item a one-node half-tree and insert it into the list of roots, in first
 * position if it has minimum key, second if not.
 */
void
stroll_drpheap_base_insert(struct stroll_drpheap_base * __restrict heap,
                           struct stroll_drpheap_node * __restrict node,
                           stroll_drpheap_cmp_fn   *               compare,
                           void *                                  data)
{
	stroll_drpheap_assert_api(heap);
	stroll_drpheap_assert_api(node);
	stroll_drpheap_assert_api(compare);

	struct stroll_drpheap_link * prev = &heap->roots;
	struct stroll_drpheap_link * next = prev->edges[0];

	if ((next != prev) &&
	    (compare(stroll_drpheap_node_from_link(next), node, data) <= 0)) {
		prev = next;
		next = next->edges[0];
	}

	node->rank = 0;
	node->child = &stroll_drpheap_plug.link;

	stroll_drpheap_inject_root_link(prev, &node->link, next);
}

static __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_drpheap_clone_roots(struct stroll_drpheap_link * __restrict       result,
                           const struct stroll_drpheap_link * __restrict source)
{
	stroll_drpheap_assert_intern(result);
	stroll_drpheap_assert_intern(source);
	stroll_drpheap_assert_intern(result != source);
	stroll_drpheap_assert_intern(source->edges[0]);
	stroll_drpheap_assert_intern(source->edges[0] != source);
	stroll_drpheap_assert_intern(
		stroll_drpheap_decode_root_link(source->edges[1]));
	stroll_drpheap_assert_intern(
		stroll_drpheap_decode_root_link(source->edges[1]) != source);

	struct stroll_drpheap_link * head = source->edges[0];
	struct stroll_drpheap_link * tail = source->edges[1];

	result->edges[0] = head;
	result->edges[1] = tail;
	head->edges[1] = stroll_drpheap_encode_root_link(result);
	stroll_drpheap_decode_root_link(tail)->edges[0] = result;
}

static __stroll_nonull(1, 2, 3) __stroll_nothrow
void
stroll_drpheap_concat_roots(struct stroll_drpheap_link *       at,
                            const struct stroll_drpheap_link * first,
                            const struct stroll_drpheap_link * second)
{
	stroll_drpheap_assert_intern(at);
	stroll_drpheap_assert_intern(first);
	stroll_drpheap_assert_intern(first->edges[0]);
	stroll_drpheap_assert_intern(first->edges[0] != first);
	stroll_drpheap_assert_intern(
		stroll_drpheap_decode_root_link(first->edges[1]));
	stroll_drpheap_assert_intern(
		stroll_drpheap_decode_root_link(first->edges[1]) != first);
	stroll_drpheap_assert_intern(second->edges[0]);
	stroll_drpheap_assert_intern(second->edges[0] != second);
	stroll_drpheap_assert_intern(
		stroll_drpheap_decode_root_link(second->edges[1]));
	stroll_drpheap_assert_intern(
		stroll_drpheap_decode_root_link(second->edges[1]) != second);
	stroll_drpheap_assert_intern(first != second);

	struct stroll_drpheap_link * fhead = first->edges[0];
	struct stroll_drpheap_link * ftail = first->edges[1];
	struct stroll_drpheap_link * shead = second->edges[0];
	struct stroll_drpheap_link * stail = second->edges[1];

	at->edges[0] = fhead;
	at->edges[1] = stail;
	fhead->edges[1] = stroll_drpheap_encode_root_link(at);
	stroll_drpheap_decode_root_link(ftail)->edges[0] = shead;
	shead->edges[1] = ftail;
	stroll_drpheap_decode_root_link(stail)->edges[0] = at;
}

/*
 * Merge 2 heaps.
 *
 * Catenate both heap lists of roots, making the root of minimum key first on
 * the new list.
 */
void
stroll_drpheap_base_merge(struct stroll_drpheap_base * __restrict       result,
                          const struct stroll_drpheap_base * __restrict source,
                          stroll_drpheap_cmp_fn *                       compare,
                          void *                                        data)
{
	stroll_drpheap_assert_api(result);
	stroll_drpheap_assert_api(source);
	stroll_drpheap_assert_api(result != source);
	stroll_drpheap_assert_api(compare);

	struct stroll_drpheap_link * shead = source->roots.edges[0];

	if (shead != &source->roots) {
		struct stroll_drpheap_link * rhead = result->roots.edges[0];

		if (rhead != &result->roots) {
			if (compare(stroll_drpheap_node_from_link(rhead),
			            stroll_drpheap_node_from_link(shead),
			            data) < 0)
				stroll_drpheap_concat_roots(&result->roots,
				                            &result->roots,
				                            &source->roots);
			else
				stroll_drpheap_concat_roots(&result->roots,
				                            &source->roots,
				                            &result->roots);
		}
		else
			stroll_drpheap_clone_roots(&result->roots,
			                           &source->roots);
	}
}

#define STROLL_DRPHEAP_BUCKETS_MAX (sizeof(unsigned int) * CHAR_BIT)

struct stroll_drpheap_slots {
	unsigned int                 bmap;
	struct stroll_drpheap_node * buckets[STROLL_DRPHEAP_BUCKETS_MAX];
};

static __stroll_nonull(1, 2, 3)
void
stroll_drpheap_merge_slots(struct stroll_drpheap_slots * __restrict slots,
                           struct stroll_drpheap_node * __restrict  node,
                           stroll_drpheap_cmp_fn *                  compare,
                           void *                                   data)
{
	stroll_drpheap_assert_intern(slots);
	stroll_drpheap_assert_intern(node);
	stroll_drpheap_assert_intern(node->rank >= 0);
	stroll_drpheap_assert_intern((unsigned int)node->rank <
	                             STROLL_DRPHEAP_BUCKETS_MAX);
	stroll_drpheap_assert_intern(
		node->rank >= stroll_drpheap_node_from_link(node->child)->rank);
	stroll_drpheap_assert_intern(node->link.edges[0]);
	stroll_drpheap_assert_intern(node->link.edges[1]);

	int rank = node->rank;

	while (slots->buckets[rank]) {
		stroll_drpheap_assert_intern((unsigned int)rank <
		                             STROLL_DRPHEAP_BUCKETS_MAX);

		node = stroll_drpheap_join(node,
		                           slots->buckets[rank],
		                           compare,
		                           data);
		stroll_bmap_clear(&slots->bmap, (unsigned int)rank);
		slots->buckets[rank++] = NULL;
	}

	stroll_drpheap_assert_intern((unsigned int)rank <
	                             STROLL_DRPHEAP_BUCKETS_MAX);

	stroll_bmap_set(&slots->bmap, (unsigned int)rank);
	slots->buckets[rank] = node;
}

static __stroll_nonull(1, 2, 3)
void
stroll_drpheap_merge_nodes(struct stroll_drpheap_slots * __restrict slots,
                           struct stroll_drpheap_link * __restrict  links,
                           stroll_drpheap_cmp_fn *                  compare,
                           void *                                   data)
{
	stroll_drpheap_assert_intern(slots);
	stroll_drpheap_assert_intern(links);

	while (links != &stroll_drpheap_plug.link) {
		struct stroll_drpheap_link * tmp = links->edges[0];
		struct stroll_drpheap_node * node =
			stroll_drpheap_node_from_link(links);

		node->rank = stroll_drpheap_node_from_link(node->child)->rank +
		             1;
		stroll_drpheap_merge_slots(slots, node, compare, data);

		links = tmp;
	}
}

static __stroll_nonull(1, 2, 3)
void
stroll_drpheap_make_roots(struct stroll_drpheap_base * __restrict        heap,
                          const struct stroll_drpheap_slots * __restrict slots,
                          stroll_drpheap_cmp_fn *                        compare,
                          void *                                         data)
{
	stroll_drpheap_assert_intern(slots);
	stroll_drpheap_assert_intern(compare);

	struct stroll_drpheap_link * roots = &heap->roots;

	roots->edges[0] = roots;
	roots->edges[1] = stroll_drpheap_encode_root_link(roots);

	if (slots->bmap) {
		unsigned int                 rank =
			stroll_bops_ffs32(slots->bmap) - 1;
		unsigned int                 iter = slots->bmap >> (rank + 1);
		struct stroll_drpheap_node * first = slots->buckets[rank];
		struct stroll_drpheap_link * lead = roots->edges[0];

		stroll_drpheap_assert_intern(first);

		while (iter) {
			unsigned int                 diff;
			struct stroll_drpheap_node * curr;
			struct stroll_drpheap_node * tmp;

			diff = stroll_bops_ffs32(iter);
			rank += diff;

			curr = slots->buckets[rank];
			if (compare(curr, first, data) < 0) {
				tmp = first;
				first = curr;
			}
			else
				tmp = curr;

			tmp->link.edges[0] = lead;
			lead->edges[1] =
				stroll_drpheap_encode_root_link(&tmp->link);
			roots->edges[0] = &tmp->link;
			tmp->link.edges[1] =
				stroll_drpheap_encode_root_link(roots);

			lead = &tmp->link;

			iter >>= diff;
		}

		first->link.edges[0] = lead;
		lead->edges[1] = stroll_drpheap_encode_root_link(&first->link);
		roots->edges[0] = &first->link;
		first->link.edges[1] = stroll_drpheap_encode_root_link(roots);
	}
}

static __stroll_nonull(1, 2, 3, 4)
void
stroll_drpheap_merge_roots(struct stroll_drpheap_base * __restrict  heap,
                           struct stroll_drpheap_slots * __restrict slots,
                           struct stroll_drpheap_link * __restrict  links,
                           stroll_drpheap_cmp_fn *                  compare,
                           void *                                   data)
{
	stroll_drpheap_assert_intern(heap);
	stroll_drpheap_assert_intern(slots);
	stroll_drpheap_assert_intern(links);
	stroll_drpheap_assert_intern(compare);

	while (links != &heap->roots) {
		struct stroll_drpheap_link * tmp = links->edges[0];

		stroll_drpheap_merge_slots(slots,
		                           stroll_drpheap_node_from_link(links),
		                           compare,
		                           data);

		links = tmp;
	}

	stroll_drpheap_make_roots(heap, slots, compare, data);
}

static __stroll_nonull(1, 2, 3, 4, 5)
void
stroll_drpheap_restore_order(struct stroll_drpheap_base * __restrict  heap,
                             struct stroll_drpheap_slots * __restrict slots,
                             struct stroll_drpheap_link * __restrict  roots,
                             struct stroll_drpheap_link * __restrict  links,
                             stroll_drpheap_cmp_fn *                  compare,
                             void *                                   data)
{
	stroll_drpheap_assert_intern(heap);
	stroll_drpheap_assert_intern(slots);
	stroll_drpheap_assert_intern(roots);
	stroll_drpheap_assert_intern(links);
	stroll_drpheap_assert_intern(roots != links);
	stroll_drpheap_assert_intern(compare);

	stroll_drpheap_merge_nodes(slots, links, compare, data);

	stroll_drpheap_merge_roots(heap, slots, roots, compare, data);
}

/*
 * Extract minimum / maximum item.
 *
 * Disassemble the half-tree rooted at the first root: delete first root and
 * and cut each edge on the right spine of the root's left child. This makes
 * each node on the right spine a new half-tree, containing itself and its
 * left subtree.
 * Assign the new half-tree roots a rank of 1 + rank of their left child.
 *
 * Do zero or more links of half-trees to reduce their number: insert resulting
 * half-trees ahead of the root list, link roots of equal rank until no two
 * roots have equal rank.
 * Build the new root list using remaining half trees, making the root of
 * minimum key the first item of the list.
 *
 * During linking, maintain a set of buckets, one per rank. To process a
 * half-tree, add it to the bucket for its rank if this bucket is empty ; if
 * not, link the half-tree with the half-tree in the bucket and add it to the
 * bucket matching the rank of the newly formed half-tree.  Once all the
 * half-trees have been processed, add any half-tree still in a bucket to the
 * final root list.
 *
 * When linking 2 roots, assign the winning root a rank of 1 + rank of its left
 * child, i.e., of looser root. As a consequence, since linking happens between
 * 2 roots of equal rank only, rank of winning root increases by 1.
 */
struct stroll_drpheap_node *
stroll_drpheap_base_extract(struct stroll_drpheap_base * __restrict heap,
                            stroll_drpheap_cmp_fn *                 compare,
                            void *                                  data)
{
	stroll_drpheap_assert_api(heap);
	stroll_drpheap_assert_api(!stroll_drpheap_base_isempty(heap));
	stroll_drpheap_assert_api(compare);

	struct stroll_drpheap_link * lead = heap->roots.edges[0];
	struct stroll_drpheap_slots  slots = {
		.bmap    = STROLL_BMAP_INIT_CLEAR,
		.buckets = { NULL, }
	};

	stroll_drpheap_restore_order(heap,
	                             &slots,
	                             lead->edges[0],
	                             stroll_drpheap_node_from_link(lead)->child,
	                             compare,
	                             data);

	return stroll_drpheap_node_from_link(lead);
}

static __stroll_nonull(1) __stroll_nothrow
void
stroll_drpheap_remove_node(struct stroll_drpheap_link * __restrict link)
{
	stroll_drpheap_assert_intern(link);
	stroll_drpheap_assert_intern(
		!stroll_drpheap_isroot_link(link->edges[1]));

	struct stroll_drpheap_link *       plink; /* parent link */
	struct stroll_drpheap_node *       pnode; /* parent node */
	const struct stroll_drpheap_node * cnode; /* first / left parent's child
	                                             node */

	plink = link->edges[1];
	pnode = stroll_drpheap_node_from_link(plink);
	stroll_drpheap_detach_child(link, pnode);

	for (cnode = stroll_drpheap_node_from_link(pnode->child);
	     !stroll_drpheap_isroot_link(plink->edges[1]);
	     plink = plink->edges[1],
	     pnode = stroll_drpheap_node_from_link(plink),
	     cnode = stroll_drpheap_node_from_link(pnode->child)) {
		const struct stroll_drpheap_node * snode; /* right sibling */
		int                                rank;

		snode = stroll_drpheap_node_from_link(plink->edges[0]);

		/* Restore ranks according to type-2 rank rule. */
		rank = stroll_max(cnode->rank, snode->rank) +
		       (int)(stroll_abs(cnode->rank - snode->rank) <= 1);
		if (rank == pnode->rank)
			return;

		stroll_drpheap_assert_intern(rank < pnode->rank);
		pnode->rank = rank;
	}

	pnode->rank = cnode->rank + 1;
}

void
stroll_drpheap_base_remove(struct stroll_drpheap_base * __restrict heap,
                           struct stroll_drpheap_node * __restrict node,
                           stroll_drpheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_drpheap_assert_api(heap);
	stroll_drpheap_assert_api(!stroll_drpheap_base_isempty(heap));
	stroll_drpheap_assert_api(node);
	stroll_drpheap_assert_api(compare);

	if (!stroll_drpheap_isroot_link(node->link.edges[1]))
		stroll_drpheap_remove_node(&node->link);
	else
		stroll_drpheap_remove_root_link(&node->link);

	if ((node->child != &stroll_drpheap_plug.link) ||
	    (node->link.edges[0] == heap->roots.edges[0])) {
		struct stroll_drpheap_slots slots = {
			.bmap    = STROLL_BMAP_INIT_CLEAR,
			.buckets = { NULL, }
		};

		stroll_drpheap_restore_order(heap,
		                             &slots,
		                             heap->roots.edges[0],
		                             node->child,
		                             compare,
		                             data);
	}
}

/*
 * Decrease item.
 *
 * Reduce the item key.
 * If the item is a root, make it the first root if its key is now minimum.
 * If not, detach the subtrees rooted at the item and at its right child y, reattach the subtree rooted at y in place of the
 * one rooted at x, and add x to the list of roots, in first position if its key is minimum, second
 * if not. Finish by restoring the rank rule: make the rank of x one greater than that of its left
 * child; and, starting at the new parent of y, walk up through ancestors, reducing their ranks to
 * obey the rank rule, until reaching the root or reaching a node whose rank needs no reduction.
 * To do the rank reductions, let u = p(y) and repeat the following step until it stops:
 */
void
stroll_drpheap_base_promote(struct stroll_drpheap_base * __restrict heap,
                            struct stroll_drpheap_node * __restrict node,
                            stroll_drpheap_cmp_fn *                 compare,
                            void *                                  data)
{
	stroll_drpheap_assert_api(heap);
	stroll_drpheap_assert_api(node);
	stroll_drpheap_assert_api(!stroll_drpheap_base_isempty(heap));
	stroll_drpheap_assert_api(compare);

	struct stroll_drpheap_link * lead = heap->roots.edges[0];
	bool                         small =
		compare(node, stroll_drpheap_node_from_link(lead), data) < 0;

	if (!stroll_drpheap_isroot_link(node->link.edges[1])) {
		stroll_drpheap_remove_node(&node->link);

		if (small)
			stroll_drpheap_inject_root_link(&heap->roots,
			                                &node->link,
			                                lead);
		else
			stroll_drpheap_append_root_link(lead, &node->link);
	}
	else if ((lead != &node->link) && small) {
		stroll_drpheap_remove_root_link(&node->link);
		stroll_drpheap_inject_root_link(&heap->roots,
		                                &node->link,
		                                lead);
	}
}

void
stroll_drpheap_base_demote(struct stroll_drpheap_base * __restrict heap,
                           struct stroll_drpheap_node * __restrict node,
                           stroll_drpheap_cmp_fn *                 compare,
                           void *                                  data)
{
	stroll_drpheap_assert_api(heap);
	stroll_drpheap_assert_api(node);
	stroll_drpheap_assert_api(!stroll_drpheap_base_isempty(heap));
	stroll_drpheap_assert_api(compare);

	struct stroll_drpheap_link * clinks = node->child;
	struct stroll_drpheap_link * nlink = &node->link;

	if ((clinks != &stroll_drpheap_plug.link) ||
	    (nlink == heap->roots.edges[0])) {
		struct stroll_drpheap_slots slots = {
			.bmap    = 1U,
			.buckets = { node, NULL, }
		};

		if (!stroll_drpheap_isroot_link(nlink->edges[1]))
			stroll_drpheap_remove_node(nlink);
		else
			stroll_drpheap_remove_root_link(nlink);

		node->rank = 0;
		node->child = &stroll_drpheap_plug.link;

		stroll_drpheap_restore_order(heap,
		                             &slots,
		                             heap->roots.edges[0],
		                             clinks,
		                             compare,
		                             data);
	}
}

void
stroll_drpheap_insert(struct stroll_drpheap * __restrict      heap,
                      struct stroll_drpheap_node * __restrict node,
                      void *                                  data)
{
	stroll_drpheap_assert_heap_api(heap);
	stroll_drpheap_assert_api(heap->cnt < heap->nr);

	stroll_drpheap_base_insert(&heap->base, node, heap->compare, data);

	heap->cnt++;
}

void
stroll_drpheap_merge(struct stroll_drpheap * __restrict       result,
                     const struct stroll_drpheap * __restrict source,
                     void *                                   data)
{
	stroll_drpheap_assert_heap_api(result);
	stroll_drpheap_assert_heap_api(source);
	stroll_drpheap_assert_api(result != source);
	stroll_drpheap_assert_api(result->compare == source->compare);
	stroll_drpheap_assert_api((result->cnt + source->cnt) <= result->nr);

	stroll_drpheap_base_merge(&result->base,
	                          &source->base,
	                          result->compare,
	                          data);

	result->cnt += source->cnt;
}

struct stroll_drpheap_node *
stroll_drpheap_extract(struct stroll_drpheap * __restrict heap, void * data)
{
	stroll_drpheap_assert_heap_api(heap);
	stroll_drpheap_assert_api(heap->cnt);

	struct stroll_drpheap_node * node;

	node = stroll_drpheap_base_extract(&heap->base, heap->compare, data);

	heap->cnt--;

	return node;
}

void
stroll_drpheap_remove(struct stroll_drpheap * __restrict      heap,
                      struct stroll_drpheap_node * __restrict node,
                      void *                                  data)
{
	stroll_drpheap_assert_heap_api(heap);
	stroll_drpheap_assert_api(heap->cnt);

	stroll_drpheap_base_remove(&heap->base, node, heap->compare, data);

	heap->cnt--;
}

void
stroll_drpheap_setup(struct stroll_drpheap * __restrict heap,
                     unsigned int                       nr,
                     stroll_drpheap_cmp_fn *            compare)
{
	stroll_drpheap_assert_api(heap);
	stroll_drpheap_assert_api(nr);
	stroll_drpheap_assert_api(compare);

	heap->cnt = 0;
	heap->nr = nr;
	stroll_drpheap_base_setup(&heap->base);
	heap->compare = compare;
}

struct stroll_drpheap *
stroll_drpheap_create(unsigned int nr, stroll_drpheap_cmp_fn * compare)
{
	stroll_drpheap_assert_api(nr);
	stroll_drpheap_assert_api(compare);

	struct stroll_drpheap * heap;

	heap = malloc(sizeof(*heap));
	if (!heap)
		return NULL;

	stroll_drpheap_setup(heap, nr, compare);

	return heap;
}
