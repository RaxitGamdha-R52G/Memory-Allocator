#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "allocator.h"

#define TESTS (size_t)10000
static volatile unsigned char sink;

typedef void *(*alloc_fn)(size_t);
typedef void *(*calloc_fn)(size_t, size_t);
typedef void (*free_fn)(void *);

typedef struct
{
    double allocator_time;
    double stdlib_time;
    size_t mmap_count;
    size_t munmap_count;
} benchmark_result;

static double benchmark_alloc(alloc_fn alloc, free_fn dealloc, const size_t *sizes)
{
    void *ptrs[TESTS];

    clock_t start = clock();

    for (size_t i = 0; i < TESTS; i++)
    {
        ptrs[i] = alloc(sizes[i]);

        if (ptrs[i])
        {
            ((unsigned char *)ptrs[i])[0] = 1;
            sink ^= ((unsigned char *)ptrs[i])[0];
        }
    }

    for (size_t i = 0; i < TESTS; i++)
        dealloc(ptrs[i]);

    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

static double benchmark_calloc(calloc_fn alloc, free_fn dealloc, const size_t *nmemb, const size_t *sizes)
{
    void *ptrs[TESTS];

    clock_t start = clock();

    for (size_t i = 0; i < TESTS; i++)
    {
        ptrs[i] = alloc(nmemb[i], sizes[i]);

        if (ptrs[i])
            sink ^= ((unsigned char *)ptrs[i])[0];
    }

    for (size_t i = 0; i < TESTS; i++)
        dealloc(ptrs[i]);

    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

static void print_benchmark(const char *name1, const char *name2, const benchmark_result result)
{
    printf("\n========== %s vs %s ==========\n", name1, name2);
    printf("Iterations: %zu\n\n", TESTS);

    printf("%-20s : %.6f seconds\n", name1, result.allocator_time);
    printf("%-20s : %.6f seconds\n", name2, result.stdlib_time);

    printf("\nRatio: %.2fx\n\n", result.allocator_time / result.stdlib_time);

    printf("Total mmap() function calls   : %zu\n", result.mmap_count);
    printf("Total munmap() function calls : %zu\n", result.munmap_count);
}

static void run_benchmark(size_t *nmemb, size_t *sizes)
{
    benchmark_result result = {0};

    size_t before_mmap;
    size_t before_munmap;

    if (nmemb)
    {
        for (size_t i = 0; i < TESTS; i++)
        {
            nmemb[i] = (rand() % 32) + 1;
            sizes[i] = (rand() % 64) + 1;
        }

        before_mmap = mmap_call;
        before_munmap = munmap_call;

        result.allocator_time = benchmark_calloc(mem_calloc, mem_free, nmemb, sizes);
        result.stdlib_time = benchmark_calloc(calloc, free, nmemb, sizes);

        result.mmap_count = mmap_call - before_mmap;
        result.munmap_count = munmap_call - before_munmap;

        print_benchmark("mem_calloc/mem_free", "calloc/free", result);
    }
    else
    {
        for (size_t i = 0; i < TESTS; i++)
            sizes[i] = (rand() % 512) + 1;

        before_mmap = mmap_call;
        before_munmap = munmap_call;

        result.allocator_time = benchmark_alloc(mem_alloc, mem_free, sizes);
        result.stdlib_time = benchmark_alloc(malloc, free, sizes);

        result.mmap_count = mmap_call - before_mmap;
        result.munmap_count = munmap_call - before_munmap;

        print_benchmark("mem_alloc/mem_free", "malloc/free", result);
    }
}

int main(void)
{
    srand((unsigned)time(NULL));

    size_t nmemb[TESTS];
    size_t sizes[TESTS];

    run_benchmark(NULL, sizes);

    run_benchmark(nmemb, sizes);
    return 0;
}