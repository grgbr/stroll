#include "ptest.h"
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>

typedef int
        (strollpt_sort_validate_fn)(const unsigned int * __restrict,
                                    unsigned int,
                                    size_t)
	__stroll_nonull(1);

typedef int
        (strollpt_sort_measure_fn)(const unsigned int * __restrict,
                                   unsigned int,
                                   size_t,
                                   unsigned long long * __restrict)
	__stroll_nonull(1, 4);

struct strollpt_sort_algo {
	const char *                name;
	strollpt_sort_validate_fn * validate;
	strollpt_sort_measure_fn *  measure;
};

#if defined(CONFIG_STROLL_ARRAY)

#include "stroll/array.h"

typedef void
        (strollpt_sort_array_fn)(void * __restrict,
                                 unsigned int,
                                 size_t,
                                 stroll_array_cmp_fn *)
	__stroll_nonull(1, 4);

struct strollpt_array_elem {
	unsigned int id;
	const char   data[0];
};

static struct strollpt_array_elem *
strollpt_array_create(const unsigned int * __restrict elements,
                      unsigned int                    nr,
                      size_t                          size)
{
	assert(elements);
	assert(nr);
	assert(size >= sizeof(elements[0]));
	assert(!(size % sizeof(elements[0])));

	struct strollpt_array_elem * elms;
	struct strollpt_array_elem * ptr;
	unsigned int                 n;

	elms = malloc(nr * size);
	if (!elms)
		return NULL;

	for (n = 0, ptr = elms;
	     n < nr;
	     n++, ptr = (struct strollpt_array_elem *)((char *)ptr + size))
		ptr->id = elements[n];

	return elms;
}

static int
strollpt_sort_validate_array(const unsigned int * __restrict elements,
                             unsigned int                    nr,
                             size_t                          size,
                             strollpt_sort_array_fn *        sort)
{
	assert(elements);
	assert(nr);
	assert(size);
	assert(size <= UINT_MAX);
	assert(sort);

	struct strollpt_array_elem * tmp;
	unsigned int                 n;
	int                          ret = EXIT_FAILURE;

	if (size % sizeof_member(struct strollpt_array_elem, id)) {
		strollpt_err("invalid data element size %zu specified: "
		             "multiple of %zu expected.\n",
		             size,
		             sizeof_member(struct strollpt_array_elem, id));
		return EXIT_FAILURE;
	}

	tmp = strollpt_array_create(elements, nr, size);
	if (!tmp)
		return EXIT_FAILURE;

	sort(tmp, nr, size, strollpt_array_compare_min);

	for (n = 1; n < nr; n++) {
		const struct strollpt_array_elem * curr;
		const struct strollpt_array_elem * prev;

		prev = (const struct strollpt_array_elem *)
		       (&((const char *)tmp)[(n - 1) * size]);
		curr = (const struct strollpt_array_elem *)
		       (&((const char *)tmp)[n * size]);

		if (prev->id > curr->id) {
			strollpt_err("Bogus sorting scheme.\n");
			goto free;
		}
	}

	ret = EXIT_SUCCESS;

free:
	free(tmp);

	return ret;
}

static int
strollpt_sort_measure_array(const unsigned int * __restrict elements,
                            unsigned int                    nr,
                            size_t                          size,
                            unsigned long long * __restrict nsecs,
                            strollpt_sort_array_fn *        sort)
{
	struct timespec              start, elapse;
	struct strollpt_array_elem * tmp;

	tmp = strollpt_array_create(elements, nr, size);
	if (!tmp)
		return EXIT_FAILURE;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	sort(tmp, nr, size, strollpt_array_compare_min);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	free(tmp);

	return EXIT_SUCCESS;
}

/* Glibc's quick sorting */
static inline void
strollpt_sort_array_qsort(void * __restrict     array,
                          unsigned int          nr,
                          size_t                size,
                          stroll_array_cmp_fn * compare)
{
	qsort_r(array, (unsigned int)nr, size, compare, NULL);
}

static int
strollpt_sort_validate_array_qsort(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size)
{
	return strollpt_sort_validate_array(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_array_qsort);
}

static int
strollpt_sort_measure_array_qsort(const unsigned int * __restrict elements,
                                  unsigned int                    nr,
                                  size_t                          size,
                                  unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_array(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_array_qsort);
}

