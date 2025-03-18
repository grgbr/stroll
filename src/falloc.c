#include "stroll/falloc.h"
#include "stroll/pow2.h"
#include <stdlib.h>
#include <errno.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include "stroll/assert.h"

#define stroll_falloc_assert_api(_expr) \
	stroll_assert("stroll:falloc", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_falloc_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include "stroll/assert.h"

#define stroll_falloc_assert_intern(_expr) \
	stroll_assert("stroll:falloc", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_falloc_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_falloc_assert_alloc_api(_alloc) \
	stroll_falloc_assert_api(_alloc); \
	stroll_falloc_assert_api( \
		stroll_aligned((_alloc)->chunk_sz, \
		               sizeof_member(struct stroll_falloc_block, \
		                             next_free))); \
	stroll_falloc_assert_api( \
		(_alloc)->block_sz == \
		(1UL << stroll_pow2_upul((_alloc)->chunk_nr * \
		                         (_alloc)->chunk_sz)))

#define stroll_falloc_assert_alloc_intern(_alloc) \
	stroll_falloc_assert_intern(_alloc); \
	stroll_falloc_assert_intern( \
		stroll_aligned((_alloc)->chunk_sz, \
		               sizeof_member(struct stroll_falloc_block, \
		                             next_free))); \
	stroll_falloc_assert_intern( \
		(_alloc)->block_sz == \
		(1UL << stroll_pow2_upul((_alloc)->chunk_nr * \
		                         (_alloc)->chunk_sz)))

struct stroll_falloc_block {
	unsigned int             busy_cnt;  /* Count of allocated chunks */
	void **                  next_free; /* Pointer to next free chunk */
	struct stroll_dlist_node node;
};

#define stroll_falloc_assert_block(_block, _alloc) \
	stroll_falloc_assert_intern(_block); \
	stroll_falloc_assert_alloc_intern(alloc); \
	stroll_falloc_assert_intern((void *)(_block) >= \
	                            (void *)alloc->block_sz); \
	stroll_falloc_assert_intern( \
		stroll_aligned((unsigned long)(_block), \
		               (unsigned long)(_alloc)->block_sz)); \
	stroll_falloc_assert_intern((_block)->busy_cnt <= (_alloc)->chunk_nr); \
	stroll_falloc_assert_intern( \
		!(_block)->next_free || \
		(((const void *)(_block)->next_free >= \
		  stroll_falloc_chunks_from_block(_block, _alloc)) && \
		  ((const void *)(_block)->next_free < \
		   (const void *)(_block))))

static
void *
stroll_falloc_chunks_from_block(
	const struct stroll_falloc_block * __restrict block,
	const struct stroll_falloc * __restrict       alloc)
{
	stroll_falloc_assert_intern(alloc);
	stroll_falloc_assert_intern((const void *)block >=
	                            (const void *)alloc->block_sz);
	stroll_falloc_assert_intern(stroll_aligned((unsigned long)block,
	                                           (unsigned long)
	                                           alloc->block_sz));

STROLL_IGNORE_WARN("-Wcast-qual")
	return (void *)block - alloc->block_sz;
STROLL_RESTORE_WARN
}

static
void *
stroll_falloc_alloc_blockn_chunk(struct stroll_falloc * __restrict alloc)
{
	stroll_falloc_assert_alloc_intern(alloc);

	void *                       chunks;
	struct stroll_falloc_block * blk;
	int                          err;

	err = posix_memalign(&chunks,
	                     alloc->block_sz,
	                     alloc->block_sz + sizeof(*blk));
	if (err) {
		stroll_falloc_assert_intern(err == ENOMEM);
		errno = err;
		return NULL;
	}

	blk = (struct stroll_falloc_block *)(chunks + alloc->block_sz);
	blk->busy_cnt = 1;
	blk->next_free = NULL;
	stroll_dlist_append(&alloc->blocks, &blk->node);

	return chunks;
}

static
void
stroll_falloc_free_block(struct stroll_falloc_block * __restrict block,
                         const struct stroll_falloc * __restrict alloc)
{
	stroll_falloc_assert_block(block, alloc);

	stroll_dlist_remove(&block->node);
	free(stroll_falloc_chunks_from_block(block, alloc));
}

static
void *
stroll_falloc_next_free_block_chunk(
	struct stroll_falloc_block * __restrict block,
	const struct stroll_falloc * __restrict alloc)
{
	stroll_falloc_assert_block(block, alloc);
	stroll_falloc_assert_intern(block->busy_cnt < alloc->chunk_nr);

	void * chunk;

	if (block->next_free) {
		chunk = block->next_free;
		block->next_free = *block->next_free;
	}
	else
		chunk = stroll_falloc_chunks_from_block(block, alloc) +
		        (block->busy_cnt * alloc->chunk_sz);

	return chunk;
}

void *
stroll_falloc_alloc(struct stroll_falloc * __restrict alloc)
{
	stroll_falloc_assert_alloc_api(alloc);

	struct stroll_falloc_block * blk;
	void *                       chunk;

	if (!stroll_dlist_empty(&alloc->blocks)) {
		blk = stroll_dlist_entry(stroll_dlist_next(&alloc->blocks),
		                         struct stroll_falloc_block,
		                         node);
		if (blk->busy_cnt < alloc->chunk_nr) {
			chunk = stroll_falloc_next_free_block_chunk(blk, alloc);

			if (++blk->busy_cnt == alloc->chunk_nr)
				/* Block is full: move it to block list tail. */
				stroll_dlist_move_before(&alloc->blocks,
				                         &blk->node);

			return chunk;
		}
	}

	return stroll_falloc_alloc_blockn_chunk(alloc);
}

void
stroll_falloc_free(struct stroll_falloc * __restrict alloc, void * chunk)
{
	stroll_falloc_assert_alloc_api(alloc);

	struct stroll_falloc_block * blk;

	blk = (struct stroll_falloc_block *)
	      (stroll_align_lower((size_t)chunk,
	                          (size_t)alloc->block_sz) + alloc->block_sz);

	*((void **)chunk) = blk->next_free;
	blk->next_free = chunk;

	/*
	 * TODO:
	 * - implement an hysteresis logic ??
	 * - an alternate free block list ??
	 */
	blk->busy_cnt--;
	if (blk->busy_cnt == (alloc->chunk_nr - 1))
		/*
		 * Block is not full: move it to block list head to maximize the
		 * its fill ratio.
		 */
		stroll_dlist_move_after(&alloc->blocks, &blk->node);
	else if (!blk->busy_cnt)
		/* This block contains no more allocated chunks: free it. */
		stroll_falloc_free_block(blk, alloc);
}

void
stroll_falloc_init(struct stroll_falloc * __restrict alloc,
                   unsigned int                      chunk_nr,
                   size_t                            chunk_size)
{
	stroll_falloc_assert_api(alloc);
	stroll_falloc_assert_api(chunk_nr > 1);
	stroll_falloc_assert_api(chunk_size);

	size_t blk_sz;

	chunk_size = stroll_align_upper(
		chunk_size,
		sizeof_member(struct stroll_falloc_block, next_free));
	blk_sz = 1UL << stroll_pow2_upul(chunk_size * chunk_nr);

	stroll_dlist_init(&alloc->blocks);
	alloc->block_sz = blk_sz;
	alloc->chunk_nr = (unsigned int)(blk_sz / chunk_size);
	alloc->chunk_sz = chunk_size;
}

void
stroll_falloc_fini(struct stroll_falloc * __restrict alloc)
{
	stroll_falloc_assert_alloc_api(alloc);

	while (!stroll_dlist_empty(&alloc->blocks)) {
		struct stroll_dlist_node * node;

		node  = stroll_dlist_dqueue_front(&alloc->blocks);

		stroll_falloc_free_block(
			stroll_dlist_entry(node,
			                   struct stroll_falloc_block,
			                   node),
			alloc);
	}
}
