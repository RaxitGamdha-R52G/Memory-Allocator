#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "allocator.h"
#include "block.h"
#include "page.h"

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
    else
        block->free = 0;

    return (char *)block + BLOCK_HEADER_SIZE;
}

void *mem_alloc(size_t size)
{
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

void mem_free(void *ptr)
{
    if (ptr == NULL)
        return;

    p_mem_block block = (p_mem_block)((char *)ptr - BLOCK_HEADER_SIZE);

    if (block->free)
        return;

    remove_block(block);
}