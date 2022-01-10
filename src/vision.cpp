#include "vision.hpp"
#include <cstdio>
#include <iostream>
#include <vector>

#define MP_SOURCE 1

static int SPACE_ID = 512;

double cosine_similarity(std::vector<double> a, std::vector<double> b,
                         uint64_t end)
{
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
    for (uint64_t i = 0; i < end; i++) {
        dot += a[i] * b[i];
        denom_a += a[i] * a[i];
        denom_b += b[i] * b[i];
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b));
}

int init_conn_str(char *conn_string)
{
    char *tnt_user = getenv("TNT_USER");
    char *tnt_pass = getenv("TNT_PASSWORD");
    char *tnt_host = getenv("TNT_HOST");
    char *tnt_port = getenv("TNT_PORT");
    sprintf(conn_string, "%s:%s@%s:%s", tnt_user, tnt_pass, tnt_host, tnt_port);
    return 0;
}

int tarantool_insert(struct crypto_t *cd)
{
    struct tnt_stream *tnt = tnt_net(NULL);
    char conn_string[128];
    init_conn_str(conn_string);

    tnt_set(tnt, TNT_OPT_URI, conn_string);
    if (tnt_connect(tnt) < 0) {
        printf("Connection refused \n");
        return -1;
    }
    const char *format = "[%d%s%s%lf%lf%lf%lf%lf%lf]";
    struct tnt_stream *tuple = tnt_object(NULL);
    tnt_object_format(tuple, format, cd->unix_val, cd->datetime, cd->symbol,
                      cd->open, cd->high, cd->low, cd->close,
                      cd->volume_original, cd->volume_usd);
    tnt_insert(tnt, SPACE_ID, tuple);
    tnt_flush(tnt);
    struct tnt_reply reply;
    tnt_reply_init(&reply);
    tnt->read_reply(tnt, &reply);
    if (reply.code != 0) {
        printf("Insert failed %llu.\n", reply.code);
    }
    printf("Tuple inserted with code %llu.\n", reply.code);
    fflush(stdout);
    tnt_close(tnt);
    tnt_stream_free(tuple);
    tnt_stream_free(tnt);
    return 0;
}

int tarantool_select(struct query_t *query, struct crypto_t *cd)
{
    struct tnt_stream *tnt = tnt_net(NULL);
    char conn_string[128];
    init_conn_str(conn_string);
    tnt_set(tnt, TNT_OPT_URI, conn_string);
    if (tnt_connect(tnt) < 0) {
        printf("Connection refused\n");
        exit(1);
    }
    struct tnt_stream *tuple = tnt_object(NULL);
    tnt_object_format(tuple, "[%lld]", query->symbol);
    tnt_select(tnt, SPACE_ID, 0, 1048576, 0, 0, tuple);
    tnt_flush(tnt);
    struct tnt_reply reply;
    tnt_reply_init(&reply);
    tnt->read_reply(tnt, &reply);
    if (reply.code != 0) {
        printf("Select failed with status code %lld.\n", reply.code);
        exit(1);
    }
    char field_type;
    field_type = mp_typeof(*reply.data);
    if (field_type != MP_ARRAY) {
        printf("no tuple array\n");
        exit(1);
    }
    long unsigned int row_count;
    uint32_t tuple_count = mp_decode_array(&reply.data);
    printf("tuple count=%u\n", tuple_count);
    unsigned int i, j;
    for (i = 0; i < tuple_count; ++i) {
        field_type = mp_typeof(*reply.data);
        if (field_type != MP_ARRAY) {
            printf("no field array\n");
        }
        uint32_t field_count = mp_decode_array(&reply.data);

        unsigned long unix_val = mp_decode_uint(&reply.data);
        cd[i].unix_val = unix_val;

        const char *str_value;
        uint32_t str_value_length;
        str_value = mp_decode_str(&reply.data, &str_value_length);
        //cd[i].datetime = str_value;

        unsigned long symbol = mp_decode_uint(&reply.data);
        cd[i].symbol = symbol;

        double open = mp_decode_double(&reply.data);
        cd[i].open = open;

        double high = mp_decode_double(&reply.data);
        cd[i].high = high;

        double low = mp_decode_double(&reply.data);
        cd[i].low = low;

        double close = mp_decode_double(&reply.data);
        cd[i].close = close;

        double volume_original = mp_decode_double(&reply.data);
        cd[i].volume_original = volume_original;

        double volume_usd = mp_decode_double(&reply.data);
        cd[i].volume_usd = volume_usd;
    }
    tnt_close(tnt);
    tnt_stream_free(tuple);
    tnt_stream_free(tnt);
    return tuple_count;
}

int selector_test()
{
    // extract all
    struct query_t *q = new query_t();
    q->symbol = 2;
    //q->start_date = 1417132800; // min unix btc
    //q->end_date = 1639699200; // max unix btc
    struct crypto_t fd[4096];
    int tuple_count = tarantool_select(q, fd);
    printf("tarantool_select tuple_count: %d\n", tuple_count);
    // extract by query
    uint64_t min_unix = 1599004800; // 2020-09-02
    uint64_t max_unix = 1606694400; // 2020-11-30
    uint64_t day_unix = 86400; // one day in unix format
    uint64_t interval = max_unix - min_unix; // get interval
    uint64_t ssize = interval / day_unix; // size of array
    // extract by range, extract by currency type
    std::vector<double> target;
    for (uint64_t i = 0; i < tuple_count; i++) {
        if (fd[i].unix_val >= min_unix && fd[i].unix_val < max_unix) {
            target.push_back(fd[i].close);
        }
    }
    uint64_t start = 0;
    uint64_t search_count = 0;
    uint64_t resolution = 10;
    double thresh = 0.9966;
    for (uint64_t x = 0; x < tuple_count; x++) {
        std::vector<double> source;
        if (x % resolution == 0) {
            for (uint64_t y = start; y < tuple_count; y++) {
                source.push_back(fd[y].close);
                if (y % ssize == 0) {
                    double similarity = cosine_similarity(source, target, ssize);
                    search_count += 1;
                    if (similarity > thresh) {
                        printf("--- similarity: %lf ---\n", similarity);
                    }
                    source.clear();
                }
            }
            start += resolution;
        }
    }
    printf("--- search count: %lld ---\n", search_count);
    return 0;}
