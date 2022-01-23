#include "vision.h"
#include <stdlib.h>

static bool consumer_active = 1;

int zmq_publish()
{
    printf("connecting to hello world server...\n");
    void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");
    char recv[128];
    printf("sending message...\n");
    zmq_send(requester, "hello", 5, 0);
    zmq_recv(requester, recv, 10, 0);
    printf("received message...\n");
    zmq_close(requester);
    zmq_ctx_destroy(context);
    return 0;
}

int zmq_listen()
{
    void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP);
    int rc = zmq_bind(responder, "tcp://*:5555");
    while (consumer_active) {
        char buffer[128];
        zmq_recv(responder, buffer, 128, 0);
        printf("decode message...\n");
        const char *r = buffer;
        uint32_t tuple_count;
        tuple_count = mp_decode_array(&r);
        struct query_t *query = malloc(sizeof(struct query_t));
        //for (int i = 0; i < tuple_count; i++) {
        query->searchio = mp_decode_uint(&r);
        query->start_date = mp_decode_uint(&r);
        query->end_date = mp_decode_uint(&r);
        query->user_id = mp_decode_uint(&r);
        //printf("iter: %d, val: %lld\n", i, val);
        //}
        struct query_t *result = malloc(sizeof(struct query_t));
        uint64_t request_id = (unsigned long)time(NULL);
        result->searchio = request_id;
        result->user_id = query->user_id;
        search_similarity(query, result);
        char buf[128];
        char *w = buf;
        w = mp_encode_array(w, 3);
        w = mp_encode_uint(w, result->searchio); // request_id
        w = mp_encode_uint(w, result->start_date); // start_date
        w = mp_encode_uint(w, result->end_date); // end_date
        w = mp_encode_uint(w, result->user_id); // user_id
        zmq_send(responder, buf, 128, 0);
        free(query);
        free(result);
    }
    return 0;
}
