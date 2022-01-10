#include "src/vision.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <msgpuck.h>

void initArray(Array *a, size_t initialSize)
{
    a->array = malloc(initialSize * sizeof(double));
    a->used = 0;
    a->size = initialSize;
}

void insertArray(Array *a, double element)
{
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size
    if (a->used == a->size) {
        a->size *= 2;
        a->array = realloc(a->array, a->size * sizeof(double));
    }
    a->array[a->used++] = element;
}

void freeArray(Array *a)
{
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

void debugArray(Array *a, int length)
{
    for (int i = 0; i < length; i++) {
        printf("[%f]", a->array[i]);
    }
    printf("\n");
}

int selector_test()
{
    // extract all
    struct query_t *q = malloc(sizeof(struct query_t));
    q->symbol = 2;
    q->start_date = 1417132800; // min unix btc
    q->end_date = 1639699200; // max unix btc
    struct crypto_t fd[4096];
    int tuple_count = tarantool_select(q, fd);
    printf("tarantool_select tuple_count: %d\n", tuple_count);

    // extract by query
    uint64_t min_unix = 1599004800; // 2020-09-02
    uint64_t max_unix = 1606694400; // 2020-11-30
    uint64_t day_unix = 86400; // one day in unix format
    uint64_t range = max_unix - min_unix; // get range
    uint64_t ssize = range / day_unix; // size of array
    printf("ssize: %lld\n", ssize); // print size of array

    // init arrays
    Array a;
    Array b;
    initArray(&a, ssize);
    initArray(&b, ssize);
    double items[ssize];
    // extract by range
    for (uint64_t i = 0; i < tuple_count; i++) {
        if (fd[i].unix_val >= min_unix && fd[i].unix_val <= max_unix) {
            //insertArray(&a, fd[i].close);
            items[i] = fd[i].close;
        }
    }
    debugArray(&a, ssize);
    uint64_t start = ssize;
    for (int i = 0; i < tuple_count; i++) {
        //printf("iter %d and close value: %f\n", i, fd[i].close);
        int iter = i - start;
        insertArray(&b, fd[i].close);
        //printf("%f ", fd[i].open);
        if (i % ssize == 0) {
            debugArray(&b, ssize);
            //printf("iter: %lld", i);
            double result = cosine_similarity(&a, &b, start, start + ssize);
            printf("result: %f\n", result);
            start += ssize;
            //break;
            //freeArray(&b);
            //initArray(&b, ssize);
            break;
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
    selector_test();
    zmq_listen();
    return 0;
}