#endif /* defined(CONFIG_STROLL_ARRAY) */

#if defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT)

static inline void
strollpt_sort_array_bubble(void * __restrict     array,
                           unsigned int          nr,
                           size_t                size,
                           stroll_array_cmp_fn * compare)
{
	stroll_array_bubble_sort(array, nr, size, compare, NULL);
}

static int
strollpt_sort_validate_array_bubble(const unsigned int * __restrict elements,
                                    unsigned int                    nr,
                                    size_t                          size)
{
	return strollpt_sort_validate_array(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_array_bubble);
}

static int
strollpt_sort_measure_array_bubble(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size,
                                   unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_array(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_array_bubble);
}

#endif /* defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT) */

#if defined(CONFIG_STROLL_ARRAY_SELECT_SORT)

static inline void
strollpt_sort_array_select(void * __restrict     array,
                           unsigned int          nr,
                           size_t                size,
                           stroll_array_cmp_fn * compare)
{
	stroll_array_select_sort(array, nr, size, compare, NULL);
}

static int
strollpt_sort_validate_array_select(const unsigned int * __restrict elements,
                                    unsigned int                    nr,
                                    size_t                          size)
{
	return strollpt_sort_validate_array(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_array_select);
}

static int
strollpt_sort_measure_array_select(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size,
                                   unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_array(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_array_select);
}

#endif /* defined(CONFIG_STROLL_ARRAY_SELECT_SORT) */

#if defined(CONFIG_STROLL_ARRAY_INSERT_SORT)

static inline void
strollpt_sort_array_insert(void * __restrict     array,
                           unsigned int          nr,
                           size_t                size,
                           stroll_array_cmp_fn * compare)
{
	stroll_array_insert_sort(array, nr, size, compare, NULL);
}

static int
strollpt_sort_validate_array_insert(const unsigned int * __restrict elements,
                                    unsigned int                    nr,
                                    size_t                          size)
{
	return strollpt_sort_validate_array(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_array_insert);
}

static int
strollpt_sort_measure_array_insert(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size,
                                   unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_array(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_array_insert);
}

#endif /* defined(CONFIG_STROLL_ARRAY_INSERT_SORT) */

#if defined(CONFIG_STROLL_ARRAY_QUICK_SORT)

static inline void
strollpt_sort_array_quick(void * __restrict     array,
                          unsigned int          nr,
                          size_t                size,
                          stroll_array_cmp_fn * compare)
{
	stroll_array_quick_sort(array, nr, size, compare, NULL);
}

static int
strollpt_sort_validate_array_quick(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size)
{
	return strollpt_sort_validate_array(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_array_quick);
}

static int
strollpt_sort_measure_array_quick(const unsigned int * __restrict elements,
                                  unsigned int                    nr,
                                  size_t                          size,
                                  unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_array(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_array_quick);
}

#endif /* defined(CONFIG_STROLL_ARRAY_QUICK_SORT) */

#if defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT)

static inline void
strollpt_sort_array_3wquick(void * __restrict     array,
                            unsigned int          nr,
                            size_t                size,
                            stroll_array_cmp_fn * compare)
{
	stroll_array_3wquick_sort(array, nr, size, compare, NULL);
}

static int
strollpt_sort_validate_array_3wquick(const unsigned int * __restrict elements,
                                     unsigned int                    nr,
                                     size_t                          size)
{
	return strollpt_sort_validate_array(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_array_3wquick);
}

static int
strollpt_sort_measure_array_3wquick(const unsigned int * __restrict elements,
                                    unsigned int                    nr,
                                    size_t                          size,
                                    unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_array(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_array_3wquick);
}

#endif /* defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT) */

#if defined(CONFIG_STROLL_ARRAY_MERGE_SORT)

static inline void
strollpt_sort_array_merge(void * __restrict     array,
                          unsigned int          nr,
                          size_t                size,
                          stroll_array_cmp_fn * compare)
{
	if (stroll_array_merge_sort(array, nr, size, compare, NULL))
		exit(1);
}

static int
strollpt_sort_validate_array_merge(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size)
{
	return strollpt_sort_validate_array(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_array_merge);
}

static int
strollpt_sort_measure_array_merge(const unsigned int * __restrict elements,
                                  unsigned int                    nr,
                                  size_t                          size,
                                  unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_array(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_array_merge);
}

