#ifndef BLOCK_H
#define BLOCK_H

#include <stddef.h>
#include "allocator.h"

// Forward declaration of Memory Page for reference
struct mem_page;
typedef struct mem_page *p_mem_page;

// Structure of Memory Block
typedef struct mem_block
{
    size_t size;
    size_t free;

    struct mem_block *prev;
    struct mem_block *next;

    p_mem_page page;
} *p_mem_block;

// Default Memory Block Properties
#define BLOCK_HEADER_SIZE sizeof(struct mem_block)
#define BLOCK_MIN_USABLE_SIZE ALIGNMENT

// Memory Functions
void split_block(p_mem_block block, size_t size);
void coalesce_blocks(p_mem_block curr_block, p_mem_block next_block);
void remove_block(p_mem_block block);

#endif // BLOCK_H