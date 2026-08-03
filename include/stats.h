#ifndef STATS_H
#define STATS_H

#include <stddef.h>

// Structure of Statistics
typedef struct
{
    // Allocator API Stats 
    struct
    {
        size_t alloc;
        size_t calloc;
        size_t realloc;
        size_t free;
    } api;

} allocator_stats;

// Global statistics object 
extern allocator_stats stats;

// Stats functions
void stats_reset(void);
const allocator_stats *stats_get(void);

#endif // STATS_H