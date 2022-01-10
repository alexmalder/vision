#include "vision.h"

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
