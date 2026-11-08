#include "dynamic_array/array.h"
#include <stdio.h>

int main() {
    array *arr = array_create(sizeof(int));
    int cap;
    cap = arr->cap;
    int i;
    for (i = 0; i < 40; i++) {
        int err = array_push(arr, &(int){i});
        if (err) {
            printf("array push error, probably realloc");
            return err;
        }
        if (arr->cap != cap) {
            printf("\n-------------------- \n");
            printf("array cap is changed, now %d \n", arr->cap);
            cap = arr->cap;
        }

        int *data = (int *)array_get(arr, i);
        printf("data: %d array_cap: %d array_len: %d \n", *data,
                arr->cap, arr->len);
    }
}
