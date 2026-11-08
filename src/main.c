#include "dynamic_array/array.h"
#include <stdio.h>

int main() {
  array *arr = array_create(32, sizeof(int));

  array_push(arr, &(int){32});
  array_push(arr, &(int){33});
  array_push(arr, &(int){34});
  array_push(arr, &(int){35});

  void *one = array_get(arr, 0);
  printf("%p \n", one);
  printf("%d \n", *(int *)one);

  void *two = array_get(arr, 1);
  printf("%p \n", two);
  printf("%d \n", *(int *)two);

  void *three = array_get(arr, 2);
  printf("%p \n", three);
  printf("%d \n", *(int *)three);
}
