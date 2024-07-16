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

	do {
		stroll_dlist_assert_intern(!stroll_dlist_empty(sort));

		struct stroll_dlist_node * curr = stroll_dlist_next(head);
		struct stroll_dlist_node * swap = NULL;

		/* For each node in the list... */
		do {
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

			if (next == sort)
				/*
				 * No more out-of-order node: current pass is
				 * over.
				 */
				break;

			/* Remove current out-of-order node. */
			stroll_dlist_remove(curr);
			/* Keep a pointer to it for later insertion. */
			swap = curr;

			/* Find in order location for out-of-order node. */
			while (true) {
				curr = stroll_dlist_next(curr);
				if (curr == sort)
					break;

				if (compare(curr, swap, data) >= 0)
					break;
			}

			/* Insert it in order. */
			stroll_dlist_insert(curr, swap);
		} while (curr != sort);

		/*
		 * `swap' now points to the last node inserted in order for the
		 * current pass, i.e., points to the first node of the final
		 * sorted list.
		 */
		sort = swap;

		/* Do this as long as current pass swapped at least one node. */
	} while (sort);
}

#endif /* defined(CONFIG_STROLL_DLIST_BUBBLE_SORT) */
