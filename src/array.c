#include "../include/vision.h"

void init_array(array_t *a, size_t initial_size)
{
    a->rows = malloc(initial_size * sizeof(row_t));
    a->used = 0;
    a->size = initial_size;
}

void insert_array(array_t *a, row_t *row)
{
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size
    if (a->used == a->size) {
        a->size *= 2;
        a->rows = realloc(a->rows, a->size * sizeof(row_t));
    }
    a->rows[a->used++] = *row;
}

void free_array(array_t *a)
{
    free(a->rows);
    a->rows = NULL;
    a->used = a->size = 0;
}
