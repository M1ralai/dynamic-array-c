#include "../dynamic_array/array.h"
#include <assert.h>
#include <stdio.h>



void test_array_integrity(void) {
    array *arr = array_create(sizeof(int));
    assert(arr != NULL);

    const int n = 10000;

    for (int i = 0; i < n; i++) {
        assert(array_push(arr, &i) == 0);
    }

    assert(arr->len == n);

    for (int i = 0; i < n; i++) {
        int result = -1;

        assert(array_get(arr, i, &result) == 0);
        assert(result == i);
    }

    array_destroy(arr);
}

void test_array_cap(void) {
    array *arr =array_create(sizeof(int));
    assert(arr->cap == CAP_INITIAL);
    for (int i = 0; i <= CAP_INITIAL; i++) {
        array_push(arr, &(int){i});
    }
    assert(arr->cap == CAP_INITIAL CAP_GROWTH);
    array_destroy(arr);
    arr = array_create(sizeof(int));
    for (int i = 0; i <= CAP_INITIAL CAP_GROWTH; i++) {
        array_push(arr, &(int){i});
    }
    assert(arr->cap == CAP_INITIAL CAP_GROWTH CAP_GROWTH);
    array_destroy(arr);
}


void test_array_get(void) {
    array *arr = array_create(sizeof(int));
    array_push(arr, &(int){32});
    assert(arr->len == 1);
    assert(arr->cap == CAP_INITIAL);
    int result;
    array_get(arr, 0,&result);
    assert(result == 32);

    int err = array_get(arr, 1, &result);
    assert(err == -1);

    array_destroy(arr); 
}


void test_array_push(void) {
    array *arr = array_create(sizeof(int));
    array_push(arr, &(int){32});
    assert(arr->len == 1);
    assert(arr->cap == CAP_INITIAL);
    array_destroy(arr); 
}

void test_array_create(void) {
    array *arr = array_create(sizeof(int));

    assert(arr != NULL);
    assert(arr->len == 0);
    assert(arr->cap == CAP_INITIAL);
    assert(arr->data_size == sizeof(int));

    array_destroy(arr);

    arr = array_create(0);
    assert(arr == NULL);
    array_destroy(arr);
}

int main(void) {
    test_array_create();
    test_array_push();
    test_array_get();
    test_array_cap();
    test_array_integrity();

    printf("All tests passed!\n");
    return 0;
}

