#include "array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

array *array_create(size_t data_size) {
    array *ret = malloc(sizeof *ret);
    if (ret == NULL) {
        return NULL;
    }

    ret->data = malloc(data_size * CAP_INITIAL);
    if (ret->data == NULL) {
        free(ret);
        return NULL;
    }

    ret->cap = CAP_INITIAL;
    ret->len = 0;
    ret->data_size = data_size;

    return ret;
}

static int resize_cap(array *arr) {
    if (arr == NULL)
        return -1;

    int new_cap = arr->cap CAP_GROWTH;

    void *tmp = realloc(arr->data, (size_t)new_cap * arr->data_size);

    if (tmp == NULL) {
        return -1;
    }

    arr->data = tmp;
    arr->cap = new_cap;

    return 0;
}

int array_push(array *arr, const void *data) {
    if (arr == NULL) {
        return -1;
    }
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

int array_get(const array *arr, size_t index, void *value) {
    if (arr == NULL || value == NULL || index >= arr->len) {
        return -1;
    }

    const char *src = (const char *)arr->data + index * arr->data_size;

    memcpy(value, src, arr->data_size);
    return 0;
}

void array_destroy(array *arr) {
    if (arr != NULL) 
        free(arr->data);
    
    free(arr);
}
