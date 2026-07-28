#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "block.h"

#define TESTS 100

int main(void)
{
    clock_t start, end;
    double my_time, malloc_time;

    start = clock();

    for (long long i = 0; i < TESTS; i++)
    {
        char *p = mem_alloc(i*i+5);
        strcpy(p, "TEST");
        mem_free(p);
    }

    end = clock();
    my_time = (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();

    for (long long i = 0; i < TESTS; i++)
    {
        char *p = malloc(i*i+5);
        strcpy(p, "TEST");
        free(p);
    }

    end = clock();
    malloc_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\n========== Benchmark ==========\n");
    printf("Iterations: %d\n\n", TESTS);

    printf("mem_alloc/mem_free : %.6f seconds\n", my_time);
    printf("malloc/free        : %.6f seconds\n", malloc_time);

    printf("\nRatio: %.2fx\n", my_time / malloc_time);

    return 0;
}