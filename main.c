#include "src/vision.h"
#include <unistd.h>
#include <msgpuck.h>

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
        printf("decode message...\n");
        const char *r = buffer;
        uint32_t tuple_count;
        //double value;
        tuple_count = mp_decode_array(&r);
        uint32_t size = mp_decode_map(&r);
        printf("size of map: %d\n", size);
        for (uint32_t i = 0; i < size; i++) {
            uint32_t key_len = 3;
            uint32_t *mylen = &key_len;
            const char *key = mp_decode_str(&r, mylen);
            double val = mp_decode_double(&r);
            printf("key: %s, val: %lf\n", key, val);
        }
        //value = mp_decode_double(&r);
        printf("received message with tuple_count <%d>\n", tuple_count);
        //unsigned int i, j;
        //char field_type;
        //field_type = mp_typeof(*r);
        /*
        for (i = 0; i < tuple_count; ++i) {
            field_type = mp_typeof(*r);
            uint32_t field_count = mp_decode_array(&r);
            printf("  field count=%u\n", field_count);
            for (j = 0; j < field_count; ++j) {
                if (field_type != MP_ARRAY) {
                    printf("no field array\n");
                    //exit(1);
                }
                if (field_type == MP_UINT) {
                    unsigned long num_value = mp_decode_uint(&r);
                    printf("    value=%lu.\n", num_value);
                } else if (field_type == MP_STR) {
                    const char *str_value;
                    uint32_t str_value_length;
                    str_value = mp_decode_str(&r, &str_value_length);
                    printf("    value=%.*s.\n", str_value_length, str_value);
                } else if (field_type == MP_DOUBLE) {
                    double double_value;
                    double_value = mp_decode_double(&r);
                    printf("    value=%lf.\n", double_value);
                } else if (field_type == MP_FLOAT) {
                    float float_value;
                    float_value = mp_decode_double(&r);
                    printf("    value=%lf.\n", float_value);
                } else if (field_type == MP_MAP) {
                    uint32_t size = mp_decode_map(&r);
                    printf("size of map: %d", size);
                    for (uint32_t i = 0; i < size; i++) {
                        uint32_t key_len = 3;
                        uint32_t *mylen = &key_len;
                        const char *key = mp_decode_str(&r, mylen);
                        //double val = mp_decode_double(&r);
                        printf("key: %s\n", key);
                    }
                } else {
                    printf("wrong field type %d\n", field_type);
                    exit(1);
                }
            }
        }
            */
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

int main()
{
    cosine_similarity_test();
    printf(
        "MP_DOUBLE: %d, MP_ARRAY: %d, MP_FLOAT: %d, MP_UINT: %d, MP_BOOL: %d, MP_EXT: %d, MP_BIN: %d, MP_MAP: %d\n",
        MP_DOUBLE, MP_ARRAY, MP_FLOAT, MP_UINT, MP_BOOL, MP_EXT, MP_BIN,
        MP_MAP);
    zmq_listen();
    return 0;
}
