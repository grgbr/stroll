#include "stroll/falloc.h"
#include "stroll/pow2.h"
#include <stdlib.h>
#include <errno.h>

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include "stroll/assert.h"

#define stroll_falloc_assert_intern(_expr) \
	stroll_assert("stroll:falloc", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_falloc_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_falloc_assert_alloc_api(_alloc) \
	stroll_falloc_assert_api(_alloc); \
	stroll_falloc_assert_api((_alloc)->chunk_nr); \
	stroll_falloc_assert_api((_alloc)->chunk_cnt <= (_alloc)->chunk_nr); \
	stroll_falloc_assert_api( \
		stroll_aligned((_alloc)->chunk_sz, \
		               sizeof_member(union stroll_alloc_chunk, \
		                             next_free))); \
	stroll_falloc_assert_api((_alloc)->chunk_per_block > 1); \
	stroll_falloc_assert_api( \
		(_alloc)->block_sz == \
		(sizeof(struct stroll_falloc_block) + \
		 ((_alloc)->chunk_per_block * (_alloc)->chunk_sz))); \
	stroll_falloc_assert_api( \
		(_alloc)->block_al == \
		(1UL << stroll_pow2_upul((_alloc)->block_sz)))

#define stroll_falloc_assert_alloc_intern(_alloc) \
	stroll_falloc_assert_intern(_alloc); \
	stroll_falloc_assert_intern((_alloc)->chunk_nr); \
	stroll_falloc_assert_intern((_alloc)->chunk_cnt <= \
	                            (_alloc)->chunk_nr); \
	stroll_falloc_assert_intern( \
		stroll_aligned((_alloc)->chunk_sz, \
		               sizeof_member(union stroll_alloc_chunk, \
		                             next_free))); \
	stroll_falloc_assert_intern((_alloc)->chunk_per_block > 1); \
	stroll_falloc_assert_intern( \
		(_alloc)->block_sz == \
		(sizeof(struct stroll_falloc_block) + \
		 ((_alloc)->chunk_per_block * (_alloc)->chunk_sz))); \
	stroll_falloc_assert_intern( \
		(_alloc)->block_al == \
		(1UL << stroll_pow2_upul((_alloc)->block_sz)))

struct stroll_falloc_block {
	unsigned int               busy_cnt;  /* Count of allocated chunks */
	union stroll_alloc_chunk * next_free; /* Pointer to next free chunk */
	struct stroll_dlist_node   node;
	union stroll_alloc_chunk   chunks[0];
};

#define stroll_falloc_assert_block(_block, _alloc) \
	stroll_falloc_assert_intern(_block); \
	stroll_falloc_assert_alloc_intern(alloc); \
	stroll_falloc_assert_intern( \
		stroll_aligned((unsigned long)(_block), (_alloc)->block_al)); \
	stroll_falloc_assert_intern((_block)->busy_cnt); \
	stroll_falloc_assert_intern((_block)->busy_cnt <= \
	                            (_alloc)->chunk_per_block); \
	stroll_falloc_assert_intern( \
		!(_block)->next_free || \
		((_block)->next_free >= (_block)->chunks)); \
	stroll_falloc_assert_intern( \
		!(_block)->next_free || \
		((const void *)(_block)->next_free < \
		 ((const void *)(_block)->chunks + \
		  ((_alloc)->chunk_per_block * (_alloc)->chunk_sz))))

static __stroll_nonull(1)
       __malloc(stroll_falloc_free, 2)
       __assume_align(sizeof(union stroll_alloc_chunk *))
       __stroll_nothrow
       __warn_result
void *
stroll_falloc_alloc_blockn_chunk(struct stroll_falloc * __restrict alloc)
{
	stroll_falloc_assert_alloc_intern(alloc);

	struct stroll_falloc_block * blk;
	int                          err;

	err = posix_memalign((void **)&blk, alloc->block_al, alloc->block_sz);
	if (err) {
		stroll_falloc_assert_intern(err == ENOMEM);
		errno = err;
		return NULL;
	}

	blk->busy_cnt = 1;
	blk->next_free = NULL;
	stroll_dlist_append(&alloc->blocks, &blk->node);

	return blk->chunks;
}

static __stroll_nonull(1) __stroll_nothrow
void
stroll_falloc_free_block(struct stroll_falloc_block * __restrict block)
{
	stroll_falloc_assert_intern(block);

	stroll_dlist_remove(&block->node);

	free(block);
}

static __stroll_nonull(1, 2)
       __malloc(stroll_falloc_free, 2)
       __assume_align(sizeof(union stroll_alloc_chunk *))
       __stroll_nothrow
       __warn_result
void *
stroll_falloc_next_free_chunk(
	struct stroll_falloc_block * __restrict block,
	const struct stroll_falloc * __restrict alloc)
{
	stroll_falloc_assert_block(block, alloc);
	stroll_falloc_assert_intern(block->busy_cnt < alloc->chunk_per_block);

	void * chunk;

	if (block->next_free) {
		chunk = block->next_free;
		block->next_free = block->next_free->next_free;
	}
	else
		chunk = (void *)block->chunks +
		        (block->busy_cnt * alloc->chunk_sz);

	return chunk;
}

void *
stroll_falloc_alloc(struct stroll_falloc * __restrict alloc)
{
	stroll_falloc_assert_alloc_api(alloc);

	if (alloc->chunk_cnt < alloc->chunk_nr) {
		void * chunk;

		if (!stroll_dlist_empty(&alloc->blocks)) {
			struct stroll_falloc_block * blk;

			blk = stroll_dlist_entry(
				stroll_dlist_next(&alloc->blocks),
				struct stroll_falloc_block,
				node);
			if (blk->busy_cnt < alloc->chunk_per_block) {
				chunk = stroll_falloc_next_free_chunk(blk,
				                                      alloc);

				/*
				 * If block is full, move it to block list tail.
				 */
				if (++blk->busy_cnt == alloc->chunk_per_block)
					stroll_dlist_move_before(&alloc->blocks,
					                         &blk->node);

				alloc->chunk_cnt++;

				return chunk;
			}
		}

		chunk = stroll_falloc_alloc_blockn_chunk(alloc);
		if (chunk) {
			alloc->chunk_cnt++;

			return chunk;
		}
	}

	return NULL;
}

void
stroll_falloc_free(struct stroll_falloc * __restrict alloc,
                   void * __restrict                 chunk)
{
	stroll_falloc_assert_alloc_api(alloc);

	if (chunk) {
		stroll_falloc_assert_api(alloc->chunk_cnt);

		struct stroll_falloc_block * blk;
		union stroll_alloc_chunk *   chnk;

		alloc->chunk_cnt--;

		blk = (struct stroll_falloc_block *)
		      stroll_align_lower((unsigned long)chunk, alloc->block_al);
		chnk = (union stroll_alloc_chunk *)chunk;

		/*
		 * Insert chunk to free at the head of free chunk list.
		 */
		chnk->next_free = blk->next_free;
		blk->next_free = chnk;

		/*
		 * TODO:
		 * - implement an hysteresis logic ??
		 * - an alternate free block list ??
		 */
		blk->busy_cnt--;
		if (blk->busy_cnt)
			/* Block not empty: move it to block list head. */
			stroll_dlist_move_after(&alloc->blocks, &blk->node);
		else
			/* Block is empty: free it. */
			stroll_falloc_free_block(blk);
	}
}

void
stroll_falloc_init(struct stroll_falloc * __restrict alloc,
                   unsigned int                      chunk_nr,
                   unsigned int                      chunk_per_block,
                   size_t                            chunk_size)
{
	stroll_falloc_assert_api(alloc);
	stroll_falloc_assert_api(chunk_nr);
	stroll_falloc_assert_api(chunk_per_block > 1);
	stroll_falloc_assert_api(chunk_nr >= chunk_per_block);
	stroll_falloc_assert_api(chunk_size);

	size_t blk_sz;

	chunk_size = stroll_align_upper(
		chunk_size,
		sizeof_member(union stroll_alloc_chunk, next_free));
	blk_sz = sizeof(struct stroll_falloc_block) +
	         (chunk_per_block * chunk_size);

	stroll_dlist_init(&alloc->blocks);
	alloc->chunk_cnt = 0;
	alloc->chunk_nr = chunk_nr;
	alloc->block_al = 1UL << stroll_pow2_upul(blk_sz);
	alloc->chunk_per_block = chunk_per_block;
	alloc->chunk_sz = chunk_size;
	alloc->block_sz = blk_sz;
}

void
stroll_falloc_fini(struct stroll_falloc * __restrict alloc)
{
	stroll_falloc_assert_alloc_api(alloc);

	while (!stroll_dlist_empty(&alloc->blocks)) {
		struct stroll_dlist_node * node;

		node  = stroll_dlist_dqueue_front(&alloc->blocks);

		free(stroll_dlist_entry(node,
		                        struct stroll_falloc_block,
		                        node));
	}
}

#if defined(CONFIG_STROLL_ALLOC)

#include "alloc.h"

struct stroll_falloc_impl {
	struct stroll_alloc  iface;
	struct stroll_falloc falloc;
};

static __stroll_nonull(1) __stroll_nothrow
void
stroll_falloc_impl_free(struct stroll_alloc * __restrict alloc,
                        void * __restrict                chunk)
{
	stroll_falloc_assert_intern(alloc);

	return stroll_falloc_free(&((struct stroll_falloc_impl *)alloc)->falloc,
	                          chunk);
}

static __stroll_nonull(1)
       __malloc(stroll_falloc_impl_free, 2)
       __assume_align(sizeof(union stroll_alloc_chunk *))
       __stroll_nothrow
       __warn_result
void *
stroll_falloc_impl_alloc(struct stroll_alloc * __restrict alloc)
{
	stroll_falloc_assert_intern(alloc);

	return stroll_falloc_alloc(
		&((struct stroll_falloc_impl *)alloc)->falloc);
}

static __stroll_nonull(1) __stroll_nothrow
void
stroll_falloc_impl_fini(struct stroll_alloc * __restrict alloc)
{
	stroll_falloc_assert_intern(alloc);

	stroll_falloc_fini(&((struct stroll_falloc_impl *)alloc)->falloc);
}

static const struct stroll_alloc_ops stroll_falloc_impl_ops = {
	.alloc = stroll_falloc_impl_alloc,
	.free  = stroll_falloc_impl_free,
	.fini  = stroll_falloc_impl_fini
};

struct stroll_alloc *
stroll_falloc_create_alloc(unsigned int chunk_nr,
                           unsigned int chunk_per_block,
                           size_t       chunk_size)
{
	stroll_falloc_assert_api(chunk_nr);
	stroll_falloc_assert_api(chunk_per_block > 1);
	stroll_falloc_assert_api(chunk_nr >= chunk_per_block);
	stroll_falloc_assert_api(chunk_size);

	struct stroll_falloc_impl * alloc;

	alloc = malloc(sizeof(*alloc));
	if (!alloc)
		return NULL;

	stroll_falloc_init(&alloc->falloc,
	                   chunk_nr,
	                   chunk_per_block,
	                   chunk_size);

	alloc->iface.ops = &stroll_falloc_impl_ops;

	return &alloc->iface;
}

#endif /* defined(CONFIG_STROLL_ALLOC) */
