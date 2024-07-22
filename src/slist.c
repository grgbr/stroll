/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/slist.h"
#include "stroll/pow2.h"

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_slist_assert_intern(_expr) \
	stroll_assert("stroll:slist", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_slist_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/******************************************************************************
 * Singly linked list primitives
 ******************************************************************************/

void
stroll_slist_move(struct stroll_slist *                 list,
                  struct stroll_slist_node * __restrict at,
                  struct stroll_slist_node * __restrict previous,
                  struct stroll_slist_node * __restrict node)
{
	stroll_slist_assert_api(list);
	stroll_slist_assert_api(at);
	stroll_slist_assert_api(previous);
	stroll_slist_assert_api(node);

	stroll_slist_remove(list, previous, node);
	stroll_slist_append(list, at, node);
}

void
stroll_slist_splice(struct stroll_slist * __restrict      result,
                    struct stroll_slist_node * __restrict at,
                    struct stroll_slist * __restrict      source,
                    struct stroll_slist_node * __restrict first,
                    struct stroll_slist_node * __restrict last)
{
	stroll_slist_assert_api(result);
	stroll_slist_assert_api(at);
	stroll_slist_assert_api(source);
	stroll_slist_assert_api(first);
	stroll_slist_assert_api(last);
	stroll_slist_assert_api(at != first);
	stroll_slist_assert_api(at != last);
	stroll_slist_assert_api(first != last);
	stroll_slist_assert_api(result != source);

	struct stroll_slist_node * fst = stroll_slist_next(first);

	stroll_slist_withdraw(source, first, last);
	stroll_slist_embed(result, at, fst, last);
}

/******************************************************************************
 * Singly linked list bubble sorting
 ******************************************************************************/

#if defined(CONFIG_STROLL_SLIST_BUBBLE_SORT)

void
stroll_slist_bubble_sort(struct stroll_slist * __restrict list,
                         stroll_slist_cmp_fn *            compare,
                         void *                           data)
{
	stroll_slist_assert_api(!stroll_slist_empty(list));
	stroll_slist_assert_api(compare);

	struct stroll_slist_node * sort = NULL;
	struct stroll_slist_node * swap;

	do {
		struct stroll_slist_node * prev = stroll_slist_head(list);
		struct stroll_slist_node * curr;

		swap = NULL;

		/* For each node in the list... */
		while (true) {
			struct stroll_slist_node * next;

			/* Find next out-of-order node. */
			while (true) {
				curr = stroll_slist_next(prev);
				next = stroll_slist_next(curr);
				if (!next || (next == sort))
					break;

				if (compare(curr, next, data) > 0)
					break;

				prev = curr;
			}

			if (!next || (next == sort)) {
				/*
				 * No more out-of-order node: current pass is
				 * over.
				 */
				sort = curr;
				break;
			}

			/* Remove current out-of-order node. */
			stroll_slist_remove(list, prev, curr);

			/*
			 * Keep a pointer to it for later insertion.  `swap' now
			 * points to the last node inserted in order for the
			 * current pass, i.e., points to the first node of the
			 * final sorted list.
			 */
			swap = curr;

			/* Find in order location for out-of-order node. */
			curr = next;
			while (true) {
				prev = curr;
				curr = stroll_slist_next(curr);
				if (curr == sort)
					curr = NULL;
				if (!curr)
					break;

				if (compare(curr, swap, data) >= 0)
					break;
			}

			/* Insert it in order. */
			stroll_slist_append(list, prev, swap);

			if (!curr) {
				sort = swap;
				break;
			}

			prev = swap;
		}

		/* Do this as long as current pass swapped at least one node. */
	} while (swap);
}

#endif /* defined(CONFIG_STROLL_SLIST_BUBBLE_SORT) */

/******************************************************************************
 * Singly linked list selection sorting
 ******************************************************************************/

#if defined(CONFIG_STROLL_SLIST_SELECT_SORT)

/*
 * Thanks to stroll_slist, implementation is always stable: swap is always
 * performed after the tail of sorted sublist.
 */
void
stroll_slist_select_sort(struct stroll_slist * __restrict list,
                         stroll_slist_cmp_fn *            compare,
                         void *                           data)
{
	stroll_slist_assert_api(!stroll_slist_empty(list));
	stroll_slist_assert_api(compare);

	/* Tail of sorted sublist. */
	struct stroll_slist_node * tail = stroll_slist_head(list);

	while (true) {
		stroll_slist_assert_intern(tail);

		struct stroll_slist_node * prev = stroll_slist_next(tail);
		struct stroll_slist_node * curr;
		struct stroll_slist_node * prev_min;
		struct stroll_slist_node * curr_min;

		if (prev == stroll_slist_last(list))
			break;

		curr = stroll_slist_next(prev);
		prev_min = tail;
		curr_min = prev;

		do {
			if (compare(curr, curr_min, data) < 0) {
				prev_min = prev;
				curr_min = curr;
			}

			prev = curr;
			curr = stroll_slist_next(curr);
		} while (curr);

		if (curr_min != stroll_slist_next(tail))
			stroll_slist_move(list, tail, prev_min, curr_min);

		tail = curr_min;
	}
}

#endif /* defined(CONFIG_STROLL_SLIST_SELECT_SORT) */

/******************************************************************************
 * Singly linked list insertion sorting
 ******************************************************************************/

#if defined(CONFIG_STROLL_SLIST_INSERT_SORT_UTILS)

/*
 * Insert node into stroll_slist in order.
 *
 * @param list    stroll_slist to insert node into
 * @param node    node to insert
 * @param compare comparison function used to perform in order insertion
 */
static __stroll_nonull(1, 2, 3)
void
stroll_slist_insert_inorder(struct stroll_slist * __restrict      list,
                            struct stroll_slist_node * __restrict node,
                            stroll_slist_cmp_fn *                 compare,
                            void *                                data)
{
	stroll_slist_assert_intern(list);
	stroll_slist_assert_intern(node);
	stroll_slist_assert_intern(compare);

	struct stroll_slist_node * prev = stroll_slist_head(list);
	struct stroll_slist_node * curr = stroll_slist_next(prev);

	/*
	 * No need to check for end of list since curr must be inserted before
	 * current sorted list tail, i.e., curr will always be inserted before
	 * an existing node.
	 */
	while (compare(node, curr, data) >= 0) {
		stroll_slist_assert_intern(curr);

		/*
		 * Although it seems a pretty good place to perform some
		 * prefetching, performance measurement doesn't show significant
		 * improvements... For most data sets, prefetching enhance
		 * processing time by an amount < 4/5%. In the worst case, this
		 * incurs a 3/4% penalty.  Measurements were done onto amd64
		 * platform with moderate read-only prefetching scheme (giving
		 * the best results), i.e. :
		 *   stroll_prefetch(stroll_slist_next(curr),
		 *                   STROLL_PREFETCH_ACCESS_RO,
		 *                   STROLL_PREFETCH_LOCALITY_LOW)
		 *
		 * Additional code and complexity don't really worth it...
		 */
		prev = curr;
		curr = stroll_slist_next(curr);
	}

	stroll_slist_append(list, prev, node);
}

#endif /* defined(CONFIG_STROLL_SLIST_INSERT_SORT_UTILS) */

#if defined(CONFIG_STROLL_SLIST_INSERT_SORT)

void
stroll_slist_insert_sort(struct stroll_slist * __restrict list,
                         stroll_slist_cmp_fn *            compare,
                         void *                           data)
{
	stroll_slist_assert_api(!stroll_slist_empty(list));
	stroll_slist_assert_api(compare);

	struct stroll_slist_node * prev = stroll_slist_first(list);
	struct stroll_slist_node * curr = stroll_slist_next(prev);

	while (curr) {
		if (compare(curr, prev, data) < 0) {
			stroll_slist_remove(list, prev, curr);
			stroll_slist_insert_inorder(list, curr, compare, data);

			curr = stroll_slist_next(prev);
			continue;
		}

		prev = curr;
		curr = stroll_slist_next(curr);
	}
}

#endif /* defined(CONFIG_STROLL_SLIST_INSERT_SORT) */

/******************************************************************************
 * Singly linked list hybrid merge sorting
 ******************************************************************************/

#if defined(CONFIG_STROLL_SLIST_MERGE_SORT)

#define STROLL_SLIST_MSORT_INSERT_THRESHOLD \
	STROLL_CONCAT(CONFIG_STROLL_SLIST_MERGE_SORT_INSERT_THRESHOLD, U)

/*
 * @internal
 *
 * Sort a fixed number of nodes from a stroll_slist according to the
 * insertion sort scheme and move sorted nodes into a result stroll_slist.
 *
 * @param[inout] result  Result stroll_slist where sorted nodes are moved.
 * @param[inout] source  Source stroll_slist to sort.
 * @param[in]    compare stroll_list nodes comparison function.
 * @param[inout] data    Optional arbitrary user data.
 *
 * @warning
 * - Behavior is undefined when called on a non empty @p result stroll_slist.
 * - Behavior is undefined when called on an empty @p source stroll_slist.
 */
static __stroll_nonull(1, 2, 4)
void
stroll_slist_merge_insert_sort(struct stroll_slist * __restrict result,
                               struct stroll_slist * __restrict source,
                               stroll_slist_cmp_fn *            compare,
                               void *                           data)
{
	stroll_slist_assert_intern(stroll_slist_empty(result));
	stroll_slist_assert_intern(!stroll_slist_empty(source));
	stroll_slist_assert_intern(compare);

	unsigned int               cnt = STROLL_SLIST_MSORT_INSERT_THRESHOLD;
	struct stroll_slist_node * prev = stroll_slist_first(source);
	struct stroll_slist_node * curr = stroll_slist_next(prev);

	while (--cnt && curr) {
		if (compare(curr, prev, data) < 0) {
			stroll_slist_remove(source, prev, curr);
			stroll_slist_insert_inorder(source,
			                            curr,
			                            compare,
			                            data);

			curr = stroll_slist_next(prev);
			continue;
		}

		prev = curr;
		curr = stroll_slist_next(curr);
	}

	stroll_slist_splice(result,
	                    stroll_slist_head(result),
	                    source,
	                    stroll_slist_head(source),
	                    prev);
}

/*
 * @internal
 *
 * Merge 2 sorted (sub)lists segments.
 *
 * @param[inout] result  stroll_slist within which both stroll_slist will be
 *                       sorted into.
 * @param[inout] at      First node of result's segment.
 * @param[inout] source  Source stroll_slist to sort within @p result.
 * @param[in]    compare comparison function used to perform in order merge.
 *
 * @return Last sorted node merged into result.
 */
static __stroll_nonull(1, 2, 3, 4)
struct stroll_slist_node *
stroll_slist_merge_sorted_subs(struct stroll_slist *      result,
                               struct stroll_slist_node * at,
                               struct stroll_slist *      source,
                               stroll_slist_cmp_fn *      compare,
                               void *                     data)
{
	stroll_slist_assert_intern(!stroll_slist_empty(result));
	stroll_slist_assert_intern(at);
	stroll_slist_assert_intern(!stroll_slist_empty(source));

	struct stroll_slist_node * res_cur = at;
	struct stroll_slist_node * res_nxt = at;
	struct stroll_slist_node * ref = stroll_slist_first(source);
	struct stroll_slist_node * src_cur;
	struct stroll_slist_node * src_nxt;

	if (compare(ref, stroll_slist_last(result), data) >= 0) {
		/*
		 * Fast path: don't bother sorting every nodes if source list
		 * comes entirely after result one.
		 */
		res_cur = stroll_slist_last(result);
		src_cur = stroll_slist_last(source);
		goto splice;
	}

	/*
	 * Find the first node in result which should be located after first
	 * source list's node.
	 */
	while (true) {
		res_nxt = stroll_slist_next(res_nxt);
		if (!res_nxt)
			break;

		if (compare(res_nxt, ref, data) > 0)
			break;

		res_cur = res_nxt;
	}

	src_cur = stroll_slist_last(source);

	if (!res_nxt)
		/* Nothing's left in result: join source after result. */
		goto splice;

	if (compare(res_nxt, src_cur, data) > 0)
		/*
		 * The entire source list may be inserted before current
		 * result's node.
		 */
		goto splice;

	/*
	 * Search for the longest source nodes segment that may be inserted
	 * before current result's node.
	 */
	src_cur = stroll_slist_head(source);
	src_nxt = src_cur;
	while (true) {
		src_nxt = stroll_slist_next(src_nxt);
		if (!src_nxt)
			break;

		if (compare(src_nxt, res_nxt, data) >= 0)
			break;

		src_cur = src_nxt;
	}

	/* Embed source nodes segment at the right location into result. */
splice:
	stroll_slist_splice(result,
	                    res_cur,
	                    source,
	                    stroll_slist_head(source),
	                    src_cur);

	/* Return last sorted node. */
	return src_cur;
}

void
stroll_slist_merge_presort(struct stroll_slist * __restrict result,
                           struct stroll_slist * __restrict source,
                           stroll_slist_cmp_fn *            compare,
                           void *                           data)
{
	stroll_slist_assert_api(!stroll_slist_empty(result));
	stroll_slist_assert_api(!stroll_slist_empty(source));
	stroll_slist_assert_api(compare);

	struct stroll_slist_node * at = stroll_slist_head(result);

	do {
		at = stroll_slist_merge_sorted_subs(result,
		                                    at,
		                                    source,
		                                    compare,
		                                    data);
	} while (!stroll_slist_empty(source));
}

void
stroll_slist_merge_sort(struct stroll_slist * __restrict list,
                        stroll_slist_cmp_fn *            compare,
                        void *                           data)
{
	stroll_slist_assert_api(!stroll_slist_empty(list));
	stroll_slist_assert_api(compare);

	/*
	 * Maximum number of sublists that can be merged. Sublist heads are
	 * allocated onto stack since implementation needs logarithmic auxiliary
	 * space only.
	 * Here, a maximum of 2**32 nodes may be merged.
	 */
#define STROLL_SLIST_MSORT_SUBS_MAX \
	((sizeof(unsigned int) * CHAR_BIT) - \
	 stroll_pow2_up(STROLL_SLIST_MSORT_INSERT_THRESHOLD) \
	 + 2)

	unsigned int        cnt;
        unsigned int        nr = 0;
	struct stroll_slist subs[STROLL_SLIST_MSORT_SUBS_MAX];

	for (cnt = 0; cnt < STROLL_SLIST_MSORT_SUBS_MAX; cnt++)
		stroll_slist_init(&subs[cnt]);

	/* Iteratively split list and merge presorted sublists. */
	do {
		/*
		 * Sort an initial run using insertion sort and store the result
		 * in the current sublist.
		 */
		stroll_slist_merge_insert_sort(&subs[0], list, compare, data);

		/* Merge up 2 runs / sublists of the same length in a row.*/
		cnt = 1;
		while (!stroll_slist_empty(&subs[cnt])) {
			stroll_slist_merge_presort(&subs[cnt],
			                           &subs[cnt - 1],
			                           compare,
			                           data);
			cnt++;
			stroll_slist_assert_intern(cnt <
			                           STROLL_SLIST_MSORT_SUBS_MAX);
		}

		subs[cnt] = subs[cnt - 1];
		stroll_slist_init(&subs[cnt - 1]);

		nr = stroll_max(nr, cnt);
	} while (!stroll_slist_empty(list));

	/*
	 * Finally merge remaining runs into a single sorted list. To preserve
	 * stability, iterate over the sublists array in reverse order since
	 * earlier runs were "pushed" in the upper areas of the array.
	 */
	*list = subs[nr];
	while (nr--)
		if (!stroll_slist_empty(&subs[nr]))
			stroll_slist_merge_presort(list,
			                           &subs[nr],
			                           compare,
			                           data);
}

#endif /* defined(CONFIG_STROLL_SLIST_MERGE_SORT) */
