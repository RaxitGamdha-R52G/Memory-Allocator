#include <stdio.h>

#include "block.h"
#include "page.h"

void split_block(p_mem_block block, size_t size)
{
    p_mem_block new_block = (p_mem_block)((char *)block + BLOCK_HEADER_SIZE + size);
    new_block->size = block->size - size - BLOCK_HEADER_SIZE;
    new_block->free = 1;
    new_block->page = block->page;

    new_block->prev = block;
    new_block->next = block->next; // Any non-free memory block ahead
    if (block->next != NULL)
        block->next->prev = new_block; // Link back the next non-free memory block to new_block

    block->next = new_block;
    block->size = size;
    // block->free = 0;
}

void coalesce_blocks(p_mem_block curr_block, p_mem_block next_block)
{
    curr_block->size += next_block->size + BLOCK_HEADER_SIZE;
    curr_block->next = next_block->next;

    if (next_block->next != NULL)
        next_block->next->prev = curr_block;
}

void remove_block(p_mem_block block)
{
    block->free = 1;

    if (block->next != NULL && block->next->free)
        coalesce_blocks(block, block->next);

    if (block->prev != NULL && block->prev->free)
    {
        block = block->prev;
        coalesce_blocks(block, block->next);
    }

    if (is_page_free(block))
        remove_page(block);
}