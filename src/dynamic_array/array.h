#ifndef DYNAMIC_ARRAY
#define DYNAMIC_ARRAY


//this is for if the cap is reached
//this operand will be used in to
//expand cap
#define CAP_GROWTH +4
#define CAP_INITIAL 4

typedef struct {
  int cap;
  void *data;
  int data_size;

  int len;
} array;

array *array_create( int data_size);


int array_push(array *arr, const void *data);
void *array_get(const array *arr, int index);
void array_destroy(array *arr);

#endif
