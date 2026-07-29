#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "allocator.h"
#define TESTS (size_t)100000

int main(void)
{
    clock_t start, end;
    double my_time, malloc_time;
    srand((unsigned)time(NULL));

    char *ptrs[TESTS];

    start = clock();

    // mem_alloc-mem_free
    for (size_t i = 0; i < TESTS; i++)
    {
        size_t size = (rand() % 512) + 1;
        ptrs[i] = mem_alloc(size);
    }

    for (size_t i = 0; i < TESTS; i++)
        mem_free(ptrs[i]);

    end = clock();
    my_time = (double)(end - start) / CLOCKS_PER_SEC;

    // Malloc-free
    start = clock();

    for (size_t i = 0; i < TESTS; i++)
    {
        size_t size = (rand() % 512) + 1;
        ptrs[i] = malloc(size);
    }

    for (size_t i = 0; i < TESTS; i++)
        free(ptrs[i]);

    end = clock();
    malloc_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\n========== Benchmark ==========\n");
    printf("Iterations: %zu\n\n", TESTS);

    printf("mem_alloc/mem_free : %.6f seconds\n", my_time);
    printf("malloc/free        : %.6f seconds\n", malloc_time);

    printf("\nRatio: %.2fx\n\n", my_time / malloc_time);

    printf("Total mmap() function calls   : %zu\n", mmap_call);
    printf("Total munmap() function calls : %zu\n", munmap_call);

    return 0;
}