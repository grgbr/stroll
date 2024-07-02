#include "stroll/dlist.h"

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
