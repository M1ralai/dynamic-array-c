# dynamic-array

A small, generic dynamic array implementation in C, built to explore memory management, dynamic allocation, and the performance trade-offs of different capacity growth strategies.

The implementation uses contiguous heap memory, supports arbitrary fixed-size elements, and allows its initial capacity and growth policy to be configured at compile time.

The project includes unit tests, configurable test builds, and benchmarks comparing linear and geometric capacity growth.

## How it works

An `array` maintains three properties:

- `len`: Number of elements currently stored.
- `cap`: Number of elements the allocated buffer can hold.
- `data_size`: Size of each element in bytes.

Elements are stored contiguously in heap memory.

`array_push()` copies an element into the buffer using `memcpy()`. When the array reaches its capacity, the buffer is expanded using `realloc()`.

`array_get()` copies the element at a specified index into a caller-provided output buffer.

### Capacity growth

The initial capacity and growth policy are controlled through two compile-time macros:

```c
#ifndef CAP_INITIAL
#define CAP_INITIAL 4
#endif

#ifndef CAP_GROWTH
#define CAP_GROWTH +4
#endif
```

Both can be overridden using compiler flags.

For example:

```sh
-DCAP_INITIAL=8 '-DCAP_GROWTH=*2'
```

The implementation currently supports growth expressions such as:

| Policy | Expression | Behavior |
|---|---|---|
| Linear | `+4` | Adds four slots per expansion |
| Geometric | `*2` | Doubles the capacity |
| Geometric | `*3` | Triples the capacity |

Linear growth minimizes unused capacity but can require many more reallocations.

Geometric growth reduces the frequency of reallocations at the expense of potentially allocating more memory than immediately necessary.

## API

```c
array *array_create(size_t data_size);
int array_push(array *arr, const void *data);
int array_get(const array *arr, size_t index, void *value);
void array_destroy(array *arr);
```

The implementation stores copies of elements rather than managing their ownership.

## Example

```c
#include "dynamic_array/array.h"
#include <stdio.h>

int main(void) {
    array *a = array_create(sizeof(int));
    if (!a) return 1;

    int value = 42;

    if (array_push(a, &value) != 0) {
        array_destroy(a);
        return 1;
    }

    int out = 0;

    if (array_get(a, 0, &out) == 0)
        printf("%d\n", out);

    array_destroy(a);
    return 0;
}
```

## Build

The project uses a Makefile.

```sh
mkdir -p bin
make
```

The default build compiles and runs the demonstration in `src/main.c`.

## Testing

Unit tests verify array creation, insertion, indexed retrieval, capacity expansion, and data integrity across reallocations.

The test suite includes inserting and retrieving 10,000 elements to verify that previously stored values remain intact after repeated capacity growth.

Tests have also been run with AddressSanitizer without reported memory errors.

### Run tests

```sh
make test
```

### Test multiple configurations

```sh
make test-matrix
```

The configuration matrix tests different initial capacities and growth factors to verify that the implementation behaves correctly under multiple allocation policies.

## Benchmarks

The project includes two benchmarks:

- **Integer benchmark:** Measures insertion performance with 4-byte integers across increasing array lengths.
- **Struct benchmark:** Measures insertion performance for larger elements while keeping the total stored data volume approximately constant.

All benchmarks were compiled with:

```sh
-O2 -Wall -Wextra -Wpedantic
```

Measurements were performed on an Apple Silicon Mac with 8 GB of unified memory.

Each benchmark configuration was executed five times, and the median execution time was recorded.

Timing covers the insertion loop, including capacity checks and expansion. Array creation and destruction are excluded.

The results represent observations from a specific environment, not universal performance guarantees.

### Benchmark 1: Integer insertion

Each element occupies 4 bytes.

| Elements | ×2 | ×3 | +4 |
|---|---:|---:|---:|
| 100,000 | 1.069 ms | 1.082 ms | 1.738 ms |
| 1,000,000 | 7.737 ms | 7.623 ms | 10.954 ms |
| 10,000,000 | 46.720 ms | 45.467 ms | 139.987 ms |

#### Capacity behavior at 10 million elements

| Metric | ×2 | ×3 | +4 |
|---|---:|---:|---:|
| Capacity expansions | 21 | 13 | 2,499,998 |
| Final capacity | 16,777,216 | 12,754,584 | 10,000,000 |
| Allocated buffer | 64.00 MiB | 48.65 MiB | 38.15 MiB |

