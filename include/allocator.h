#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

#define ALIGNMENT 8
#define ALIGNMENT_MASK (ALIGNMENT - 1)

// Memory Allocator Functions
void *mem_alloc(size_t size);
void *mem_calloc(size_t quantity, size_t base_size);
void *mem_realloc(void *ptr, size_t size);
void mem_free(void *ptr);

extern size_t mmap_call;
extern size_t munmap_call;

#endif // ALLOCATOR_H
