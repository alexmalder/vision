#ifndef LIBVISION_H
#define LIBVISION_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <json-c/json.h>

/*
 * Library declarations
 */

struct crypto_data {
    uint64_t unix;
    char* datetime;
    char* symbol;
    double open;
    double high;
    double low;
    double close;
    double volume_original;
    double volume_usd;
};

struct query_t {
    const char *start_date;
    const char *end_date;
    const char *symbol;
    const char *field_name;
};

double cosine_similarity(double *a, double *b, uint64_t length);
void crypto_get();
void crypto_post();
void crypto_search();

#endif
