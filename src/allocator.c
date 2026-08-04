#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "allocator.h"
#include "block.h"
#include "page.h"
#include "stats.h"

static size_t normalize_size(size_t size)
{
    size = (size + ALIGNMENT_MASK) & ~ALIGNMENT_MASK;
    return size;
}

static bool is_valid_request(size_t quantity, size_t base_size)
{
    size_t max_size = SIZE_MAX - ALIGNMENT_MASK - BLOCK_HEADER_SIZE - PAGE_HEADER_SIZE;

    return quantity <= max_size / base_size;
}

static void *allocate_region(size_t size)
{
    stats.internal.allocate_region++;

    // Normalize the size first
    size = normalize_size(size);

    // Find any free block from current memory page
    p_mem_block block = find_first_fit(size);

    if (block == NULL)
    {
        // Get free memory block from a new page
        block = add_page(size);
        if (block == NULL)
            return NULL;
    }

    if (block->size >= size + BLOCK_HEADER_SIZE + BLOCK_MIN_USABLE_SIZE)
        split_block(block, size);

    block->free = 0;

    stats.blocks.current_used++;
    stats.blocks.current_free--;

    return (char *)block + BLOCK_HEADER_SIZE;
}

static void *relocate_block(void *ptr, p_mem_block block, size_t new_size)
{
    stats.internal.relocate_block++;

    void *new_region = mem_alloc(new_size);

    if (new_region == NULL)
        return NULL;

    memcpy(new_region, ptr, block->size);
    mem_free(ptr);

    return new_region;
}

static bool expand_block(p_mem_block block, size_t new_size)
{
    stats.internal.expand_block++;

    if (block->next == NULL || !block->next->free)
        return false;

    // Remaining Size
    size_t rem_size = new_size - block->size;

    if (block->next->size + BLOCK_HEADER_SIZE < rem_size)
        return false;

    coalesce_blocks(block, block->next);

    if (block->size - new_size >= BLOCK_HEADER_SIZE + BLOCK_MIN_USABLE_SIZE)
    {
        split_block(block, new_size);
    }

    block->free = 0;

    return true;
}

static void shrink_block(p_mem_block block, size_t new_size)
{
    stats.internal.shrink_block++;

    // Remaining Size
    size_t rem_size = block->size - new_size;

    if (rem_size >= BLOCK_HEADER_SIZE + BLOCK_MIN_USABLE_SIZE)
    {
        split_block(block, new_size);
    }

    block->free = 0;
}

void *mem_alloc(size_t size)
{
    stats.api.alloc++;

    if (size == 0)
        return NULL;

    // Validate the size first to avoid overflow
    if (!is_valid_request(1, size))
    {
        fprintf(stderr, "mem_alloc request failed due to requested memory size overflow\n");
        return NULL;
    }

    return allocate_region(size);
}

void *mem_calloc(size_t quantity, size_t base_size)
{
    stats.api.calloc++;

    if (!quantity || !base_size)
        return NULL;

    // Validate if given quantity with its base_size can be allocated in memory
    if (!is_valid_request(quantity, base_size))
    {
        fprintf(stderr, "mem_calloc request failed due to requested memory size overflow\n");
        return NULL;
    }

    size_t req_size = quantity * base_size;

    void *region = allocate_region(req_size);
    if (region == NULL)
        return NULL;

    return memset(region, 0, req_size);
}

void *mem_realloc(void *ptr, size_t new_size)
{
    stats.api.realloc++;

    if (!ptr && !new_size)
        return NULL;

    if (!ptr)
        return mem_alloc(new_size);

    if (!new_size)
    {
        mem_free(ptr);
        return NULL;
    }

    p_mem_block block = (p_mem_block)((char *)ptr - BLOCK_HEADER_SIZE);

    // Validate new_size
    if (!is_valid_request(1, new_size))
    {
        fprintf(stderr, "mem_realloc request failed due to requested memory size overflow\n");
        return NULL;
    }

    // Normalize new_size
    new_size = normalize_size(new_size);

    // Same new_size as old_size
    if (block->size == new_size)
        return ptr;

    // Shrink current block
    if (block->size > new_size)
    {
        shrink_block(block, new_size);
        return ptr;
    }

    // Extend current block
    if (expand_block(block, new_size))
        return ptr;

    // Alloc new block and free current block
    return relocate_block(ptr, block, new_size);
}

void mem_free(void *ptr)
{
    stats.api.free++;

    if (ptr == NULL)
        return;

    p_mem_block block = (p_mem_block)((char *)ptr - BLOCK_HEADER_SIZE);

    if (block->free)
        return;

    remove_block(block);
}