Despite performing approximately 2.5 million capacity expansions, linear growth completed the benchmark in approximately 140 ms.

This result illustrates that the number of `realloc()` calls alone does not determine execution time.

A possible explanation is that the memory allocator can expand some allocations without relocating the underlying buffer. However, this behavior was not instrumented directly.

### Benchmark 2: Struct insertion

To investigate the impact of element size, three fixed-size structures were tested:

```c
typedef struct {
    unsigned char data[64];
} small_struct;

typedef struct {
    unsigned char data[256];
} medium_struct;

typedef struct {
    unsigned char data[1024];
} large_struct;
```

The element count was adjusted to keep the stored data volume approximately constant at 61 MiB.

| Element size | Elements | ×2 | ×3 | +4 |
|---|---:|---:|---:|---:|
| 64 B | 1,000,000 | 13.444 ms | 11.790 ms | 166.437 ms |
| 256 B | 250,000 | 8.686 ms | 6.204 ms | 139.013 ms |
| 1024 B | 62,500 | 6.232 ms | 8.068 ms | 150.136 ms |

#### Memory allocation

The following table shows the final buffer capacity in MiB, not total physical memory consumption.

| Element size | ×2 | ×3 | +4 |
|---|---:|---:|---:|
| 64 B | 64.00 | 86.50 | 61.04 |
| 256 B | 64.00 | 115.33 | 61.04 |
| 1024 B | 64.00 | 153.77 | 61.04 |

### Observations

**1. Geometric growth substantially reduces capacity expansions.**

For 10 million integers, doubling capacity required only 21 expansions, compared with almost 2.5 million for linear growth.

**2. Fewer reallocations do not necessarily mean proportionally faster execution.**

The performance difference between linear and geometric growth was far smaller than the difference in expansion counts.

Allocator behavior, memory copying, and insertion overhead may all contribute to the observed results.

**3. Element size affects performance.**

When approximately 61 MiB of data was inserted, larger elements sometimes resulted in lower execution times.

One contributing factor is the reduced number of `array_push()` calls required to insert the same total data volume.

**4. More aggressive geometric growth does not always produce better performance.**

Tripling capacity occasionally outperformed doubling it, but the opposite was also observed.

These measurements are not sufficient to establish a universally faster strategy.

**5. Memory efficiency depends on the final array length.**

In the 1024-byte struct benchmark, the ×3 strategy allocated 153.77 MiB to store approximately 61 MiB of data.

In contrast, the ×2 strategy allocated 64 MiB.

The amount of unused capacity depends on where the final element count falls relative to the growth boundaries.

### Complexity

Assuming capacity expansions that relocate the allocation copy the existing elements:

| Operation / policy | Amortized time |
|---|---|
| Indexed retrieval | O(1) |
| Push with geometric growth | O(1) |
| Push with fixed-increment growth | O(n) |

These complexity bounds describe the general copying-cost model.

Actual execution times also depend on the memory allocator, which may sometimes expand allocations in place.

### Run benchmarks

Integer benchmark:

```sh
make bench-all
```

Struct benchmark:

```sh
make bench-struct-all
```

The Makefile compiles separate benchmark executables for each growth policy, ensuring the same implementation is evaluated under different compile-time configurations.

## Limitations

- `realloc()` may move the underlying buffer, invalidating pointers into it.
- Elements are copied by value. Memory referenced by stored pointers is not automatically managed.
- The implementation is not thread-safe.
- Allocation overflow and invalid-input handling are not comprehensive.
- The benchmarks do not directly measure allocation relocation counts or actual bytes copied by the allocator.
- Benchmark results may vary across compilers, operating systems, allocators, and hardware.
- This project is experimental and is not intended to be a production-ready container library.

## Motivation

This project started as an exercise in implementing a generic dynamic array in C.

It gradually became an opportunity to investigate how allocation strategies influence both theoretical complexity and observed performance.

The main objective is not to identify a universally optimal growth factor, but to understand the trade-offs between allocation frequency, unused capacity, element size, and allocator behavior through implementation and measurement.

---

Part of [choplib](https://github.com/choplib), a collection of small C experiments.
