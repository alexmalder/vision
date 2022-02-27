#include "../include/vision.h"

static uint64_t resolution = 10;
static double thresh = 0.995;

int vec_fill(crypto_t *source, query_t *query, int tuple_count, array_t *target)
{
    for (uint64_t i = 0; i < tuple_count; i++) {
        if (source[i].unix_val >= query->start_date &&
            source[i].unix_val < query->end_date) {
            row_t *row_i = malloc(sizeof(row_t));
            row_i->unix_val = source[i].unix_val;
            row_i->value = source[i].close;
            insert_array(target, row_i);
        }
    }
    return 0;
}

int vec_slide(crypto_t *cd, query_t *result, int tuple_count, array_t *target)
{
    return 0;
}

double vec_similarity(row_t *a, row_t *b, uint64_t end)
{
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
    for (uint64_t i = 0; i < end; i++) {
        dot += a[i].value * b[i].value;
        denom_a += a[i].value * a[i].value;
        denom_b += b[i].value * b[i].value;
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b));
}

double vec_distance(row_t *target, uint64_t end)
{
    double distance;
    uint64_t i;
    for (i = 0; i < end; i++) {
        distance += target[i].value;
    }
    return (distance / end);
}

int vec_up(row_t *source, uint64_t end, double distance)
{
    uint64_t i;
    for (i = 0; i < end; i++) {
        source[i].value *= distance;
    }
    return 0;
}

int vec_down(row_t *source, uint64_t end, double distance)
{
    uint64_t i;
    for (i = 0; i < end; i++) {
        source[i].value /= distance;
    }
    return 0;
}

int vec_merge(row_t *source, row_t *target, uint64_t end)
{
    double sum;
    uint64_t i;
    for (i = 0; i < end; i++) {
        sum = target[i].value + source[i].value;
        target[i].value = sum / 2;
    }
    return 0;
}

uint64_t calculate_size(query_t *query)
{
    uint64_t day_unix = 86400; // one day in unix format: constant
    uint64_t interval = query->end_date - query->start_date; // get interval
    uint64_t ssize = interval / day_unix; // size of array
    return ssize;
}

int vec_search(char *conn_str, query_t *query)
{
    // extract all
    crypto_t *cd = malloc(sizeof(crypto_t) * 4096);
    int tuple_count = select_crypto(conn_str, query, cd);
    uint64_t ssize = calculate_size(query);
    // extract by range, extract by currency type
    uint64_t x = 0;
    uint64_t ssize_fork = ssize;
    double sim;
    while (x < tuple_count) {
        // src
        array_t source;
        init_array(&source, tuple_count);
        vec_fill(cd, query, tuple_count, &source);
        double src_dist = vec_distance(source.rows, ssize);
        // dest
        array_t target;
        init_array(&target, tuple_count);
        // result
        array_t result;
        init_array(&result, tuple_count);
        // iter
        if (x % resolution == 0) {
            uint64_t y = x;
            while (y < tuple_count) {
                row_t *row_y = malloc(sizeof(row_t));
                row_y->unix_val = cd[y].unix_val;
                row_y->value = cd[y].close;
                insert_array(&target, row_y);

                row_t *row_z = malloc(sizeof(row_t));
                row_z->unix_val = cd[y + ssize].unix_val;
                row_z->value = cd[y + ssize].close;
                insert_array(&result, row_z);

                if (y % ssize_fork == 0) {
                    sim = vec_similarity(target.rows, source.rows, ssize);
                    if (sim > thresh) {
                        double distance;
                        double dest_dist = vec_distance(target.rows, ssize);
                        printf("src_dist and dest_dist: [%lf,%lf]\n", src_dist,
                               dest_dist);
                        if (src_dist < dest_dist) {
                            distance = dest_dist - src_dist;
                            vec_up(result.rows, ssize, distance);
                        } else {
                            distance = src_dist - dest_dist;
                            vec_down(result.rows, ssize, distance);
                        }
                        //vec_merge(result.rows, source.rows, ssize);
                        debug_iteration(query, result.rows[0].unix_val,
                                        result.rows[ssize].unix_val, ssize,
                                        ssize_fork, distance, x, y, sim,
                                        source.rows, result.rows);

                        //insert_result(query, &target, request_id);
                        //break;
                    }
                    free_array(&source);
                    init_array(&source, ssize);
                    free_array(&target);
                    init_array(&target, ssize);
                    free_array(&result);
                    init_array(&result, ssize);
                }
                y++;
            }
            ssize_fork += resolution;
            x += resolution;
        }
        //if (sim > thresh) { break; }
        x++;
        free_array(&source);
        free_array(&target);
        free_array(&result);
    }
    free(cd);
    return 0;
}

