#ifndef LIBVISION_H
#define LIBVISION_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <json-c/json.h>
#include <tarantool/tarantool.h>
#include <tarantool/tnt_net.h>
#include <tarantool/tnt_opt.h>
#include "httpserver.h"

struct crypto_data {
    uint64_t unix;
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
int tarantool_insert(struct crypto_data *cd);
int tarantool_select(struct query_t *query, struct crypto_data *cd);
int request_target_is(struct http_request_s *request, char const *target);
void handle_request(struct http_request_s *request);
void handle_sigterm(int signum);

#endif
