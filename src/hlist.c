#include "stroll/hlist.h"

void
stroll_hlist_init_buckets(struct stroll_hlist * __restrict buckets,
                          unsigned int                     bits)
{
	stroll_hlist_assert_api(buckets);
	stroll_hlist_assert_api(bits <= 32);

	unsigned int b;

	for (b = 0; b < (1U << bits); b++)
		stroll_hlist_init(&buckets[b]);
}

struct stroll_hlist *
stroll_hlist_create_buckets(unsigned int bits)
{
	stroll_hlist_assert_api(bits <= 32);

	struct stroll_hlist * bucks;

	bucks = malloc((1U << bits) * sizeof(*bucks));
	if (!bucks)
		return NULL;

	stroll_hlist_init_buckets(bucks, bits);

	return bucks;
}
