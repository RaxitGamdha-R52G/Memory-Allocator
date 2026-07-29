#ifndef PAGE_H
#define PAGE_H
#include <stddef.h>

// Forward declaration of Memory Block for reference
struct mem_block;
typedef struct mem_block *p_mem_block;

// Structure of Memory Page
typedef struct mem_page
{
    size_t size;

    struct mem_page *prev;
    struct mem_page *next;

    p_mem_block first_block;
} *p_mem_page;

// Default Memory Page Properties
#define PAGE_DEFAULT_SIZE 4096
#define PAGE_HEADER_SIZE sizeof(struct mem_page)

// Memory Page Node Handlers
extern p_mem_page g_heap_head;
extern p_mem_page g_heap_tail;

// Memory Page Functions
p_mem_block add_page(size_t size);
void remove_page(p_mem_block block);

p_mem_block find_first_fit(size_t size);
int is_page_free(p_mem_block block);

#endif // PAGE_H