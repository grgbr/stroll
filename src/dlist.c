#include "stroll/dlist.h"

void
stroll_dlist_splice_before(struct stroll_dlist_node * __restrict at,
                           struct stroll_dlist_node *            first,
                           struct stroll_dlist_node *            last)
{
	stroll_dlist_assert_api(at);
	stroll_dlist_assert_api(first);
	stroll_dlist_assert_api(last);
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