#endif /* defined(CONFIG_STROLL_ARRAY_MERGE_SORT) */

#if defined(CONFIG_STROLL_ARRAY_FBHEAP_SORT)

static inline void
strollpt_sort_array_fbheap(void * __restrict     array,
                           unsigned int          nr,
                           size_t                size,
                           stroll_array_cmp_fn * compare)
{
	stroll_array_fbheap_sort(array, nr, size, compare, NULL);
}

static int
strollpt_sort_validate_array_fbheap(const unsigned int * __restrict elements,
                                    unsigned int                    nr,
                                    size_t                          size)
{
	return strollpt_sort_validate_array(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_array_fbheap);
}

static int
strollpt_sort_measure_array_fbheap(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size,
                                   unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_array(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_array_fbheap);
}

#endif /* defined(CONFIG_STROLL_ARRAY_FBHEAP_SORT) */

#if defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT)

static inline void
strollpt_sort_array_fwheap(void * __restrict     array,
                           unsigned int          nr,
                           size_t                size,
                           stroll_array_cmp_fn * compare)
{
	if (stroll_array_fwheap_sort(array, nr, size, compare, NULL))
		exit(1);
}

static int
strollpt_sort_validate_array_fwheap(const unsigned int * __restrict elements,
                                    unsigned int                    nr,
                                    size_t                          size)
{
	return strollpt_sort_validate_array(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_array_fwheap);
}

static int
strollpt_sort_measure_array_fwheap(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size,
                                   unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_array(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_array_fwheap);
}

#endif /* defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT) */

#if defined(CONFIG_STROLL_SLIST)

#include "stroll/slist.h"

typedef void
        (strollpt_sort_slist_fn)(struct stroll_slist * __restrict,
                                 stroll_slist_cmp_fn *)
	__stroll_nonull(1, 2);

struct strollpt_slist_node {
	struct stroll_slist_node super;
	unsigned int             id;
	char                     data[0];
};

static inline int
strollpt_slist_compare_min(
	const struct stroll_slist_node * __restrict a,
	const struct stroll_slist_node * __restrict b,
	void *                                      data __unused)
{
	const struct strollpt_slist_node * _a =
		stroll_slist_entry(a, const struct strollpt_slist_node, super);
	const struct strollpt_slist_node * _b =
		stroll_slist_entry(b, const struct strollpt_slist_node, super);

	return (_a->id > _b->id) - (_a->id < _b->id);
}

static void
strollpt_sort_destroy_slist(struct stroll_slist * __restrict list)
{
	assert(list);

	while (!stroll_slist_empty(list)) {
		struct strollpt_slist_node * node;

		node = stroll_slist_entry(stroll_slist_dqueue_front(list),
		                          struct strollpt_slist_node,
		                          super);
		free(node);
	}

	free(list);
}

static struct stroll_slist *
strollpt_sort_create_slist(const unsigned int * __restrict elements,
                           unsigned int                    nr,
                           size_t                          size)
{
	assert(elements);
	assert(nr);
	assert(size);
	assert(size >= sizeof_member(struct strollpt_slist_node, id));

	struct stroll_slist * list;
	unsigned int          n;

	list = malloc(sizeof(*list));
	if (!list)
		return NULL;
	stroll_slist_init(list);

	for (n = 0; n < nr; n++) {
		struct strollpt_slist_node * node;

		node = malloc(sizeof(node->super) + size);
		if (!node)
			goto destroy;

		node->id = elements[n];
		stroll_slist_nqueue_back(list, &node->super);
	}

	return list;

destroy:
	strollpt_sort_destroy_slist(list);

	return NULL;
}

static int
strollpt_sort_validate_slist(const unsigned int * __restrict elements,
                             unsigned int                    nr,
                             size_t                          size,
                             strollpt_sort_slist_fn *        sort)
{
	assert(elements);
	assert(nr);
	assert(sort);

	struct stroll_slist *              list;
	const struct strollpt_slist_node * prev = NULL;
	const struct strollpt_slist_node * curr;
	unsigned int                       cnt = 0;
	int                                ret = EXIT_FAILURE;

	if (size % sizeof_member(struct strollpt_slist_node, id)) {
		strollpt_err("invalid data element size %zu specified: "
		             "multiple of %zu expected.\n",
		             size,
		             sizeof_member(struct strollpt_slist_node, id));
		return EXIT_FAILURE;
	}

	list = strollpt_sort_create_slist(elements, nr, size);
	if (!list)
		return EXIT_FAILURE;

	sort(list, strollpt_slist_compare_min);

	stroll_slist_foreach_entry(list, curr, super) {
		cnt++;
		if (prev) {
			if (prev->id > curr->id) {
				strollpt_err("bogus sorting scheme: "
				             "node out of order.\n");
				goto free;
			}
		}
		prev = curr;
	}

	if (cnt != nr) {
		strollpt_err("bogus sorting scheme: "
		             "unexpected number of nodes.\n");
		goto free;
	}

	ret = EXIT_SUCCESS;

free:
	strollpt_sort_destroy_slist(list);

	return ret;
}

