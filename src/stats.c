#include <string.h>

#include "stats.h"

allocator_stats stats = {0};

void stats_reset(void)
{
    memset(&stats, 0, sizeof(stats));
}

const allocator_stats *stats_get(void)
{
    return &stats;
}