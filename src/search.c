#include "vision.h"

double cosine_similarity(double *a, double *b, uint64_t end)
{
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
    for (uint64_t i = 0; i < end; i++) {
        dot += a[i] * b[i];
        denom_a += a[i] * a[i];
        denom_b += b[i] * b[i];
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b));
}

typedef struct {
    double *array;
    size_t used;
    size_t size;
} Array;

void initArray(Array *a, size_t initialSize)
{
    a->array = malloc(initialSize * sizeof(double));
    a->used = 0;
    a->size = initialSize;
}

void insertArray(Array *a, int element)
{
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size
    if (a->used == a->size) {
        a->size *= 2;
        a->array = realloc(a->array, a->size * sizeof(int));
    }
    a->array[a->used++] = element;
}

void freeArray(Array *a)
{
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

void debugArray(Array *a, uint64_t length)
{
    printf("[");
    for (uint64_t i = 0; i < length; i++) {
        printf(" %lf ", a->array[i]);
    }
    printf("]");
}

int selector_test()
{
    // extract all
    struct query_t *q = malloc(sizeof(struct query_t));
    q->symbol = 2;
    //q->start_date = 1417132800; // min unix btc
    //q->end_date = 1639699200; // max unix btc
    struct crypto_t *fulldata = malloc(sizeof(struct crypto_t) * 4096);
    int tuple_count = tarantool_select(q, fulldata);
    printf("tarantool_select tuple_count: %d\n", tuple_count);
    // extract by query
    uint64_t min_unix = 1599004800; // 2020-09-02
    uint64_t max_unix = 1606694400; // 2020-11-30
    uint64_t day_unix = 86400; // one day in unix format
    uint64_t interval = max_unix - min_unix; // get interval
    uint64_t ssize = interval / day_unix; // size of array
    // initialize arrays
    Array a_target;
    initArray(&a_target, tuple_count);
    // extract by range, extract by currency type
    //std::vector<double> target;
    for (uint64_t i = 0; i < tuple_count; i++) {
        if (fulldata[i].unix_val >= min_unix &&
            fulldata[i].unix_val < max_unix) {
            //target.push_back(fulldata[i].close);
            insertArray(&a_target, fulldata[i].close);
        }
    }
    uint64_t search_count = 0;
    uint64_t insertion_count = 0;
    uint64_t resolution = 256;
    double thresh = 0.9955;
    for (uint64_t x = 0; x < tuple_count; x++) {
        Array a_source;
        initArray(&a_source, tuple_count);
        if (x % resolution == 0) {
            uint64_t y;
            for (y = 0; y < tuple_count; y++) {
                insertion_count += 1;
                insertArray(&a_source, fulldata[y].close);
                if (y % ssize == 0) {
                    double similarity = cosine_similarity(
                        a_source.array, a_target.array, ssize);
                    search_count += 1;
                    if (similarity > thresh) {
                        debugArray(&a_source, ssize);
                        printf(" <<<--- [%lf] --->>> ", similarity);
                        debugArray(&a_target, ssize);
                        printf("\n\n");
                        break;
                    }
                    freeArray(&a_source);
                    initArray(&a_source, tuple_count);
                }
            }
            y += resolution;
        }
    }
    printf("--- search count: %lld ---\n", search_count);
    printf("--- insertion count: %lld ---\n", insertion_count);
    return 0;
}
