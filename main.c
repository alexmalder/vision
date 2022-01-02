#include "src/vision.h"
#include <unistd.h>

int cosine_similarity_test()
{
    double a[] = { 0.11, 0.12, 0.13 };
    double b[] = { 0.11, 0.12, 0.14 };
    double r = cosine_similarity(a, b, 3);
    printf("cosine_similarity test result: %lf\n", r);
    fflush(stdout);
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
        printf("received hello\n");
        sleep(1);
        zmq_send(responder, "world", 5, 0);
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
        printf("sending hello %d...", request_nbr);
        zmq_send(requester, "hello", 5, 0);
        zmq_recv(requester, buffer, 10, 0);
        printf("received world %d\n", request_nbr);
    }
    zmq_close(requester);
    zmq_ctx_destroy(context);
    return 0;
}

int main()
{
    cosine_similarity_test();
    zmq_listen();
    return 0;
}
