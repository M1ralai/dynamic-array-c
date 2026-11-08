#include "array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

array *array_create(int data_size) {
    array *ret = malloc(sizeof *ret);
    ret->data = malloc(data_size * CAP_INITIAL);
    ret->cap = CAP_INITIAL;
    ret->len = 0;
    ret->data_size = data_size;

    return ret;
}

static int resize_cap(array *arr) {
    arr->cap = arr->cap CAP_GROWTH;
    arr->data = realloc(arr->data, arr->cap * arr -> data_size);
    if (arr -> data == NULL)  {
        return -1;
    }
    return 0;
}

int array_push(array *arr, const void *data) {
    if (arr->len >= arr->cap) {
        int ret = resize_cap(arr);
        if (ret) {
            return ret;
        }
    }

    char *slot = (char *)arr->data + arr->data_size * arr->len;

    memcpy(slot, data, arr->data_size);
    arr->len++;
    return 0;
}

void *array_get(const array *arr, int index) {
    return (index >= 0 && index < arr->len)
               ? (char *)arr->data + index * arr->data_size
               : NULL;
}

void array_destroy(array *arr) {
    free(arr->data);
    free(arr);
}
