#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>

#include "allocator.h"
#include "page.h"
#include "block.h"

// Memory Page Node Handlers
p_mem_page g_heap_head = NULL;
p_mem_page g_heap_tail = NULL;

// Trace number of system calls
size_t mmap_call = 0;
size_t munmap_call = 0;

// Static functions
static p_mem_page create_page(size_t size);

p_mem_block find_first_fit(size_t size)
{
    p_mem_page curr_page = g_heap_head;

    while (curr_page)
    {
        p_mem_block curr_block = curr_page->first_block;

        while (curr_block)
        {
            if (curr_block->free && curr_block->size >= size)
                return curr_block;
            curr_block = curr_block->next;
        }
        curr_page = curr_page->next;
    }

    return NULL;
}

static p_mem_page create_page(size_t size)
{
    size_t page_region_size = PAGE_DEFAULT_SIZE;
    size_t req_size = PAGE_HEADER_SIZE + BLOCK_HEADER_SIZE + size;

    while (page_region_size < req_size)
    {
        if (page_region_size > SIZE_MAX - PAGE_DEFAULT_SIZE)
        {
            fprintf(stderr, "CREATE_PAGE request size overflow\n");
            return NULL;
        }
        page_region_size += PAGE_DEFAULT_SIZE;
    }

    p_mem_page new_page = mmap(NULL, page_region_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (new_page == MAP_FAILED)
    {
        fprintf(stderr, "CREATE_PAGE request failed\n");
        return NULL;
    }

    // Count total calls were made to mmap
    ++mmap_call;

    new_page->next = NULL;
    new_page->prev = NULL;
    new_page->size = page_region_size - PAGE_HEADER_SIZE;

    p_mem_block block = (p_mem_block)((char *)new_page + PAGE_HEADER_SIZE);
    block->next = NULL;
    block->prev = NULL;
    block->free = 1;
    block->size = new_page->size - BLOCK_HEADER_SIZE;
    block->page = new_page;

    new_page->first_block = block;

    return new_page;
}

p_mem_block add_page(size_t size)
{
    p_mem_page new_page = create_page(size);

    if (new_page == NULL)
        return NULL;

    if (g_heap_head == NULL)
    {
        g_heap_head = new_page;
        g_heap_tail = g_heap_head;
        return g_heap_head->first_block;
    }

    g_heap_tail->next = new_page;
    new_page->prev = g_heap_tail;
    g_heap_tail = new_page;

    return g_heap_tail->first_block;
}

void remove_page(p_mem_block block)
{
    if (block == NULL)
        return;

    p_mem_page page = block->page;

    if (page->prev == NULL)
        g_heap_head = page->next;
    else
        page->prev->next = page->next;

    if (page->next == NULL)
        g_heap_tail = page->prev;
    else
        page->next->prev = page->prev;

    if (munmap(page, page->size + PAGE_HEADER_SIZE) != 0)
    {
        fprintf(stderr, "REMOVE_PAGE request failed\n");
    }
    else
    {
        // Count total calls were made to munmap
        ++munmap_call;
    }
}

int is_page_free(p_mem_block block)
{
    return block &&
           block->free &&
           block == block->page->first_block &&
           block->prev == NULL &&
           block->next == NULL &&
           block->size + BLOCK_HEADER_SIZE == block->page->size;
}