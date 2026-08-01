#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "allocator.h"

#define TESTS (size_t)10000
static volatile unsigned char sink;

typedef struct
{
    double allocator_time;
    double stdlib_time;
    size_t mmap_count;
    size_t munmap_count;
} benchmark_result;

typedef void *(*alloc_fn)(size_t);
typedef void *(*calloc_fn)(size_t, size_t);
typedef void *(*realloc_fn)(void *, size_t);
typedef void (*free_fn)(void *);

typedef struct
{
    alloc_fn alloc;
    calloc_fn calloc;
    realloc_fn realloc;
    free_fn free;
} allocator_ops;

static const allocator_ops mem_allocator = {
    .alloc = mem_alloc,
    .calloc = mem_calloc,
    .realloc = mem_realloc,
    .free = mem_free};

static const allocator_ops libc_allocator = {
    .alloc = malloc,
    .calloc = calloc,
    .realloc = realloc,
    .free = free};

typedef enum
{
    BENCH_ALLOC,
    BENCH_CALLOC,
    BENCH_REALLOC
} benchmark_type;

static double benchmark_alloc(const allocator_ops *allocator, const size_t *sizes)
{
    void *ptrs[TESTS];

    clock_t start = clock();

    for (size_t i = 0; i < TESTS; i++)
    {
        ptrs[i] = allocator->alloc(sizes[i]);

        if (ptrs[i])
        {
            ((unsigned char *)ptrs[i])[0] = 1;
            sink ^= ((unsigned char *)ptrs[i])[0];
        }
    }

    for (size_t i = 0; i < TESTS; i++)
        allocator->free(ptrs[i]);

    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

static double benchmark_calloc(const allocator_ops *allocator, const size_t *nmemb, const size_t *sizes)
{
    void *ptrs[TESTS];

    clock_t start = clock();

    for (size_t i = 0; i < TESTS; i++)
    {
        ptrs[i] = allocator->calloc(nmemb[i], sizes[i]);

        if (ptrs[i])
            sink ^= ((unsigned char *)ptrs[i])[0];
    }

    for (size_t i = 0; i < TESTS; i++)
        allocator->free(ptrs[i]);

    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

static double benchmark_realloc(const allocator_ops *allocator, const size_t *initial_size, const size_t *new_size)
{
    void *ptrs[TESTS];

    clock_t start = clock();

    for (size_t i = 0; i < TESTS; ++i)
    {
        ptrs[i] = allocator->alloc(initial_size[i]);

        if (ptrs[i])
        {
            ((unsigned char *)ptrs[i])[0] = 1;
            sink ^= ((unsigned char *)ptrs[i])[0];
        }
    }

    for (size_t i = 0; i < TESTS; ++i)
    {
        ptrs[i] = allocator->realloc(ptrs[i], new_size[i]);

        if (ptrs[i])
        {
            ((unsigned char *)ptrs[i])[0] = 2;
            sink ^= ((unsigned char *)ptrs[i])[0];
        }
    }

    for (size_t i = 0; i < TESTS; i++)
        allocator->free(ptrs[i]);

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

static void run_benchmark(const benchmark_type type)
{
    benchmark_result result = {0};

    size_t before_mmap;
    size_t before_munmap;

    size_t nmemb[TESTS];
    size_t sizes[TESTS];
    size_t new_sizes[TESTS];

    switch (type)
    {
    case BENCH_ALLOC:
        for (size_t i = 0; i < TESTS; i++)
            sizes[i] = (rand() % 512) + 1;

        before_mmap = mmap_call;
        before_munmap = munmap_call;

        result.allocator_time = benchmark_alloc(&mem_allocator, sizes);
        result.stdlib_time = benchmark_alloc(&libc_allocator, sizes);

        result.mmap_count = mmap_call - before_mmap;
        result.munmap_count = munmap_call - before_munmap;

        print_benchmark("mem_alloc/mem_free", "malloc/free", result);
        break;

    case BENCH_CALLOC:
        for (size_t i = 0; i < TESTS; i++)
        {
            nmemb[i] = (rand() % 32) + 1;
            sizes[i] = (rand() % 64) + 1;
        }

        before_mmap = mmap_call;
        before_munmap = munmap_call;

        result.allocator_time = benchmark_calloc(&mem_allocator, nmemb, sizes);
        result.stdlib_time = benchmark_calloc(&libc_allocator, nmemb, sizes);

        result.mmap_count = mmap_call - before_mmap;
        result.munmap_count = munmap_call - before_munmap;

        print_benchmark("mem_calloc/mem_free", "calloc/free", result);
        break;

    case BENCH_REALLOC:
        for (size_t i = 0; i < TESTS; i++)
        {
            sizes[i] = (rand() % 512) + 1;
            new_sizes[i] = (rand() % 512) + 1;
        }

        before_mmap = mmap_call;
        before_munmap = munmap_call;

        result.allocator_time =
            benchmark_realloc(&mem_allocator, sizes, new_sizes);

        result.stdlib_time =
            benchmark_realloc(&libc_allocator, sizes, new_sizes);

        result.mmap_count = mmap_call - before_mmap;
        result.munmap_count = munmap_call - before_munmap;

        print_benchmark("mem_realloc/mem_free", "realloc/free", result);
        break;

    default:
        break;
    }
}

int main(void)
{
    srand((unsigned)time(NULL));

    run_benchmark(BENCH_ALLOC);
    run_benchmark(BENCH_CALLOC);
    run_benchmark(BENCH_REALLOC);
    return 0;
}