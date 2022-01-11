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

struct array_t {
    double *array;
    size_t used;
    size_t size;
};

struct crypto_t {
    uint64_t unix_val;
    char *datetime;
    uint64_t symbol;
    double open;
    double high;
    double low;
    double close;
    double volume_original;
    double volume_usd;
};

struct query_t {
    uint64_t start_date;
    uint64_t end_date;
    uint64_t symbol;
};

// insert data into tarantool
int tarantool_insert(struct crypto_t *cd);
// select data from tarantool
int tarantool_select(struct query_t *query, struct crypto_t *cd);
// 0mq
int zmq_listen();
// tests
int selector_test();
// array
void init_array(struct array_t *a, size_t initial_size);
void insert_array(struct array_t *a, int element);
void free_array(struct array_t *a);
void debug_array(struct array_t *a, uint64_t length);

#endif