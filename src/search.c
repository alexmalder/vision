#include "vision.h"

void query_init(struct query_t *query, uint64_t start_date, uint64_t end_date,
                uint64_t searchio, uint64_t user_id)
{
    query->start_date = start_date;
    query->end_date = end_date;
    query->searchio = searchio;
    query->user_id = user_id;
}

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

int search_similarity(struct query_t *query)
{
    // extract all
    struct crypto_t *cd = malloc(sizeof(struct crypto_t) * 4096);
    int tuple_count = tarantool_select(query, cd);
    printf("tarantool_select tuple_count: %d\n", tuple_count);
    // initialize parameters

    uint64_t day_unix = 86400; // one day in unix format: constant
    uint64_t interval = query->end_date - query->start_date; // get interval
    uint64_t ssize = interval / day_unix; // size of array
    // initialize arrays
    struct array_t b;
    init_array(&b, tuple_count);
    // extract by range, extract by currency type
    for (uint64_t i = 0; i < tuple_count; i++) {
        if (cd[i].unix_val >= query->start_date &&
            cd[i].unix_val < query->end_date) {
            insert_array(&b, cd[i].close);
        }
    }
    uint64_t resolution = 10;
    double thresh = 0.998;
    uint64_t x = 0;
    uint64_t slide = ssize;
    while (x < tuple_count) {
        struct array_t a;
        init_array(&a, tuple_count);
        if (x % resolution == 0) {
            uint64_t y = x;
            while (y < tuple_count) {
                insert_array(&a, cd[y].close);
                if (y % slide == 0) {
                    double sim = cosine_similarity(a.array, b.array, ssize);
                    if (sim > thresh) {
                        debug_array(&a, ssize);
                        printf(" <<<--- [%lf] --->>> ", sim);
                        debug_array(&b, ssize);
                        printf("\nslide: %lld\nx: %lld\n", slide, x);
                        printf("\n\n");
                    }
                    free_array(&a);
                    init_array(&a, tuple_count);
                }
                //printf(" [%lld] ", y);
                y++;
            }
            //printf("\n");
            slide += resolution;
            x += resolution;
        }
        x++;
    }
    return 0;
}
