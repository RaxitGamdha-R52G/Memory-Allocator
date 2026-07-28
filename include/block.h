#ifndef BLOCK_H
#define BLOCK_H

#include <stddef.h>

typedef struct mem_block{
    size_t size;
    int free;
} *p_mem_block;

void *mem_alloc(size_t size);
void mem_free(void *ptr);

#endif // BLOCK_H