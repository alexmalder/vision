#include "vision.h"
#include <stdio.h>
#include <msgpuck.h>

#define MP_SOURCE 1
#define SPACE_ID 520

double cosine_similarity(double *a, double *b, uint64_t length)
{
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
    for (uint64_t i = 0; i < length; i++) {
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

int tarantool_insert(struct crypto_data *cd)
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
    tnt_object_format(tuple, format, cd->unix, cd->datetime, cd->symbol,
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

int tarantool_select(struct query_t *query)
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
    tnt_object_format(tuple, "[%s]", "BTC/USD");
    tnt_select(tnt, 520, 0, 1024, 0, 0, tuple);
    tnt_flush(tnt);
    struct tnt_reply reply;
    tnt_reply_init(&reply);
    tnt->read_reply(tnt, &reply);
    if (reply.code != 0) {
        printf("Select failed.\n");
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
        if (field_type != MP_ARRAY) { printf("no field array\n"); exit(1); }
        uint32_t field_count = mp_decode_array(&reply.data);
        printf("  field count=%u\n", field_count);
        for (j = 0; j < 4; ++j) {
            field_type = mp_typeof(*reply.data);
            if (field_type == MP_UINT) {
                unsigned long num_value = mp_decode_uint(&reply.data);
                printf("    uint value=%lu.\n", num_value);
            } else if (field_type == MP_STR) {
                const char *str_value;
                uint32_t str_value_length;
                str_value = mp_decode_str(&reply.data, &str_value_length);
                printf("    str value=%.*s.\n", str_value_length, str_value);
            } else if (field_type == MP_DOUBLE) {
                double double_value;
                double_value = mp_decode_double(&reply.data);
                printf("    double value=%lf.\n", double_value);
            } else {
                printf("wrong field type\n");
                exit(1);
            }
        }
    }
    fflush(stdout);
    tnt_close(tnt);
    tnt_stream_free(tuple);
    tnt_stream_free(tnt);
}
