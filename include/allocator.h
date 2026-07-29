#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#define ALIGNMENT_SIZE (size_t)7

// Memory Allocator Functions
void *mem_alloc(size_t size);
void mem_free(void *ptr);

extern size_t mmap_call;
extern size_t munmap_call;

#endif // ALLOCATOR_H
