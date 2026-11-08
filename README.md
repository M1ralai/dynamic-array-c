# dynamic-array

A small generic dynamic array in C, built as a learning exercise.

## How it works

An `array` stores fixed-size elements in contiguous heap memory, tracking `len`, `cap`, and `data_size`. `array_push` copies the supplied element with `memcpy`; `array_get` copies an indexed element into a caller-provided output parameter.

The buffer starts at `CAP_INITIAL = 4` and grows with `realloc` according to `CAP_GROWTH` (currently `+4`). Linear growth is simple but requires more copying than geometric growth as the array gets large. Comparing growth policies and their performance/memory trade-offs is a natural follow-up experiment.

## API

```c
array *array_create(size_t data_size);
int array_push(array *arr, const void *data);
int array_get(const array *arr, size_t index, void *value);
void array_destroy(array *arr);
```

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
    if (array_get(a, 0, &out) == 0) printf("%d\n", out);
    array_destroy(a);
    return 0;
}
```

## Build

```sh
mkdir -p bin
make
```

The current Makefile builds **and immediately runs** the demonstration in `src/main.c`.

## Limitations

- `realloc` may move the underlying buffer; pointers into it are not stable across growth.
- Elements are copied by value. Objects referenced by pointer elements are not automatically managed.
- No automated tests or benchmark results are included yet.
- Allocation overflow and invalid-input handling are not comprehensive; this is not a hardened container library.

Part of [choplib](https://github.com/choplib), a collection of small C experiments.
