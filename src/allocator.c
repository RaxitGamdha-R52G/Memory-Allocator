#include "allocator.h"
#include "block.h"
#include "page.h"

static size_t normalize_size(size_t size)
{
    size = (size + ALIGNMENT_SIZE) & ~ALIGNMENT_SIZE;
    return size;
}

void *mem_alloc(size_t size)
{
    if (size == 0)
        return NULL;

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

void mem_free(void *ptr)
{
    if (ptr == NULL)
        return;

    p_mem_block block = (p_mem_block)((char *)ptr - BLOCK_HEADER_SIZE);

    if (block->free)
        return;

    remove_block(block);
}