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

#endif /* defined(CONFIG_STROLL_SLIST_SELECT_SORT) */
