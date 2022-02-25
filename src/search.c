#include "vision.h"

/**
 * @brief vec_fill
 *
 * Model of operation
 * 1. Find tuples by unix_val range
 * 2. Insert founded values to a target array
 * 
 * @param cd crypto data array pointer
 * @param query real user input
 * @param tuple_count tarantool tuple count
 * @param target array for operation result
 * @return int operation state
 */
int vec_fill(struct crypto_t *cd, struct query_t *query, int tuple_count,
             struct array_t *target)
{
    for (uint64_t i = 0; i < tuple_count; i++) {
        if (cd[i].unix_val >= query->start_date &&
            cd[i].unix_val < query->end_date) {
            struct row_t *row_i = malloc(sizeof(struct row_t));
            row_i->unix_val = cd[i].unix_val;
            row_i->value = cd[i].close;
            insert_array(target, row_i);
        }
    }
    return 0;
}
/**
 * @brief vec_slide
 *
 * Model of operation
 * 1. Increment unix_val range by step size
 * 2. Find real data with incremented unix_val range
 * 3. Make vector stabilization with founded distance
 * 4. Write vector to a database
 * 5. Fill start_date and end_date values in result
 * 6. Return result
 *  
 * @param cd crypto data array pointer
 * @param result output example data structure
 * @param tuple_count tarantool tuple count 
 * @param target array for operation result
 * @return int operation state
 */
int vec_slide(struct crypto_t *cd, struct query_t *result, int tuple_count,
              struct array_t *target)
{
    return 0;
}

/**
 * @brief vec_similarity
 * 
 * @param a source array
 * @param b target array
 * @param end length of array
 * @return double similarity
 */
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

/**
 * @brief vec_distance
 * 
 * @param target array for distance extraction
 * @param end length of array
 * @return double 
 */
double vec_distance(struct row_t *target, uint64_t end)
{
    double distance;
    uint64_t i;
    for (i = 0; i < end; i++) {
        distance += target[i].value;
    }
    return (distance / end);
}

/**
 * @brief vec_stabilization
 * 
 * @param source array for stabilization by distance
 * @param end length of array
 * @param distance is factor
 * @return int operation state
 */
int vec_stabilization(struct row_t *source, uint64_t end, double distance)
{
    uint64_t i;
    for (i = 0; i < end; i++) {
        source[i].value *= distance;
    }
    return 0;
}

/**
 * @brief vec_merge
 * 
 * Model of operation:
 * - this is not default vector merge
 * 
 * @param source array is read only
 * @param target array is mutable
 * @param end length of array
 * @return int operation state
 */
int vec_merge(struct row_t *source, struct row_t *target, uint64_t end)
{
    double sum;
    uint64_t i;
    for (i = 0; i < end; i++) {
        sum = target[i].value + source[i].value;
        target[i].value = sum / 2;
    }
    return 0;
}

int search_similarity(struct query_t *query, struct query_t *result)
{
    // extract all
    struct crypto_t *cd = malloc(sizeof(struct crypto_t) * 4096);
    int tuple_count = select_crypto(query, cd);
    printf("select_crypto tuple_count: %d\n", tuple_count);

    // initialize parameters
    uint64_t day_unix = 86400; // one day in unix format: constant
    uint64_t interval = query->end_date - query->start_date; // get interval
    uint64_t ssize = interval / day_unix; // size of array

    // initialize arrays
    struct array_t source;
    init_array(&source, tuple_count);
    vec_fill(cd, query, tuple_count, &source);

    // extract by range, extract by currency type
    uint64_t resolution = 10;
    double thresh = 0.995;
    uint64_t x = 0;
    uint64_t sl = ssize;
    double source_distance = vec_distance(source.rows, ssize);
    double sim;
    while (x < tuple_count) {
        struct array_t target;
        init_array(&target, tuple_count);
        if (x % resolution == 0) {
            uint64_t y = x;
            while (y < tuple_count) {
                struct row_t *row_y = malloc(sizeof(struct row_t));
                row_y->unix_val = cd[y].unix_val;
                row_y->value = cd[y].close;
                insert_array(&target, row_y);
                if (y % sl == 0) {
                    sim = vec_similarity(target.rows, source.rows, ssize);
                    if (sim > thresh) {
                        double distance;
                        double target_distance =
                            vec_distance(target.rows, ssize);
                        if (source_distance > target_distance) {
                            distance = source_distance / target_distance;
                            vec_stabilization(target.rows, ssize, distance);
                        } else {
                            distance = target_distance / source_distance;
                            vec_stabilization(source.rows, ssize, distance);
                        }
                        //vec_merge(target.rows, source.rows, ssize);
                        debug_iteration(query, target.rows[0].unix_val,
                                        target.rows[ssize].unix_val, ssize, sl,
                                        distance, x, y, sim, source.rows,
                                        target.rows);

                        insert_result(query, &target, request_id);
                        break;
                    }
                    free_array(&target);
                    init_array(&target, tuple_count);
                }
                y++;
            }
            sl += resolution;
            x += resolution;
        }
        if (sim > thresh) {
            break;
        }
        x++;
        free_array(&target);
    }
    free_array(&source);
    free(cd);
    return 0;
}

void query_init(struct query_t *query, uint64_t searchio, uint64_t start_date,
                uint64_t end_date, uint64_t user_id)
{
    query->searchio = searchio;
    query->start_date = start_date;
    query->end_date = end_date;
    query->user_id = user_id;
}