static int
strollpt_sort_measure_slist(const unsigned int * __restrict elements,
                            unsigned int                    nr,
                            size_t                          size,
                            unsigned long long * __restrict nsecs,
                            strollpt_sort_slist_fn *        sort)
{
	struct timespec start, elapse;
	struct stroll_slist *  list;

	list = strollpt_sort_create_slist(elements, nr, size);
	if (!list)
		return EXIT_FAILURE;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	sort(list, strollpt_slist_compare_min);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	strollpt_sort_destroy_slist(list);

	return EXIT_SUCCESS;
}

#endif /* defined(CONFIG_STROLL_SLIST) */

#if defined(CONFIG_STROLL_SLIST_BUBBLE_SORT)

static void
strollpt_sort_slist_bubble(struct stroll_slist * __restrict list,
                           stroll_slist_cmp_fn *            compare)
{
	stroll_slist_bubble_sort(list, compare, NULL);
}

static int
strollpt_sort_validate_slist_bubble(const unsigned int * __restrict elements,
                                    unsigned int                    nr,
                                    size_t                          size)
{
	return strollpt_sort_validate_slist(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_slist_bubble);
}

static int
strollpt_sort_measure_slist_bubble(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size,
                                   unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_slist(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_slist_bubble);
}

#endif /* defined(CONFIG_STROLL_SLIST_BUBBLE_SORT) */

#if defined(CONFIG_STROLL_SLIST_SELECT_SORT)

static void
strollpt_sort_slist_select(struct stroll_slist * __restrict list,
                           stroll_slist_cmp_fn *            compare)
{
	stroll_slist_select_sort(list, compare, NULL);
}

static int
strollpt_sort_validate_slist_select(const unsigned int * __restrict elements,
                                    unsigned int                    nr,
                                    size_t                          size)
{
	return strollpt_sort_validate_slist(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_slist_select);
}

static int
strollpt_sort_measure_slist_select(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size,
                                   unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_slist(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_slist_select);
}

#endif /* defined(CONFIG_STROLL_SLIST_SELECT_SORT) */

#if defined(CONFIG_STROLL_SLIST_INSERT_SORT)

static void
strollpt_sort_slist_insert(struct stroll_slist * __restrict list,
                           stroll_slist_cmp_fn *            compare)
{
	stroll_slist_insert_sort(list, compare, NULL);
}

static int
strollpt_sort_validate_slist_insert(const unsigned int *__restrict elements,
                                    unsigned int                   nr,
                                    size_t                         size)
{
	return strollpt_sort_validate_slist(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_slist_insert);
}

static int
strollpt_sort_measure_slist_insert(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size,
                                   unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_slist(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_slist_insert);
}

#endif /* defined(CONFIG_STROLL_SLIST_INSERT_SORT) */

#if defined(CONFIG_STROLL_SLIST_MERGE_SORT)

static void
strollpt_sort_slist_merge(struct stroll_slist * __restrict list,
                          stroll_slist_cmp_fn *            compare)
{
	stroll_slist_merge_sort(list, compare, NULL);
}

static int
strollpt_sort_validate_slist_merge(const unsigned int *__restrict elements,
                                   unsigned int                   nr,
                                   size_t                         size)
{
	return strollpt_sort_validate_slist(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_slist_merge);
}

static int
strollpt_sort_measure_slist_merge(const unsigned int * __restrict elements,
                                  unsigned int                    nr,
                                  size_t                          size,
                                  unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_slist(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_slist_merge);
}

#endif /* defined(CONFIG_STROLL_SLIST_MERGE_SORT) */

#if defined(CONFIG_STROLL_DLIST)

