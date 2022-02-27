#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include <librdkafka/rdkafka.h>

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
