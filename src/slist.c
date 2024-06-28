/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/slist.h"
#include "stroll/pow2.h"
#include <errno.h>

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

/* Implementation is always stable */
void
stroll_slist_bubble_sort(struct stroll_slist * __restrict list,
                         stroll_slist_cmp_fn *            compare,
                         void *                           data)
{
	stroll_slist_assert_api(!stroll_slist_empty(list));
	stroll_slist_assert_api(compare);

	struct stroll_slist_node * head = NULL;
	struct stroll_slist_node * swap;

	do {
		struct stroll_slist_node * curr = stroll_slist_head(list);
		struct stroll_slist_node * prev;
		struct stroll_slist_node * nxt;

		swap = NULL;

		while (true) {
			/*
			 * Find the next swap location by progressing along the
			 * list untill end of list.
			 */
			do {
				prev = curr;
				curr = stroll_slist_next(curr);
				nxt = stroll_slist_next(curr);
				if (nxt == head)
					nxt = NULL;

				if (!nxt)
					/* End of sublist. */
					break;

				if (compare(curr, nxt, data) > 0)
					/* Swap required. */
					break;
			} while (true);

			if (!nxt) {
				/* End of current pass. */
				head = curr;
				break;
			}

			/* Extract out of order element. */
			stroll_slist_remove(list, prev, curr);

			swap = curr;
			curr = nxt;
			do {
				prev = curr;
				curr = stroll_slist_next(curr);
				if (curr == head)
					curr = NULL;

				if (!curr)
					/* End of sublist. */
					break;

				if (compare(swap, curr, data) <= 0)
					/* Swap location found. */
					break;
			} while (true);

			stroll_slist_append(list, prev, swap);

			if (!curr) {
				/* End of current pass. */
				head = swap;
				break;
			}

			curr = swap;
		}
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

#if defined(CONFIG_STROLL_SLIST_INSERT_SORT)

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
	while (true) {
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

		if (compare(node, curr, data) < 0)
			break;

		prev = curr;
		curr = stroll_slist_next(curr);
	}

	stroll_slist_append(list, prev, node);
}

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

void
stroll_slist_counted_insert_sort(struct stroll_slist * __restrict result,
                                 struct stroll_slist * __restrict source,
                                 unsigned int                     count,
                                 stroll_slist_cmp_fn *            compare,
                                 void *                           data)
{
	stroll_slist_assert_api(stroll_slist_empty(result));
	stroll_slist_assert_api(!stroll_slist_empty(source));
	stroll_slist_assert_api(compare);

	struct stroll_slist_node * prev = stroll_slist_first(source);
	struct stroll_slist_node * curr = stroll_slist_next(prev);

	while (--count && curr) {
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

#endif /* defined(CONFIG_STROLL_SLIST_INSERT_SORT) */

/******************************************************************************
 * Singly linked list hybrid merge sorting
 ******************************************************************************/

#if defined(CONFIG_STROLL_SLIST_MERGE_SORT)

/*
 * Merge 2 sorted (sub)lists segments.
 *
 * @param result  stroll_slist within which both stroll_slist will be sorted into.
 * @param at      First node of result's segment.
 * @param source  Source stroll_slist to sort within result.
 * @param compare comparison function used to perform in order insertion
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
	stroll_slist_assert_api(!stroll_slist_empty(result));
	stroll_slist_assert_api(at);
	stroll_slist_assert_api(!stroll_slist_empty(source));

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
	 * befor current result's node.
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

/*
 * Iteratively split list and merge presorted sublists.
 *
 * @param list    stroll_slist to sort.
 * @param run_len Primary sorting run length in number of nodes.
 * @param subs_nr Logarithm base 2 of number of merging pass.
 * @param compare Comparison function used to perform in order insertion.
 *
 * @note
 * Merge sorting sublists are allocated onto stack since implementation needs
 * only logarithmic auxiliary space.
 */
static __stroll_nonull(1, 4)
void
stroll_slist_split_merge_sort(struct stroll_slist * __restrict list,
                              unsigned int                     run_len,
                              unsigned int                     subs_nr,
                              stroll_slist_cmp_fn *            compare,
                              void *                           data)
{
	stroll_slist_assert_intern(list);
	stroll_slist_assert_intern(subs_nr);
	stroll_slist_assert_intern(compare);

	unsigned int        cnt;
	struct stroll_slist subs[subs_nr]; /* Space for sublists merging. */

	for (cnt = 0; cnt < subs_nr; cnt++)
		stroll_slist_init(&subs[cnt]);

	subs_nr = 0;
	do {
		/*
		 * Sort an initial run using insertion sort and store the result
		 * in the current sublist.
		 */
		stroll_slist_counted_insert_sort(&subs[0],
		                                 list,
		                                 run_len,
		                                 compare,
		                                 data);

		/* Merge up 2 runs / sublists of the same length in a row.*/
		cnt = 1;
		while (!stroll_slist_empty(&subs[cnt])) {
			stroll_slist_merge_presort(&subs[cnt],
			                           &subs[cnt - 1],
			                           compare,
			                           data);
			cnt++;
		}

		subs[cnt] = subs[cnt - 1];
		stroll_slist_init(&subs[cnt - 1]);

		subs_nr = stroll_max(subs_nr, cnt);
	} while (!stroll_slist_empty(list));

	/*
	 * Finally merge remaining runs into a single sorted list. To preserve
	 * stability, iterate over the sublists array in reverse order since
	 * earlier runs were "pushed" in the upper areas of the array.
	 */
	*list = subs[subs_nr];
	while (subs_nr--)
		if (!stroll_slist_empty(&subs[subs_nr]))
			stroll_slist_merge_presort(list,
			                           &subs[subs_nr],
			                           compare,
			                           data);
}

void
stroll_slist_hybrid_merge_sort(struct stroll_slist * __restrict list,
                               unsigned int                     run_len,
                               unsigned int                     nodes_nr,
                               stroll_slist_cmp_fn *            compare,
                               void *                           data)
{
	stroll_slist_assert_api(!stroll_slist_empty(list));
	stroll_slist_assert_api(run_len);
	stroll_slist_assert_api(nodes_nr);
	stroll_slist_assert_api(compare);

	/* Perform the real merge sort. */
	stroll_slist_split_merge_sort(
		list,
		run_len,
		stroll_pow2_up(stroll_max(nodes_nr / run_len, 2U)) + 2,
		compare,
		data);
}

void
stroll_slist_merge_sort(struct stroll_slist * __restrict list,
                      unsigned int                       nodes_nr,
                      stroll_slist_cmp_fn *              compare,
                      void *                             data)
{
	stroll_slist_assert_api(nodes_nr);

	unsigned int run_len;

	if (nodes_nr <= 4)
		/* Switch to insertion sorting for trivial cases. */
		return stroll_slist_insert_sort(list, compare, data);

	if (nodes_nr <= 16)
		run_len = 4;
	else if (nodes_nr <= 128)
		run_len = 8;
	else if (nodes_nr <= 1024)
		run_len = 16;
	else if (nodes_nr <= (8*1024))
		run_len = 32;
	else if (nodes_nr <= (64*1024))
		run_len = 64;
	else
		run_len = 128;

	stroll_slist_hybrid_merge_sort(list, run_len, nodes_nr, compare, data);
}

#endif /* defined(CONFIG_STROLL_SLIST_MERGE_SORT) */
