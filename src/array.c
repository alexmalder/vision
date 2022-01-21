#include "vision.h"

void init_array(struct array_t *a, size_t initial_size)
{
        a->array = malloc(initial_size * sizeof(double));
        a->used = 0;
        a->size = initial_size;
}

void insert_array(struct array_t *a, double element)
{
        // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
        // Therefore a->used can go up to a->size
        if (a->used == a->size) {
                a->size *= 2;
                a->array = realloc(a->array, a->size * sizeof(double));
        }
        a->array[a->used++] = element;
}

void free_array(struct array_t *a)
{
        free(a->array);
        a->array = NULL;
        a->used = a->size = 0;
}