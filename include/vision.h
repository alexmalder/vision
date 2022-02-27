//@file vision.h
#ifndef LIBVISION_H
#define LIBVISION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <tarantool/tarantool.h>
#include <tarantool/tnt_net.h>
#include <tarantool/tnt_opt.h>
#include <zmq.h>
#include <msgpuck.h>
#include <librdkafka/rdkafka.h>

typedef struct {
    uint64_t unix_val;
    double value;
} row_t;

typedef struct {
    row_t *rows;
    size_t used;
    size_t size;
} array_t;

typedef struct {
    uint64_t unix_val;
    char *datetime;
    uint64_t symbol;
    double open;
    double high;
    double low;
    double close;
    double volume_original;
    double volume_usd;
} crypto_t;

typedef struct {
    uint64_t searchio;
    uint64_t start_date;
    uint64_t end_date;
    uint64_t user_id;
} query_t;

// tarantool
int insert_result(char *conn_str, query_t *query, array_t *array,
                  uint64_t request_id);
int select_crypto(char *conn_str, query_t *query, crypto_t *cd);
int delete_result(char *conn_str, query_t *query);

// 0mq
int zmq_listen(char *conn_str);

// search engine
void query_init(query_t *query, uint64_t searchio, uint64_t start_date,
                uint64_t end_date, uint64_t user_id);
/**
 * @brief vec_fill
 *
 * Model of operation
 * 1. Find tuples by unix_val range
 * 2. Insert founded values to a target array
 * 
 * @param source crypto data array pointer
 * @param query real user input
 * @param tuple_count tarantool tuple count
 * @param target array for operation result
 * @return int operation state
 */
int vec_fill(crypto_t *source, query_t *query, int tuple_count,
             array_t *target);
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
int vec_slide(crypto_t *cd, query_t *result, int tuple_count, array_t *target);
/**
 * @brief vec_similarity
 * 
 * @param a source array
 * @param b target array
 * @param end length of array
 * @return double similarity
 */
double vec_similarity(row_t *a, row_t *b, uint64_t end);
/**
 * @brief vec_distance
 * 
 * @param target array for distance extraction
 * @param end length of array
 * @return double 
 */
double vec_distance(row_t *target, uint64_t end);
/**
 * @brief vec_stabilization
 * 
 * @param source array for stabilization by distance
 * @param end length of array
 * @param distance is factor
 * @return int operation state
 */
int vec_stabilization(row_t *source, uint64_t end, double distance);
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
int vec_merge(row_t *source, row_t *target, uint64_t end);
/**
 * @brief vec_search
 * 
 * Description:
 * - this is high-level function
 * - used by zeromq or run manually for tests
 *
 * @param query by user request
 * @param result by system response
 * @return int operation state
 */
int vec_search(char *conn_str, query_t *query);

// array
void init_array(array_t *source, size_t initial_size);
void insert_array(array_t *source, row_t *rows);
void free_array(array_t *source);
void debug_iteration(query_t *query, uint64_t founded_start_date,
                     uint64_t founded_end_date, uint64_t ssize, uint64_t slide,
                     double distance, uint64_t x, uint64_t y, double sim,
                     row_t *source, row_t *target);

// log ops
int produce(char *buf);

#endif
