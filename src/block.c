#include <sys/mman.h>
#include <stdio.h>
#include "block.h"

void *mem_alloc(size_t size)
{
    if (size == 0)
        return NULL;

    size_t block_size = size + sizeof(struct mem_block);

    void *region = mmap(NULL, block_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (region == MAP_FAILED)
        return NULL;

    p_mem_block block = (p_mem_block)region;

    block->size = size;
    block->free = 0;

    return (char *)block + sizeof(struct mem_block);
}

void mem_free(void *ptr)
{
    if (ptr == NULL)
        return;

    p_mem_block block = (p_mem_block)((char *)ptr - sizeof(struct mem_block));

    block->free = 1;
    size_t block_size = block->size + sizeof(struct mem_block);

    if (munmap(block, block_size) != 0)
    {
        printf("munmap failed\n");
    }
}