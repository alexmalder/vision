#include "vision.h"
#include <stdint.h>
#include <stdlib.h>

void query_init(struct query_t *query, uint64_t searchio, uint64_t start_date, uint64_t end_date, uint64_t user_id)
{
    query->searchio = searchio;
    query->start_date = start_date;
    query->end_date = end_date;
    query->user_id = user_id;
}

void debug_iteration(uint64_t ssize, uint64_t slide, double distance, uint64_t x, uint64_t y, double sim, struct row_t *a, struct row_t *b)
{
    char buffer[4096 * 3];
    char source_buffer[4096];
    sprintf(source_buffer, "[ ");
    for (uint64_t i = 0; i < ssize; i++) {
        if (i == ssize - 1) {
            sprintf(source_buffer + strlen(source_buffer), "%lf", a[i].value);
        } else {
            sprintf(source_buffer + strlen(source_buffer), "%lf,", a[i].value);
        }
    }
    sprintf(source_buffer + strlen(source_buffer), "]");
    char target_buffer[4096];
    sprintf(target_buffer, "[");
    for (uint64_t i = 0; i < ssize; i++) {
        if (i == ssize - 1) {
            sprintf(target_buffer + strlen(target_buffer), "%lf", a[i].value);
        } else {
            sprintf(target_buffer + strlen(target_buffer), "%lf,", a[i].value);
        }
    }
    sprintf(target_buffer + strlen(target_buffer), "]");
    sprintf(
        buffer,
        "{\"ssize\": %lld, \"slide\": %lld, \"distance\": %lf, \"x\": %lld, \"y\": %lld, \"similarity\": %lf, \"source\": %s, \"target\": %s}\n",
        ssize, slide, distance, x, y, sim, source_buffer, target_buffer);
    int status = produce(buffer);
    printf("%s\n", buffer);
    printf("[status: %d]\n", status);
}

double vec_similarity(struct row_t *a, struct row_t *b, uint64_t end)
{
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
    for (uint64_t i = 0; i < end; i++) {
        dot += a[i].value * b[i].value;
        denom_a += a[i].value * a[i].value;
        denom_b += b[i].value * b[i].value;
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b));
}

double vec_distance(struct row_t *target, uint64_t length)
{
    double distance;
    uint64_t i;
    for (i = 0; i < length; i++) {
        distance += target[i].value;
    }
    return (distance / length);
}

int vec_stabilization(struct row_t *source, uint64_t length, double distance)
{
    uint64_t i;
    for (i = 0; i < length; i++) {
        source[i].value *= distance;
    }
    return 0;
}

int vec_merge(struct row_t *source, struct row_t *target, uint64_t length)
{
    double sum;
    uint64_t i;
    for (i = 0; i < length; i++) {
        sum = source[i].value + target[i].value;
        source[i].value = sum / 2;
    }
    return 0;
}

int search_similarity(struct query_t *query)
{
    // generate request_id
    uint64_t request_id = (unsigned long)time(NULL);
    // extract all
    struct crypto_t *cd = malloc(sizeof(struct crypto_t) * 4096);
    int tuple_count = select_crypto(query, cd);
    printf("select_crypto tuple_count: %d\n", tuple_count);
    // initialize result
    //struct result_t *result = malloc(sizeof(struct result_t));

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
            struct row_t *row_i = malloc(sizeof(struct row_t));
            row_i->unix_val=cd[i].unix_val;
            row_i->value = cd[i].close;
            insert_array(&b, row_i);
        }
    }
    uint64_t resolution = 10;
    double thresh = 0.998;
    uint64_t x = 0;
    uint64_t slide = ssize;
    double source_distance = vec_distance(b.rows, ssize);
    double sim;
    while (x < tuple_count) {
        struct array_t a;
        init_array(&a, tuple_count);
        if (x % resolution == 0) {
            uint64_t y = x;
            while (y < tuple_count) {
                struct row_t *row_y = malloc(sizeof(struct row_t));
                row_y->unix_val=cd[y].unix_val;
                row_y->value = cd[y].close;
                insert_array(&a, row_y);
                if (y % slide == 0) {
                    sim = vec_similarity(a.rows, b.rows, ssize);
                    if (sim > thresh) {
                        double distance;
                        double target_distance = vec_distance(a.rows, ssize);
                        if (source_distance > target_distance) {
                            distance = source_distance / target_distance;
                            vec_stabilization(a.rows, ssize, distance);
                        } else {
                            distance = target_distance / source_distance;
                            vec_stabilization(b.rows, ssize, distance);
                        }
                        //vec_merge(a.array, b.array, ssize);
                        debug_iteration(ssize, slide, distance, x, y, sim, a.rows, b.rows);
                        insert_result(query, &a, request_id);
                    }
                    free_array(&a);
                    init_array(&a, tuple_count);
                }
                y++;
            }
            slide += resolution;
            x += resolution;
        }
        //if (sim > thresh) {}
        x++;
        free_array(&a);
    }
    free_array(&b);
    free(cd);
    return 0;
}
