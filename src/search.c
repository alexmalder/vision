#include "../include/vision.h"

static uint64_t resolution = 10;
static double thresh = 0.995;

int vec_fill(crypto_t *source, query_t *query, int tuple_count, array_t *target)
{
    for (uint64_t i = 0; i < tuple_count; i++) {
        if (source[i].unix_val >= query->start_date &&
            source[i].unix_val < query->end_date) {
            row_t *row_i = malloc(sizeof(row_t));
            row_i->unix_val = source[i].unix_val;
            row_i->value = source[i].close;
            insert_array(target, row_i);
        }
    }
    return 0;
}

int vec_slide(crypto_t *cd, query_t *result, int tuple_count, array_t *target)
{
    return 0;
}

double vec_similarity(row_t *a, row_t *b, uint64_t end)
{
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
    for (uint64_t i = 0; i < end; i++) {
        dot += a[i].value * b[i].value;
        denom_a += a[i].value * a[i].value;
        denom_b += b[i].value * b[i].value;
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b));
}

double vec_distance(row_t *target, uint64_t end)
{
    double distance;
    uint64_t i;
    for (i = 0; i < end; i++) {
        distance += target[i].value;
    }
    return (distance / end);
}

int vec_stabilization(row_t *source, uint64_t end, double distance)
{
    uint64_t i;
    for (i = 0; i < end; i++) {
        source[i].value *= distance;
    }
    return 0;
}

int vec_merge(row_t *source, row_t *target, uint64_t end)
{
    double sum;
    uint64_t i;
    for (i = 0; i < end; i++) {
        sum = target[i].value + source[i].value;
        target[i].value = sum / 2;
    }
    return 0;
}

uint64_t calculate_size(query_t *query)
{
    uint64_t day_unix = 86400; // one day in unix format: constant
    uint64_t interval = query->end_date - query->start_date; // get interval
    uint64_t ssize = interval / day_unix; // size of array
    return ssize;
}

int vec_search(query_t *query, query_t *result)
{
    // extract all
    crypto_t *cd = malloc(sizeof(crypto_t) * 4096);
    int tuple_count = select_crypto(query, cd);
    uint64_t ssize = calculate_size(query);
    // extract by range, extract by currency type
    uint64_t x = 0;
    uint64_t ssize_fork = ssize;
    double sim;
    while (x < tuple_count) {
        // src
        array_t source;
        init_array(&source, tuple_count);
        vec_fill(cd, query, tuple_count, &source);
        double src_dist = vec_distance(source.rows, ssize);
        // dest
        array_t target;
        init_array(&target, tuple_count);
        // iter
        if (x % resolution == 0) {
            uint64_t y = x;
            while (y < tuple_count) {
                row_t *row_y = malloc(sizeof(row_t));
                row_y->unix_val = cd[y].unix_val;
                row_y->value = cd[y].close;
                insert_array(&target, row_y);
                if (y % ssize_fork == 0) {
                    sim = vec_similarity(target.rows, source.rows, ssize);
                    if (sim > thresh) {
                        double distance;
                        double dest_dis = vec_distance(target.rows, ssize);
                        if (src_dist > dest_dis) {
                            distance = src_dist / dest_dis;
                            vec_stabilization(target.rows, ssize, distance);
                        } else {
                            distance = dest_dis / src_dist;
                            vec_stabilization(source.rows, ssize, distance);
                        }
                        //vec_merge(target.rows, source.rows, ssize);
                        debug_iteration(query, target.rows[0].unix_val, target.rows[ssize].unix_val, ssize, ssize_fork, distance, x, y, sim, source.rows, target.rows);

                        //insert_result(query, &target, request_id);
                        //break;
                    }
                    free_array(&source), init_array(&source, tuple_count);
                    free_array(&target), init_array(&target, tuple_count);
                }
                y++;
            }
            ssize_fork += resolution;
            x += resolution;
        }
        //if (sim > thresh) { break; }
        x++;
        free_array(&source);
        free_array(&target);
    }
    free(cd);
    return 0;
}

void query_init(query_t *query, uint64_t searchio, uint64_t start_date,
                uint64_t end_date, uint64_t user_id)
{
    query->searchio = searchio;
    query->start_date = start_date;
    query->end_date = end_date;
    query->user_id = user_id;
}
