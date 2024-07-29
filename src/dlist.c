/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/dlist.h"
#include "stroll/pow2.h"

void
stroll_dlist_splice_before(struct stroll_dlist_node * __restrict at,
                           struct stroll_dlist_node *            first,
                           struct stroll_dlist_node *            last)
{
	stroll_dlist_assert_api(at);
	stroll_dlist_assert_api(first);
	stroll_dlist_assert_api(!stroll_dlist_empty(first));
	stroll_dlist_assert_api(last);
	stroll_dlist_assert_api(!stroll_dlist_empty(last));
	stroll_dlist_assert_api(at != first);
	stroll_dlist_assert_api(at != last);

	stroll_dlist_withdraw(first, last);
	stroll_dlist_embed_before(at, first, last);
}

void
stroll_dlist_splice_after(struct stroll_dlist_node * __restrict at,
                          struct stroll_dlist_node *            first,
                          struct stroll_dlist_node *            last)
{
	stroll_dlist_assert_api(at);
	stroll_dlist_assert_api(first);
	stroll_dlist_assert_api(last);
	stroll_dlist_assert_api(at != first);
	stroll_dlist_assert_api(at != last);

	stroll_dlist_withdraw(first, last);
	stroll_dlist_embed_after(at, first, last);
}

/******************************************************************************
 * Doubly linked list bubble sorting
 ******************************************************************************/

#if defined(CONFIG_STROLL_DLIST_BUBBLE_SORT)

void
stroll_dlist_bubble_sort(struct stroll_dlist_node * __restrict head,
                         stroll_dlist_cmp_fn *                 compare,
                         void *                                data)
{
	stroll_dlist_assert_api(!stroll_dlist_empty(head));
	stroll_dlist_assert_api(compare);

	struct stroll_dlist_node * sort = head;
	struct stroll_dlist_node * swap;

	do {
		stroll_dlist_assert_intern(!stroll_dlist_empty(sort));

		struct stroll_dlist_node * curr = stroll_dlist_next(head);

		swap = NULL;

		/* For each node in the list... */
		while (true) {
			struct stroll_dlist_node * next;

			/* Find next out-of-order node. */
			while (true) {
				next = stroll_dlist_next(curr);
				if (next == sort)
					break;

				if (compare(curr, next, data) > 0)
					break;

				curr = next;
			}

			if (next == sort) {
				/*
				 * No more out-of-order node: current pass is
				 * over.
				 */
				sort = curr;
				break;
			}

			/* Remove current out-of-order node. */
			stroll_dlist_remove(curr);

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
				curr = stroll_dlist_next(curr);
				if (curr == sort)
					break;

				if (compare(curr, swap, data) >= 0)
					break;
			}

			/* Insert it in order. */
			stroll_dlist_insert(curr, swap);

			if (curr == sort) {
				sort = swap;
				break;
			}
		}

		/* Do this as long as current pass swapped at least one node. */
	} while (swap);
}

#endif /* defined(CONFIG_STROLL_DLIST_BUBBLE_SORT) */

/******************************************************************************
 * Doubly linked list selection sorting
 ******************************************************************************/

#if defined(CONFIG_STROLL_DLIST_SELECT_SORT)

/*
 * Thanks to stroll_dlist_node, implementation is always stable: swap is always
 * performed after the tail of sorted sublist.
 */
void
stroll_dlist_select_sort(struct stroll_dlist_node * __restrict head,
                         stroll_dlist_cmp_fn *                 compare,
                         void *                                data)
{
	stroll_dlist_assert_api(!stroll_dlist_empty(head));
	stroll_dlist_assert_api(compare);

	struct stroll_dlist_node unsort = STROLL_DLIST_INIT(unsort);

	stroll_dlist_splice_after(&unsort,
	                          stroll_dlist_next(head),
	                          stroll_dlist_prev(head));

	do {
		struct stroll_dlist_node * curr;
		struct stroll_dlist_node * sort = stroll_dlist_next(&unsort);

		stroll_dlist_foreach_node(&unsort, curr) {
			if (compare(curr, sort, data) < 0)
				sort = curr;
		}

		stroll_dlist_move_before(head, sort);
	} while (!stroll_dlist_empty(&unsort));
}

#endif /* defined(CONFIG_STROLL_DLIST_SELECT_SORT) */

/******************************************************************************
 * Doubly linked list insertion sorting
 ******************************************************************************/

#if defined(CONFIG_STROLL_DLIST_INSERT_SORT_UTILS)

/*
 * Insert node into stroll_dlist_node list in order.
 *
 * @param head    head of list to insert @p node into
 * @param node    node to insert
 * @param compare comparison function used to perform in order insertion
 */
