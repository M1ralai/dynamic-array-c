#include "../dynamic_array/array.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define REPEATS 5

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

static bench_result benchmark(size_t count) {
    array *arr = array_create(sizeof(int));

    if (!arr) {
        fprintf(stderr, "array_create failed\n");
        exit(EXIT_FAILURE);
    }

    struct timespec start, end;
    size_t growths = 0;
    size_t previous_cap = arr->cap;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (size_t i = 0; i < count; i++) {
        int value = (int)i;

        if (array_push(arr, &value) != 0) {
            fprintf(stderr, "array_push failed\n");
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

static void run_benchmark(size_t count) {
    double times[REPEATS];
    bench_result result = {0};

    for (int i = 0; i < REPEATS; i++) {
        result = benchmark(count);
        times[i] = result.ms;
    }

    qsort(times, REPEATS, sizeof(double), compare_double);

    printf("%zu,%0.3f,%zu,%zu,%zu\n",
           count,
           times[REPEATS / 2],
           result.growths,
           result.capacity,
           result.capacity * sizeof(int));
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <count>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *end;
    unsigned long long count = strtoull(argv[1], &end, 10);

    if (argv[1][0] == '-' || end == argv[1] ||
        *end != '\0' || count == 0 ||
        count > 10000000ULL) {
        fprintf(stderr, "Invalid count\n");
        return EXIT_FAILURE;
    }

    printf("count,median_ms,growths,capacity,bytes\n");
    run_benchmark((size_t)count);

    return EXIT_SUCCESS;
}