#include "stroll/dlist.h"

typedef void
        (strollpt_sort_dlist_fn)(struct stroll_dlist_node * __restrict,
                                 stroll_dlist_cmp_fn *)
	__stroll_nonull(1, 2);

struct strollpt_dlist_node {
	struct stroll_dlist_node super;
	unsigned int             id;
	char                     data[0];
};

static inline int
strollpt_dlist_compare_min(
	const struct stroll_dlist_node * __restrict a,
	const struct stroll_dlist_node * __restrict b,
	void *                                      data __unused)
{
	const struct strollpt_dlist_node * _a =
		stroll_dlist_entry(a, const struct strollpt_dlist_node, super);
	const struct strollpt_dlist_node * _b =
		stroll_dlist_entry(b, const struct strollpt_dlist_node, super);

	return (_a->id > _b->id) - (_a->id < _b->id);
}

static void
strollpt_sort_destroy_dlist(struct stroll_dlist_node * __restrict list)
{
	assert(list);

	while (!stroll_dlist_empty(list)) {
		struct strollpt_dlist_node * node;

		node = stroll_dlist_entry(stroll_dlist_dqueue_front(list),
		                          struct strollpt_dlist_node,
		                          super);
		free(node);
	}

	free(list);
}

static struct stroll_dlist_node *
strollpt_sort_create_dlist(const unsigned int * __restrict elements,
                           unsigned int                    nr,
                           size_t                          size)
{
	assert(elements);
	assert(nr);
	assert(size);
	assert(size >= sizeof_member(struct strollpt_dlist_node, id));

	struct stroll_dlist_node * list;
	unsigned int               n;

	list = malloc(sizeof(*list));
	if (!list)
		return NULL;
	stroll_dlist_init(list);

	for (n = 0; n < nr; n++) {
		struct strollpt_dlist_node * node;

		node = malloc(sizeof(node->super) + size);
		if (!node)
			goto destroy;

		node->id = elements[n];
		stroll_dlist_nqueue_back(list, &node->super);
	}

	return list;

destroy:
	strollpt_sort_destroy_dlist(list);

	return NULL;
}

static int
strollpt_sort_validate_dlist(const unsigned int * __restrict elements,
                             unsigned int                    nr,
                             size_t                          size,
                             strollpt_sort_dlist_fn *        sort)
{
	assert(elements);
	assert(nr);
	assert(sort);

	struct stroll_dlist_node *         list;
	const struct strollpt_dlist_node * prev = NULL;
	const struct strollpt_dlist_node * curr;
	unsigned int                       cnt = 0;
	int                                ret = EXIT_FAILURE;

	if (size % sizeof_member(struct strollpt_dlist_node, id)) {
		strollpt_err("invalid data element size %zu specified: "
		             "multiple of %zu expected.\n",
		             size,
		             sizeof_member(struct strollpt_dlist_node, id));
		return EXIT_FAILURE;
	}

	list = strollpt_sort_create_dlist(elements, nr, size);
	if (!list)
		return EXIT_FAILURE;

	sort(list, strollpt_dlist_compare_min);

	stroll_dlist_foreach_entry(list, curr, super) {
		cnt++;
		if (prev) {
			if (prev->id > curr->id) {
				strollpt_err("bogus sorting scheme: "
				             "node out of order.\n");
				goto free;
			}
		}
		prev = curr;
	}

	if (cnt != nr) {
		strollpt_err("bogus sorting scheme: "
		             "unexpected number of nodes.\n");
		goto free;
	}

	ret = EXIT_SUCCESS;

free:
	strollpt_sort_destroy_dlist(list);

	return ret;
}

static int
strollpt_sort_measure_dlist(const unsigned int * __restrict elements,
                            unsigned int                    nr,
                            size_t                          size,
                            unsigned long long * __restrict nsecs,
                            strollpt_sort_dlist_fn *        sort)
{
	struct timespec            start, elapse;
	struct stroll_dlist_node * list;

	list = strollpt_sort_create_dlist(elements, nr, size);
	if (!list)
		return EXIT_FAILURE;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	sort(list, strollpt_dlist_compare_min);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	strollpt_sort_destroy_dlist(list);

	return EXIT_SUCCESS;
}

#endif /* defined(CONFIG_STROLL_DLIST) */

#if defined(CONFIG_STROLL_DLIST_BUBBLE_SORT)