static __stroll_nonull(1, 2, 3)
void
stroll_dlist_insert_inorder(struct stroll_dlist_node * __restrict head,
                            struct stroll_dlist_node * __restrict node,
                            stroll_dlist_cmp_fn *                 compare,
                            void *                                data)
{
	stroll_dlist_assert_intern(head);
	stroll_dlist_assert_intern(node);
	stroll_dlist_assert_intern(compare);

	struct stroll_dlist_node * curr = stroll_dlist_next(head);

	/*
	 * No need to check for end of list since curr must be inserted before
	 * current sorted list tail, i.e., curr will always be inserted before
	 * an existing node.
	 */
	while (compare(node, curr, data) >= 0) {
		stroll_dlist_assert_intern(curr != head);

		/*
		 * Although it seems a pretty good place to perform some
		 * prefetching, performance measurement doesn't show significant
		 * improvements... For most data sets, prefetching enhance
		 * processing time by an amount < 4/5%. In the worst case, this
		 * incurs a 3/4% penalty.  Measurements were done onto amd64
		 * platform with moderate read-only prefetching scheme (giving
		 * the best results), i.e. :
		 *   stroll_prefetch(stroll_dlist_next(curr),
		 *                   STROLL_PREFETCH_ACCESS_RO,
		 *                   STROLL_PREFETCH_LOCALITY_LOW)
		 *
		 * Additional code and complexity don't really worth it...
		 */
		curr = stroll_dlist_next(curr);
	}

	stroll_dlist_insert(curr, node);
}

#endif /* defined(CONFIG_STROLL_DLIST_INSERT_SORT_UTILS) */

#if defined(CONFIG_STROLL_DLIST_INSERT_SORT)

void
stroll_dlist_insert_sort(struct stroll_dlist_node * __restrict head,
                         stroll_dlist_cmp_fn *                 compare,
                         void *                                data)
{
	stroll_dlist_assert_api(!stroll_dlist_empty(head));
	stroll_dlist_assert_api(compare);

	struct stroll_dlist_node * prev = stroll_dlist_next(head);
	struct stroll_dlist_node * curr = stroll_dlist_next(prev);

	while (curr != head) {
		struct stroll_dlist_node * next = stroll_dlist_next(curr);

		if (compare(curr, prev, data) < 0) {
			stroll_dlist_remove(curr);
			stroll_dlist_insert_inorder(head, curr, compare, data);
		}
		else
			prev = curr;
		curr = next;
	}
}

#endif /* defined(CONFIG_STROLL_DLIST_INSERT_SORT) */

/******************************************************************************
 * Doubly linked list hybrid merge sorting
 ******************************************************************************/

#if defined(CONFIG_STROLL_DLIST_MERGE_SORT)

#define STROLL_DLIST_MSORT_INSERT_THRESHOLD \
	STROLL_CONCAT(CONFIG_STROLL_DLIST_MERGE_SORT_INSERT_THRESHOLD, U)

/*
 * @internal
 *
 * Sort a fixed number of nodes from a stroll_dlist_node list according to the
 * insertion sort scheme and move sorted nodes into a result stroll_dlist_node
 * list.
 *
 * @param[inout] result  stroll_dlist_node result head of list where sorted
 *                       nodes are moved.
 * @param[inout] source  Source stroll_dlist_node head of list to sort.
 * @param[in]    compare stroll_dlist_node nodes comparison function.
 * @param[inout] data    Optional arbitrary user data.
 *
 * @warning
 * - Behavior is undefined when called on a non empty @p result
 *   stroll_dlist_node list.
 * - Behavior is undefined when called on an empty @p source stroll_dlist_node
 *   list.
 */
static __stroll_nonull(1, 2, 4)
void
stroll_dlist_merge_insert_sort(struct stroll_dlist_node * __restrict result,
                               struct stroll_dlist_node * __restrict source,
                               stroll_dlist_cmp_fn *                 compare,
                               void *                                data)
{
	stroll_dlist_assert_intern(stroll_dlist_empty(result));
	stroll_dlist_assert_intern(!stroll_dlist_empty(source));
	stroll_dlist_assert_intern(compare);

	unsigned int               cnt = STROLL_DLIST_MSORT_INSERT_THRESHOLD;
	struct stroll_dlist_node * prev = stroll_dlist_next(source);
	struct stroll_dlist_node * curr = stroll_dlist_next(prev);

	while (--cnt && (curr != source)) {
		struct stroll_dlist_node * next = stroll_dlist_next(curr);

		if (compare(curr, prev, data) < 0) {
			stroll_dlist_remove(curr);
			stroll_dlist_insert_inorder(source,
			                            curr,
			                            compare,
			                            data);
		}
		else
			prev = curr;
		curr = next;
	}

	stroll_dlist_splice_after(result, stroll_dlist_next(source), prev);
}

/*
 * @internal
 *
 * Merge 2 sorted (sub)lists segments.
 *
 * @param[inout] result  stroll_dlist_node head of list within which both lists
 *                       will be sorted into.
 * @param[inout] at      First node of result's segment.
 * @param[inout] source  Source stroll_dlist_node head of list to sort within @p
 *                       result.
 * @param[in]    compare comparison function used to perform in order merge.
 *
 * @return Last sorted node merged into result.
 */
