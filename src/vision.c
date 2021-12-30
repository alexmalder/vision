#include "vision.h"
#include <stdio.h>

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

int tarantool_insert(struct crypto_data *cd, uint64_t length)
{
    struct tnt_stream *tnt = tnt_net(NULL);
    char conn_string[128];
    char *tnt_user = getenv("TNT_USER");
    char *tnt_pass = getenv("TNT_PASSWORD");
    char *tnt_host = getenv("TNT_HOST");
    char *tnt_port = getenv("TNT_PORT");
    sprintf(conn_string, "%s:%s@%s:%s", tnt_user, tnt_pass, tnt_host, tnt_port);

    tnt_set(tnt, TNT_OPT_URI, conn_string);
    if (tnt_connect(tnt) < 0) {
        printf("Connection refused \n");
        return -1;
    }
    const char *format = "[%d%s%s%lf%lf%lf%lf%lf%lf]";
    for (uint64_t i = 0; i < length; i++) {
        struct tnt_stream *tuple = tnt_object(NULL);
        tnt_object_format(tuple, format, cd[i].unix, cd[i].datetime,
                          cd[i].symbol, cd[i].open, cd[i].high, cd[i].low,
                          cd[i].close, cd[i].volume_original, cd[i].volume_usd);
        tnt_insert(tnt, 512, tuple);
        //tnt_stream_free(tuple);
    }
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
    tnt_stream_free(tnt);
    return 0;
}
