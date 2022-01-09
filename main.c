#include "src/vision.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <msgpuck.h>

int cosine_similarity_test()
{
    double a[] = { 0.11, 0.12, 0.13 };
    double b[] = { 0.11, 0.12, 0.14 };
    double r = cosine_similarity(a, b, 0, 3);
    printf("cosine_similarity test result: %lf\n", r);
    fflush(stdout);
    return 0;
}

int selector_test()
{
    // extract all
    struct query_t *q = malloc(sizeof(struct query_t));
    q->symbol = 2;
    q->start_date = 1417132800; // min unix btc
    q->end_date = 1639699200; // max unix btc
    struct crypto_t *fd = malloc(sizeof(struct crypto_t) * 4096);
    int tuple_count = tarantool_select(q, fd);
    printf("tarantool_select tuple_count: %d\n", tuple_count);

    uint64_t denom = 128;
    uint64_t start = 0;
    uint64_t end = tuple_count - denom;
    for (uint64_t i = 0; i < tuple_count; i++) {
        if (i % denom == 0) {
            double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
            for (uint64_t x = start; x < start + denom; x++) {
                dot += fd[x].close * fd[x + denom].close;
                denom_a += fd[x].close * fd[x].close;
                denom_b += fd[x + denom].close * fd[x + denom].close;
            }
            double result = dot / (sqrt(denom_a) * sqrt(denom_b));
            if (result > 0.98) {
                printf("result is: %f for start: %lld and end : %lld\n", result,
                       i, start + denom);
            }
            start += denom, end += denom;
        }
    }
    return 0;
}

int zmq_listen()
{
    void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP);
    int rc = zmq_bind(responder, "tcp://*:5555");

    while (1) {
        char buffer[128];
        zmq_recv(responder, buffer, 128, 0);
        printf("decode message...\n");
        const char *r = buffer;
        uint32_t tuple_count;
        //double value;
        tuple_count = mp_decode_array(&r);
        for (int i = 0; i < tuple_count; i++) {
            double val = mp_decode_double(&r);
            printf("iter: %d, val: %lf\n", i, val);
        }

        //uint32_t size = mp_decode_map(&r);
        //printf("size of map: %d\n", size);
        //for (uint32_t i = 0; i < size; i++) {
        //    uint32_t key_len = 3;
        //    uint32_t *mylen = &key_len;
        //    const char *key = mp_decode_str(&r, mylen);
        //    double val = mp_decode_double(&r);
        //    printf("key: %s, val: %lf\n", key, val);
        //}
        printf("received message with tuple_count <%d>\n", tuple_count);
        sleep(1);
        zmq_send(responder, "ok", 5, 0);
    }
    return 0;
}

int zmq_publish()
{
    printf("connecting to hello world server...\n");
    void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        char buffer[128];
        printf("sending message %d...", request_nbr);
        zmq_send(requester, "hello", 5, 0);
        zmq_recv(requester, buffer, 10, 0);
        printf("received message %d\n", request_nbr);
    }
    zmq_close(requester);
    zmq_ctx_destroy(context);
    return 0;
}

int mp_print()
{
    printf(
        "MP_DOUBLE: %d, MP_ARRAY: %d, MP_FLOAT: %d, MP_UINT: %d, MP_BOOL: %d, MP_EXT: %d, MP_BIN: %d, MP_MAP: %d\n",
        MP_DOUBLE, MP_ARRAY, MP_FLOAT, MP_UINT, MP_BOOL, MP_EXT, MP_BIN,
        MP_MAP);
    return 0;
}

int main()
{
    cosine_similarity_test();
    selector_test();
    zmq_listen();
    return 0;
}
