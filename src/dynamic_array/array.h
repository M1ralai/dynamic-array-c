#ifndef DYNAMIC_ARRAY
#define DYNAMIC_ARRAY
#include <unistd.h>

// this is for if the cap is reached
// this operand will be used in to
// expand cap
#ifndef CAP_GROWTH
#define CAP_GROWTH +4
#endif

#ifndef CAP_INITIAL
#define CAP_INITIAL 4
#endif

typedef struct {
    size_t cap;
    void *data;
    size_t data_size;

    size_t len;
} array;

array *array_create(size_t data_size);

int array_push(array *arr, const void *data);
int array_get(const array *arr, size_t index, void *value);
void array_destroy(array *arr);

#endif
