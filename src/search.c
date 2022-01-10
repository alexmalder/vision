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
    struct array_t b;
    init_array(&b, tuple_count);
    // extract by range, extract by currency type
    //std::vector<double> target;
    for (uint64_t i = 0; i < tuple_count; i++) {
        if (fulldata[i].unix_val >= min_unix &&
            fulldata[i].unix_val < max_unix) {
            //target.push_back(fulldata[i].close);
            insert_array(&b, fulldata[i].close);
        }
    }
    //uint64_t resolution = 256;
    double thresh = 0.9955;
    //for (uint64_t x = 0; x < tuple_count; x++) {
    struct array_t a;
    init_array(&a, tuple_count);
    //if (x % resolution == 0) {
    uint64_t y;
    for (y = 0; y < tuple_count; y++) {
        insert_array(&a, fulldata[y].close);
        if (y % ssize == 0) {
            double similarity = cosine_similarity(a.array, b.array, ssize);
            if (similarity > thresh) {
                debug_array(&a, ssize);
                printf(" <<<--- [%lf] --->>> ", similarity);
                debug_array(&b, ssize);
                printf("\n\n");
            }
            free_array(&a);
            init_array(&a, tuple_count);
        }
    }
    //y += resolution;
    //}
    //}
    return 0;
}
