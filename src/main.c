#include "dynamic_array/array.h"
#include <stdio.h>

int main() {
    array *arr = array_create(sizeof(int));
    if(arr == NULL) {
        printf("error when array creating");
        return -1;
    }
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
            printf("array cap is changed, now %zu \n", arr->cap);
            cap = arr->cap;
        }
        int data;
        array_get(arr, i, &data);
        printf("data: %d array_cap: %zu array_len: %zu \n", data,
                arr->cap, arr->len);
    }
    array_destroy(arr);
}
