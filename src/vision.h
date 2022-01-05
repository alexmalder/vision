#ifndef LIBVISION_H
#define LIBVISION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <signal.h>
#include <tarantool/tarantool.h>
#include <tarantool/tnt_net.h>
#include <tarantool/tnt_opt.h>
#include <zmq.h>

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

double cosine_similarity(double *a, double *b, uint64_t length);
int cosine_similarity_test();
void crypto_get();
void crypto_post();
void crypto_search();
int tarantool_insert(struct crypto_t *cd);
int tarantool_select(struct query_t *query, struct crypto_t *cd);

#endif