static void
strollpt_sort_dlist_bubble(struct stroll_dlist_node * __restrict list,
                           stroll_dlist_cmp_fn *                 compare)
{
	stroll_dlist_bubble_sort(list, compare, NULL);
}

static int
strollpt_sort_validate_dlist_bubble(const unsigned int * __restrict elements,
                                    unsigned int                    nr,
                                    size_t                          size)
{
	return strollpt_sort_validate_dlist(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_dlist_bubble);
}

static int
strollpt_sort_measure_dlist_bubble(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size,
                                   unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_dlist(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_dlist_bubble);
}

#endif /* defined(CONFIG_STROLL_DLIST_BUBBLE_SORT) */

#if defined(CONFIG_STROLL_DLIST_SELECT_SORT)

static void
strollpt_sort_dlist_select(struct stroll_dlist_node * __restrict list,
                           stroll_dlist_cmp_fn *                 compare)
{
	stroll_dlist_select_sort(list, compare, NULL);
}

static int
strollpt_sort_validate_dlist_select(const unsigned int * __restrict elements,
                                    unsigned int                    nr,
                                    size_t                          size)
{
	return strollpt_sort_validate_dlist(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_dlist_select);
}

static int
strollpt_sort_measure_dlist_select(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size,
                                   unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_dlist(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_dlist_select);
}

#endif /* defined(CONFIG_STROLL_DLIST_SELECT_SORT) */

#if defined(CONFIG_STROLL_DLIST_INSERT_SORT)

static void
strollpt_sort_dlist_insert(struct stroll_dlist_node * __restrict list,
                           stroll_dlist_cmp_fn *                 compare)
{
	stroll_dlist_insert_sort(list, compare, NULL);
}

static int
strollpt_sort_validate_dlist_insert(const unsigned int * __restrict elements,
                                    unsigned int                    nr,
                                    size_t                          size)
{
	return strollpt_sort_validate_dlist(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_dlist_insert);
}

static int
strollpt_sort_measure_dlist_insert(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size,
                                   unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_dlist(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_dlist_insert);
}

#endif /* defined(CONFIG_STROLL_DLIST_INSERT_SORT) */

#if defined(CONFIG_STROLL_DLIST_MERGE_SORT)

static void
strollpt_sort_dlist_merge(struct stroll_dlist_node * __restrict list,
                          stroll_dlist_cmp_fn *                 compare)
{
	stroll_dlist_merge_sort(list, compare, NULL);
}

static int
strollpt_sort_validate_dlist_merge(const unsigned int * __restrict elements,
                                   unsigned int                    nr,
                                   size_t                          size)
{
	return strollpt_sort_validate_dlist(elements,
	                                    nr,
	                                    size,
	                                    strollpt_sort_dlist_merge);
}

static int
strollpt_sort_measure_dlist_merge(const unsigned int * __restrict elements,
                                  unsigned int                    nr,
                                  size_t                          size,
                                  unsigned long long * __restrict nsecs)
{
	return strollpt_sort_measure_dlist(elements,
	                                   nr,
	                                   size,
	                                   nsecs,
	                                   strollpt_sort_dlist_merge);
}

#endif /* defined(CONFIG_STROLL_DLIST_MERGE_SORT) */

