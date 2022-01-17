#include "vision.h"
#include <stdint.h>

void query_init(struct query_t *query, uint64_t searchio, uint64_t start_date,
                uint64_t end_date, uint64_t user_id)
{
    query->searchio = searchio;
    query->start_date = start_date;
    query->end_date = end_date;
    query->user_id = user_id;
}

double vec_similarity(double *a, double *b, uint64_t end)
{
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
    for (uint64_t i = 0; i < end; i++) {
        dot += a[i] * b[i];
        denom_a += a[i] * a[i];
        denom_b += b[i] * b[i];
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b));
}

double vec_distance(double *target, uint64_t length)
{
    double distance;
    uint64_t i;
    for (i = 0; i < length; i++) {
        distance += target[i];
    }
    return (distance / length);
}

int vec_stabilization(double *source, uint64_t length, double distance)
{
    uint64_t i;
    for (i = 0; i < length; i++) {
        source[i] *= distance;
    }
    return 0;
}

int vec_merge(double *source, double *target, uint64_t length)
{
    uint64_t i;
    for (i = 0; i < length; i++) {
        source[i] = (source[i] + target[i]) / 2;
    }
    return 0;
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
    double source_distance = vec_distance(b.array, ssize);
    double sim;
    while (x < tuple_count) {
        struct array_t a;
        init_array(&a, tuple_count);
        if (x % resolution == 0) {
            uint64_t y = x;
            while (y < tuple_count) {
                insert_array(&a, cd[y].close);
                if (y % slide == 0) {
                    sim = vec_similarity(a.array, b.array, ssize);
                    if (sim > thresh) {
                        double distance;
                        double target_distance = vec_distance(a.array, ssize);
                        if (source_distance > target_distance) {
                            distance = source_distance / target_distance;
                            vec_stabilization(a.array, ssize, distance);
                        } else {
                            distance = target_distance / source_distance;
                            vec_stabilization(b.array, ssize, distance);
                            // return value from stabilization or nothing
                        }
                        //debug_iteration(a.array, b.array, sim, ssize, slide, distance, x, y);
                        //vec_merge(a.array, b.array, ssize);
                        debug_iteration(a.array, b.array, sim, ssize, slide, distance, x, y);
                        break;
                    }
                    free_array(&a);
                    init_array(&a, tuple_count);
                }
                y++;
            }
            slide += resolution;
            x += resolution;
        }
        if (sim > thresh) {
            break;
        }
        x++;
        free_array(&a);
    }
    free_array(&b);
    free(cd);
    return 0;
}
