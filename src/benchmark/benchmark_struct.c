#include "../dynamic_array/array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define REPEATS 5

typedef struct {
    unsigned char data[64];
} small_struct;

typedef struct {
    unsigned char data[256];
} medium_struct;

typedef struct {
    unsigned char data[1024];
} large_struct;

typedef struct {
    double ms;
    size_t growths;
    size_t capacity;
} bench_result;

static double elapsed_ms(struct timespec start,
                         struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 +
           (end.tv_nsec - start.tv_nsec) / 1e6;
}

static int compare_double(const void *a, const void *b) {
    double x = *(const double *)a;
    double y = *(const double *)b;
    return (x > y) - (x < y);
}

static bench_result benchmark(size_t element_size,
                              size_t count) {
    array *arr = array_create(element_size);

    if (!arr) {
        fprintf(stderr, "array_create failed\n");
        exit(EXIT_FAILURE);
    }

    unsigned char value[sizeof(large_struct)];
    memset(value, 0xAB, sizeof(value));

    size_t growths = 0;
    size_t previous_cap = arr->cap;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (size_t i = 0; i < count; i++) {
        if (array_push(arr, value) != 0) {
            fprintf(stderr, "array_push failed\n");
            array_destroy(arr);
            exit(EXIT_FAILURE);
        }

        if (arr->cap != previous_cap) {
            growths++;
            previous_cap = arr->cap;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    bench_result result = {
        .ms = elapsed_ms(start, end),
        .growths = growths,
        .capacity = arr->cap
    };

    array_destroy(arr);
    return result;
}

static void run_benchmark(size_t element_size,
                          size_t count) {
    double times[REPEATS];
    bench_result result = {0};

    for (int i = 0; i < REPEATS; i++) {
        result = benchmark(element_size, count);
        times[i] = result.ms;
    }

    qsort(times, REPEATS, sizeof(double), compare_double);

    double allocated_mb =
        (double)result.capacity * element_size / (1024 * 1024);

    printf("%zu,%zu,%.3f,%zu,%.2f\n",
           element_size,
           count,
           times[REPEATS / 2],
           result.growths,
           allocated_mb);
}

int main(void) {
    printf("struct_size,count,median_ms,growths,allocated_mib\n");

    run_benchmark(sizeof(small_struct), 1000000);
    run_benchmark(sizeof(medium_struct), 250000);
    run_benchmark(sizeof(large_struct), 62500);

    return 0;
}