void query_init(query_t *query, uint64_t searchio, uint64_t start_date,
                uint64_t end_date, uint64_t user_id)
{
    query->searchio = searchio;
    query->start_date = start_date;
    query->end_date = end_date;
    query->user_id = user_id;
}

void debug_iteration(query_t *query, uint64_t founded_start_date,
                     uint64_t founded_end_date, uint64_t ssize, uint64_t slide,
                     double distance, uint64_t x, uint64_t y, double sim,
                     row_t *source, row_t *target)
{
    char buffer[4096 * 3];
    char source_buffer[4096];
    sprintf(source_buffer, "[ ");
    for (uint64_t i = 0; i < ssize; i++) {
        if (i == ssize - 1) {
            sprintf(source_buffer + strlen(source_buffer), "%lf",
                    source[i].value);
        } else {
            sprintf(source_buffer + strlen(source_buffer), "%lf,",
                    source[i].value);
        }
    }
    sprintf(source_buffer + strlen(source_buffer), "]");
    char target_buffer[4096];
    sprintf(target_buffer, "[");
    for (uint64_t i = 0; i < ssize; i++) {
        if (i == ssize - 1) {
            sprintf(target_buffer + strlen(target_buffer), "%lf",
                    target[i].value);
        } else {
            sprintf(target_buffer + strlen(target_buffer), "%lf,",
                    target[i].value);
        }
    }
    sprintf(target_buffer + strlen(target_buffer), "]");
    sprintf(
        buffer,
        "{\"symbol\": %lld, \"query_start_date\": %lld, \"query_end_date\": %lld, \"query_user_id\": %lld, \"founded_start_date\": %lld, \"founded_end_date\": %lld, \"ssize\": %lld, \"slide\": %lld, \"distance\": %lf, \"x\": %lld, \"y\": %lld, \"similarity\": %lf, \"source\": %s, \"target\": %s}\n",
        query->searchio, query->start_date, query->end_date, query->user_id,
        founded_start_date, founded_end_date, ssize, slide, distance, x, y, sim,
        source_buffer, target_buffer);
    //int status = produce(buffer);
    printf("%s\n", buffer);
    //printf("[status: %d]\n", status);
}

static volatile sig_atomic_t run = 1;

static void stop(int sig)
{
    run = 0;
    //fclose(stdin);
    exit(1);
}

static void dr_msg_cb(rd_kafka_t *rk, const rd_kafka_message_t *rkmessage,
                      void *opaque)
{
    if (rkmessage->err)
        fprintf(stderr, "%% Message delivery failed: %s\n",
                rd_kafka_err2str(rkmessage->err));
    else
        fprintf(stderr,
                "%% Message delivered (%zd bytes, "
                "partition %" PRId32 ")\n",
                rkmessage->len, rkmessage->partition);
}

int produce(char *buf)
{
    rd_kafka_t *rk; /* Producer instance handle */
    rd_kafka_conf_t *conf; /* Temporary configuration object */
    char errstr[512]; /* librdkafka API error reporting buffer */
    const char *brokers = "10.12.0.1:9092"; /* Argument: broker list */
    const char *topic = "log"; /* Argument: topic to produce to */

    conf = rd_kafka_conf_new();

    if (rd_kafka_conf_set(conf, "bootstrap.servers", brokers, errstr,
                          sizeof(errstr)) != RD_KAFKA_CONF_OK) {
        fprintf(stderr, "%s\n", errstr);
        return 1;
    }
    rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);

    rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
    if (!rk) {
        fprintf(stderr, "%% Failed to create new producer: %s\n", errstr);
        return 1;
    }

    signal(SIGINT, stop);

    int err = rd_kafka_producev(rk, RD_KAFKA_V_TOPIC(topic),
                                RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
                                RD_KAFKA_V_VALUE(buf, strlen(buf)),
                                RD_KAFKA_V_OPAQUE(NULL), RD_KAFKA_V_END);

    rd_kafka_flush(rk, 10 * 1000);

    if (rd_kafka_outq_len(rk) > 0)
        fprintf(stderr, "%% %d message(s) were not delivered\n",
                rd_kafka_outq_len(rk));

    rd_kafka_destroy(rk);
    return 0;
}
#define MP_SOURCE 1

static int CRYPTO_SPACE = 513;
static int RESULT_SPACE = 514;

