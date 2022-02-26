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
int insert_result(query_t *query, array_t *array, uint64_t request_id);
int select_crypto(query_t *query, crypto_t *cd);
int delete_result(query_t *query);

// 0mq
int zmq_listen();

// search engine
void query_init(query_t *query, uint64_t searchio, uint64_t start_date,
                uint64_t end_date, uint64_t user_id);
int search_similarity(query_t *query, query_t *result);

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