static const struct strollpt_sort_algo strollpt_sort_algos[] = {
#if defined(CONFIG_STROLL_ARRAY)
	{
		.name     = "array_qsort",
		.validate = strollpt_sort_validate_array_qsort,
		.measure  = strollpt_sort_measure_array_qsort
	},
#endif /* defined(CONFIG_STROLL_ARRAY) */
#if defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT)
	{
		.name     = "array_bubble",
		.validate = strollpt_sort_validate_array_bubble,
		.measure  = strollpt_sort_measure_array_bubble
	},
#endif /* defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT) */
#if defined(CONFIG_STROLL_ARRAY_SELECT_SORT)
	{
		.name     = "array_select",
		.validate = strollpt_sort_validate_array_select,
		.measure  = strollpt_sort_measure_array_select
	},
#endif /* defined(CONFIG_STROLL_ARRAY_SELECT_SORT) */
#if defined(CONFIG_STROLL_ARRAY_INSERT_SORT)
	{
		.name     = "array_insert",
		.validate = strollpt_sort_validate_array_insert,
		.measure  = strollpt_sort_measure_array_insert
	},
#endif /* defined(CONFIG_STROLL_ARRAY_INSERT_SORT) */
#if defined(CONFIG_STROLL_ARRAY_QUICK_SORT)
	{
		.name     = "array_quick",
		.validate = strollpt_sort_validate_array_quick,
		.measure  = strollpt_sort_measure_array_quick
	},
#endif /* defined(CONFIG_STROLL_ARRAY_QUICK_SORT) */
#if defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT)
	{
		.name     = "array_3wquick",
		.validate = strollpt_sort_validate_array_3wquick,
		.measure  = strollpt_sort_measure_array_3wquick
	},
#endif /* defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT) */
#if defined(CONFIG_STROLL_ARRAY_MERGE_SORT)
	{
		.name     = "array_merge",
		.validate = strollpt_sort_validate_array_merge,
		.measure  = strollpt_sort_measure_array_merge
	},
#endif /* defined(CONFIG_STROLL_ARRAY_MERGE_SORT) */
#if defined(CONFIG_STROLL_ARRAY_FBHEAP_SORT)
	{
		.name     = "array_fbheap",
		.validate = strollpt_sort_validate_array_fbheap,
		.measure  = strollpt_sort_measure_array_fbheap
	},
#endif /* defined(CONFIG_STROLL_ARRAY_FBHEAP_SORT) */
#if defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT)
	{
		.name     = "array_fwheap",
		.validate = strollpt_sort_validate_array_fwheap,
		.measure  = strollpt_sort_measure_array_fwheap
	},
#endif /* defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT) */
#if defined(CONFIG_STROLL_SLIST_BUBBLE_SORT)
	{
		.name     = "slist_bubble",
		.validate = strollpt_sort_validate_slist_bubble,
		.measure  = strollpt_sort_measure_slist_bubble
	},
#endif /* defined(CONFIG_STROLL_SLIST_BUBBLE_SORT) */
#if defined(CONFIG_STROLL_SLIST_SELECT_SORT)
	{
		.name     = "slist_select",
		.validate = strollpt_sort_validate_slist_select,
		.measure  = strollpt_sort_measure_slist_select
	},
#endif /* defined(CONFIG_STROLL_SLIST_SELECT_SORT) */
#if defined(CONFIG_STROLL_SLIST_INSERT_SORT)
	{
		.name     = "slist_insert",
		.validate = strollpt_sort_validate_slist_insert,
		.measure  = strollpt_sort_measure_slist_insert
	},
#endif /* defined(CONFIG_STROLL_SLIST_INSERT_SORT) */
#if defined(CONFIG_STROLL_SLIST_MERGE_SORT)
	{
		.name     = "slist_merge",
		.validate = strollpt_sort_validate_slist_merge,
		.measure  = strollpt_sort_measure_slist_merge
	},
#endif /* defined(CONFIG_STROLL_SLIST_MERGE_SORT) */
#if defined(CONFIG_STROLL_DLIST_BUBBLE_SORT)
	{
		.name     = "dlist_bubble",
		.validate = strollpt_sort_validate_dlist_bubble,
		.measure  = strollpt_sort_measure_dlist_bubble
	},
#endif /* defined(CONFIG_STROLL_DLIST_BUBBLE_SORT) */
#if defined(CONFIG_STROLL_DLIST_SELECT_SORT)
	{
		.name     = "dlist_select",
		.validate = strollpt_sort_validate_dlist_select,
		.measure  = strollpt_sort_measure_dlist_select
	},
#endif /* defined(CONFIG_STROLL_DLIST_SELECT_SORT) */
#if defined(CONFIG_STROLL_DLIST_INSERT_SORT)
	{
		.name     = "dlist_insert",
		.validate = strollpt_sort_validate_dlist_insert,
		.measure  = strollpt_sort_measure_dlist_insert
	},
#endif /* defined(CONFIG_STROLL_DLIST_INSERT_SORT) */
#if defined(CONFIG_STROLL_DLIST_MERGE_SORT)
	{
		.name     = "dlist_merge",
		.validate = strollpt_sort_validate_dlist_merge,
		.measure  = strollpt_sort_measure_dlist_merge
	},
#endif /* defined(CONFIG_STROLL_DLIST_MERGE_SORT) */
};

