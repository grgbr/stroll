#include "stroll/dlist.h"

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_dlist_assert_intern(_expr) \
	stroll_assert("stroll:dlist", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_dlist_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

void
stroll_dlist_splice(struct stroll_dlist_node * __restrict at,
                    struct stroll_dlist_node *            first,
                    struct stroll_dlist_node *            last)
{
	stroll_dlist_assert_api(at);
	stroll_dlist_assert_api(first);
	stroll_dlist_assert_api(last);
	stroll_dlist_assert_api(at != first);
	stroll_dlist_assert_api(at != last);

	stroll_dlist_withdraw(first, last);
	stroll_dlist_embed(at, first, last);
}

/******************************************************************************
 * Doubly linked list bubble sorting
 ******************************************************************************/

#if defined(CONFIG_STROLL_DLIST_BUBBLE_SORT)

/* Implementation is always stable */
void
stroll_dlist_bubble_sort(struct stroll_dlist_node * __restrict head,
                         stroll_dlist_cmp_fn *                 compare,
                         void *                                data)
{
	stroll_dlist_assert_api(!stroll_dlist_empty(head));
	stroll_dlist_assert_api(compare);

	struct stroll_dlist_node * order = head;
	struct stroll_dlist_node * swap;

	do {
		struct stroll_dlist_node * curr = head;

		swap = NULL;

		while (true) {
			struct stroll_dlist_node * next;

			/*
			 * Find the next swap location by progressing along the
			 * list untill end of list.
			 */
			do {
				curr = stroll_dlist_next(curr);
				next = stroll_dlist_next(curr);
				if (compare(curr, next, data) > 0)
					/* Swap required. */
					break;
			} while (next != order);

			if (next == order) {
				/* End of current pass. */
				order = curr;
				break;
			}

			/* Extract out of order element. */
			stroll_dlist_remove(curr);

			swap = curr;
			curr = next;
			do {
				curr = stroll_dlist_next(curr);
				if (curr == order)
					/* End of sublist. */
					break;

				if (compare(swap, curr, data) <= 0)
					/* Swap location found. */
					break;
			} while (true);

			stroll_dlist_insert(curr, swap);

			if (curr == order) {
				/* End of current pass. */
				order = swap;
				break;
			}

			curr = swap;
		}
	} while (swap);
}

#endif /* defined(CONFIG_STROLL_DLIST_BUBBLE_SORT) */