int insert_result(char *conn_string, query_t *query, array_t *array,
                  uint64_t request_id)
{
    struct tnt_stream *tnt = tnt_net(NULL);
    tnt_set(tnt, TNT_OPT_URI, conn_string);
    if (tnt_connect(tnt) < 0) {
        printf("Connection refused \n");
        return -1;
    }
    const char *format = "[%d%d%d%d%lf]";
    for (uint64_t i = 0; i < array->size; i++) {
        struct tnt_stream *tuple = tnt_object(NULL);
        tnt_object_format(tuple, format, query->user_id, request_id,
                          array->rows[i].unix_val, query->searchio,
                          array->rows[i].value);
        tnt_insert(tnt, RESULT_SPACE, tuple);
    }
    tnt_flush(tnt);
    //tnt_stream_free(tuple);
    struct tnt_reply reply;
    tnt_reply_init(&reply);
    tnt->read_reply(tnt, &reply);
    if (reply.code != 0) {
        printf("Insert failed %lu.\n", reply.code);
    }
    printf("Tuple inserted with code %lu.\n", reply.code);
    fflush(stdout);
    tnt_close(tnt);
    tnt_stream_free(tnt);
    return 0;
}

int delete_result(char *conn_string, query_t *query)
{
    struct tnt_stream *tnt = tnt_net(NULL);
    tnt_set(tnt, TNT_OPT_URI, conn_string);
    if (tnt_connect(tnt) < 0) {
        printf("Connection refused \n");
        return -1;
    }
    const char *format = "[%d%d]";
    struct tnt_stream *tuple = tnt_object(NULL);
    tnt_object_format(tuple, format, query->user_id, query->searchio);
    tnt_insert(tnt, RESULT_SPACE, tuple);
    tnt_flush(tnt);
    struct tnt_reply reply;
    tnt_reply_init(&reply);
    tnt->read_reply(tnt, &reply);
    if (reply.code != 0) {
        printf("Delete failed %lu.\n", reply.code);
    }
    printf("Tuple deleted with code %lu.\n", reply.code);
    fflush(stdout);
    tnt_close(tnt);
    tnt_stream_free(tuple);
    tnt_stream_free(tnt);
    return 0;
}

int select_crypto(char *conn_string, query_t *query, crypto_t *cd)
{
    struct tnt_stream *tnt = tnt_net(NULL);
    tnt_set(tnt, TNT_OPT_URI, conn_string);
    if (tnt_connect(tnt) < 0) {
        printf("Connection refused\n");
        exit(1);
    }
    struct tnt_stream *tuple = tnt_object(NULL);
    tnt_object_format(tuple, "[%lld]", query->searchio);
    tnt_select(tnt, CRYPTO_SPACE, 0, 1048576, 0, 0, tuple);
    tnt_flush(tnt);
    struct tnt_reply reply;
    tnt_reply_init(&reply);
    tnt->read_reply(tnt, &reply);
    if (reply.code != 0) {
        printf("Select failed with status code %ld.\n", reply.code);
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

int zmq_listen(char *conn_str)
{
    void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP);
    int rc = zmq_bind(responder, "tcp://*:5555");
    printf("consumer_active=1\n");
    while (consumer_active) {
        char buffer[128];
        zmq_recv(responder, buffer, 128, 0);
        printf("decode message...\n");
        const char *r = buffer;
        uint32_t tuple_count;
        tuple_count = mp_decode_array(&r);
        query_t *query = malloc(sizeof(query_t));
        //for (int i = 0; i < tuple_count; i++) {
        query->searchio = mp_decode_uint(&r);
        query->start_date = mp_decode_uint(&r);
        query->end_date = mp_decode_uint(&r);
        query->user_id = mp_decode_uint(&r);
        //printf("iter: %d, val: %lld\n", i, val);
        //}
        query_t *result = malloc(sizeof(query_t));
        //uint64_t request_id = (unsigned long)time(NULL);
        uint64_t request_id = 1;
        result->searchio = request_id;
        result->user_id = query->user_id;
        vec_search(conn_str, query);
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

void init_array(array_t *a, size_t initial_size)
{
    a->rows = malloc(initial_size * sizeof(row_t));
    a->used = 0;
    a->size = initial_size;
}

void insert_array(array_t *a, row_t *row)
{
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size
    if (a->used == a->size) {
        a->size *= 2;
        a->rows = realloc(a->rows, a->size * sizeof(row_t));
    }
    a->rows[a->used++] = *row;
}

void free_array(array_t *a)
{
    free(a->rows);
    a->rows = NULL;
    a->used = a->size = 0;
}
