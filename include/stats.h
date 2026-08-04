#ifndef STATS_H
#define STATS_H

#include <stddef.h>

// Structure of Statistics
typedef struct statistics
{
    // Allocator API Stats 
    struct
    {
        size_t alloc;
        size_t calloc;
        size_t realloc;
        size_t free;
    } api;

    // Internal Functions Stats
    struct{
        // Allocation
        size_t allocate_region;
        size_t find_first_fit;

        // Block management 
        size_t split_block;
        size_t coalesce_blocks;
        size_t expand_block;
        size_t shrink_block;
        size_t relocate_block;
        size_t remove_block;

        // Page management
        size_t add_page;
        size_t remove_page;
    } internal;

    // Page Stats
    struct {
        size_t current;
        size_t peak;
        size_t created;
        size_t destroyed;
    } pages;

    // Block Stats
    struct {
        size_t current;
        size_t peak;
        size_t created;
        size_t destroyed;
        size_t current_used;
        size_t current_free;
    } blocks;

} allocator_stats;

// Global statistics object 
extern allocator_stats stats;

#endif // STATS_H