static __stroll_nonull(1, 2, 3, 4)
struct stroll_dlist_node *
stroll_dlist_merge_sorted_subs(struct stroll_dlist_node * result,
                               struct stroll_dlist_node * at,
                               struct stroll_dlist_node * source,
                               stroll_dlist_cmp_fn *      compare,
                               void *                     data)
{
	stroll_dlist_assert_intern(!stroll_dlist_empty(result));
	stroll_dlist_assert_intern(at);
	stroll_dlist_assert_intern(!stroll_dlist_empty(source));

	struct stroll_dlist_node * ref = stroll_dlist_next(source);
	struct stroll_dlist_node * res_cur;
	struct stroll_dlist_node * res_nxt;
	struct stroll_dlist_node * src_cur;
	struct stroll_dlist_node * src_nxt;

	if (compare(ref, stroll_dlist_prev(result), data) >= 0) {
		/*
		 * Fast path: don't bother sorting every nodes if source list
		 * comes entirely after result one.
		 */
		res_cur = stroll_dlist_prev(result);
		src_cur = stroll_dlist_prev(source);
		goto splice;
	}

	/*
	 * Find the first node in result which should be located after first
	 * source list's node.
	 */
	res_cur = at;
	res_nxt = stroll_dlist_next(at);
	while (res_nxt != result) {
		if (compare(res_nxt, ref, data) > 0)
			break;

		res_cur = res_nxt;
		res_nxt = stroll_dlist_next(res_nxt);
	}

	src_cur = stroll_dlist_prev(source);

	if (res_nxt == result)
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
	src_cur = source;
	src_nxt = stroll_dlist_next(source);
	while (src_nxt != source) {
		if (compare(src_nxt, res_nxt, data) >= 0)
			break;

		src_cur = src_nxt;
		src_nxt = stroll_dlist_next(src_nxt);
	}

	/* Embed source nodes segment at the right location into result. */
splice:
	stroll_dlist_splice_after(res_cur, stroll_dlist_next(source), src_cur);

	/* Return last sorted node. */
	return src_cur;
}

void
stroll_dlist_merge_presort(struct stroll_dlist_node * __restrict result,
                           struct stroll_dlist_node * __restrict source,
                           stroll_dlist_cmp_fn *                 compare,
                           void *                                data)
{
	stroll_dlist_assert_api(!stroll_dlist_empty(result));
	stroll_dlist_assert_api(!stroll_dlist_empty(source));
	stroll_dlist_assert_api(compare);

	struct stroll_dlist_node * at = result;

	do {
		at = stroll_dlist_merge_sorted_subs(result,
		                                    at,
		                                    source,
		                                    compare,
		                                    data);
	} while (!stroll_dlist_empty(source));
}

void
stroll_dlist_merge_sort(struct stroll_dlist_node * __restrict head,
                        stroll_dlist_cmp_fn *                 compare,
                        void *                                data)
{
	stroll_dlist_assert_api(!stroll_dlist_empty(head));
	stroll_dlist_assert_api(compare);

	/*
	 * Maximum number of sublists that can be merged. Sublist heads are
	 * allocated onto stack since implementation needs logarithmic auxiliary
	 * space only.
	 * Here, a maximum of 2**32 nodes may be merged.
	 */
#define STROLL_DLIST_MSORT_SUBS_MAX \
	((sizeof(unsigned int) * CHAR_BIT) - \
	 stroll_pow2_up(STROLL_DLIST_MSORT_INSERT_THRESHOLD) \
	 + 2)

	unsigned int             cnt;
        unsigned int             nr = 0;
	struct stroll_dlist_node subs[STROLL_DLIST_MSORT_SUBS_MAX];

	for (cnt = 0; cnt < STROLL_DLIST_MSORT_SUBS_MAX; cnt++)
		stroll_dlist_init(&subs[cnt]);

	/* Iteratively split list and merge presorted sublists. */
	do {
		/*
		 * Sort an initial run using insertion sort and store the result
		 * in the current sublist.
		 */
		stroll_dlist_merge_insert_sort(&subs[0], head, compare, data);

		/* Merge up 2 runs / sublists of the same length in a row.*/
		cnt = 1;
		while (!stroll_dlist_empty(&subs[cnt])) {
			stroll_dlist_merge_presort(&subs[cnt],
			                           &subs[cnt - 1],
			                           compare,
			                           data);
			cnt++;
			stroll_dlist_assert_intern(cnt <
			                           STROLL_DLIST_MSORT_SUBS_MAX);
		}

		stroll_dlist_embed_after(&subs[cnt],
		                         stroll_dlist_next(&subs[cnt - 1]),
		                         stroll_dlist_prev(&subs[cnt - 1]));
		stroll_dlist_init(&subs[cnt - 1]);

		nr = stroll_max(nr, cnt);
	} while (!stroll_dlist_empty(head));

	/*
	 * Finally merge remaining runs into a single sorted list. To preserve
	 * stability, iterate over the sublists array in reverse order since
	 * earlier runs were "pushed" in the upper areas of the array.
	 */
	stroll_dlist_embed_after(head,
	                         stroll_dlist_next(&subs[nr]),
	                         stroll_dlist_prev(&subs[nr]));
	while (nr--)
		if (!stroll_dlist_empty(&subs[nr]))
			stroll_dlist_merge_presort(head,
			                           &subs[nr],
			                           compare,
			                           data);
}

#endif /* defined(CONFIG_STROLL_DLIST_MERGE_SORT) */
