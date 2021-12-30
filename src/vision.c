#include "vision.h"

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

int tarantool_insert()
{
    struct tnt_stream *tnt = tnt_net(NULL);
    const char connection[] = "tnt_user:tnt_password@192.168.238.128:3301";
    tnt_set(tnt, TNT_OPT_URI, connection);
    if (tnt_connect(tnt) < 0) {
        printf("Connection refused \n");
        //return 255;
    }
    struct tnt_stream *tuple = tnt_object(NULL);
    const char *format = "[%d%s%s%lf%lf%lf%lf%lf%lf]";
    tnt_object_format(tuple, format, 112, "2020-01-01", "BTC/USD", 0.11, 0.12,
                      0.13, 0.14, 0.111, 0.112);
    tnt_insert(tnt, 513, tuple);
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