static int
strollpt_sort_parse_algo(const char * __restrict                       name,
                         const struct strollpt_sort_algo ** __restrict algo)
{
	unsigned int a;

	for (a = 0; a < stroll_array_nr(strollpt_sort_algos); a++) {
		if (!strcmp(name, strollpt_sort_algos[a].name)) {
			*algo = &strollpt_sort_algos[a];
			return EXIT_SUCCESS;
		}
	}

	strollpt_err("invalid '%s' sort algorithm.\n", name);

	return EXIT_FAILURE;
}

static void
strollpt_sort_usage(FILE * __restrict stdio)
{
	fprintf(stdio,
	        "Usage: %s [OPTIONS] FILE ALGORITHM SIZE LOOPS\n"
	        "where OPTIONS:\n"
	        "    -p|--prio  PRIORITY\n"
	        "    -h|--help\n",
	        program_invocation_short_name);
}

int main(int argc, char *argv[])
{
	const struct strollpt_sort_algo * algo;
	size_t                            dsize;
	unsigned int                      loops;
	int                               prio = 0;
	struct strollpt_data              data;
	unsigned int *                    elms;
	unsigned long long *              nsecs;
	unsigned int                      l;
	struct strollpt_stats             stats;
	int                               ret = EXIT_FAILURE;

	while (true) {
		int                        opt;
		static const struct option lopts[] = {
			{"help",    0, NULL, 'h'},
			{"prio",    1, NULL, 'p'},
			{0,         0, 0,    0}
		};

		opt = getopt_long(argc, argv, "hp:", lopts, NULL);
		if (opt < 0)
			/* No more options: go parsing positional arguments. */
			break;

		switch (opt) {
		case 'p': /* priority */
			if (strollpt_parse_sched_prio(optarg, &prio)) {
				strollpt_sort_usage(stderr);
				return EXIT_FAILURE;
			}

			break;

		case 'h': /* Help message. */
			strollpt_sort_usage(stdout);
			exit(EXIT_SUCCESS);

		case '?': /* Unknown option. */
		default:
			strollpt_sort_usage(stderr);
			return EXIT_FAILURE;
		}
	}

	/*
	 * Check positional arguments are properly specified on command
	 * line.
	 */
	argc -= optind;
	if (argc != 4) {
		strollpt_err("invalid number of arguments.\n");
		strollpt_sort_usage(stderr);
		return EXIT_FAILURE;
	}

	if (strollpt_sort_parse_algo(argv[optind + 1], &algo))
		return EXIT_FAILURE;

	if (strollpt_parse_data_size(argv[optind + 2], &dsize))
		return EXIT_FAILURE;

	if (strollpt_parse_loop_nr(argv[optind + 3], &loops))
		return EXIT_FAILURE;

	elms = strollpt_load_data(&data, argv[optind]);
	if (!elms)
		return EXIT_FAILURE;

	if (algo->validate(elms, data.nr, dsize))
		goto free_data;

	nsecs = malloc(loops * sizeof(*nsecs));
	if (!nsecs)
		goto free_data;

	if (strollpt_setup_sched_prio(prio))
		goto free_nsecs;

	for (l = 0; l < loops; l++) {
		if (algo->measure(elms, data.nr, dsize, &nsecs[l]))
			goto free_nsecs;
	}

	if (strollpt_calc_stats(&stats, nsecs, 1, loops))
		goto free_nsecs;

	printf("#Samples:       %u\n"
	       "Order ratio:    %hu\n"
	       "Distinct ratio: %hu\n"
	       "Algorithm:      %s\n"
	       "Data size:      %zu\n"
	       "#Loops:         %u\n"
	       "#Inliers:       %u (%.2lf%%)\n"
	       "Mininum:        %llu nSec\n"
	       "Maximum:        %llu nSec\n"
	       "Deviation:      %llu nSec\n"
	       "Median:         %llu nSec\n"
	       "Mean:           %llu nSec\n",
	       data.nr,
	       data.order,
	       data.singles,
	       algo->name,
	       dsize,
	       loops,
	       stats.count,
	       ((double)stats.count * 100.0) / (double)loops,
	       stats.min,
	       stats.max,
	       (unsigned long long)round(stats.stdev),
	       stats.med,
	       (unsigned long long)round(stats.mean));

	ret = EXIT_SUCCESS;

free_nsecs:
	free(nsecs);
free_data:
	free(elms);

	return ret;
}
