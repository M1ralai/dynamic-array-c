#include "array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

array *array_create(int cap, int data_size) {
  array *ret = malloc(sizeof *ret);
  ret->data = malloc(data_size * cap);
  ret->cap = cap;
  ret->len = 0;
  ret->data_size = data_size;

  return ret;
}


void array_push(array *arr, const void *data) {
  if (arr->len >= arr->cap)
    return;

  char *slot = (char *)arr->data + arr->data_size * arr->len;

  memcpy(slot, data, arr->data_size);
  arr->len++;
}

void *array_get(const array *arr, int index) {
  return (index >= 0 && index < arr->len)
             ? (char *)arr->data + index * arr->data_size
             : NULL;
}

void array_destroy(array *arr) {
    free(arr -> data);
    free(arr);
}


