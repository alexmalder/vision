// https://github.com/edenhill/librdkafka/blob/master/examples/producer.cpp

#include <librdkafka/rdkafkacpp.h>
#include <iostream>

class Kafka {
    std::string brokers;
    std::string topic;
    RdKafka::Producer *producer;
    // Set the delivery report callback.
    // The callback is only triggered from ::poll() and ::flush().
    struct ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
        void dr_cb(RdKafka::Message &message)
        {
            /* If message.err() is non-zero the message delivery failed permanently for the message. */
            if (message.err())
                std::cerr << "% Message delivery failed: " << message.errstr()
                          << std::endl;
            else
                std::cerr << "% Message delivered to topic "
                          << message.topic_name() << " [" << message.partition()
                          << "] at offset " << message.offset() << std::endl;
        }
    } ex_dr_cb;

public:
    Kafka(std::string brokers, std::string topic)
    {
        // Create configuration object
        this->brokers = brokers;
        this->topic = topic;

        RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
        std::string errstr;

        // Set bootstrap broker(s).
        conf->set("bootstrap.servers", brokers, errstr);

        conf->set("dr_cb", &ex_dr_cb, errstr);

        // Create a producer instance.
        producer = RdKafka::Producer::create(conf, errstr);

        delete conf;
    }

    int produce(std::string message)
    {
        RdKafka::ErrorCode err = producer->produce(
            /* Topic name */
            topic,
            /* Any Partition */
            RdKafka::Topic::PARTITION_UA,
            /* Make a copy of the value */
            RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
            /* Value */
            const_cast<char *>(message.c_str()), message.size(),
            /* Key */
            NULL, 0,
            /* Timestamp (defaults to current time) */
            0,
            /* Message headers, if any */
            NULL,
            /* Per-message opaque value passed to delivery report */
            NULL);

        if (err != RdKafka::ERR_NO_ERROR) {
            std::cerr << "% Failed to produce to topic " << topic << ": "
                      << RdKafka::err2str(err) << std::endl;

            if (err == RdKafka::ERR__QUEUE_FULL) {
                // If the internal queue is full, wait for messages to be delivered and then retry.
                producer->poll(1000 /*block for max 1000ms*/);
            }
        } else {
            std::cout << "% Enqueued message (" << message.size() << " bytes) "
                      << "for topic " << topic << std::endl;
        } // A producer application should continually serve the delivery report queue // by calling poll() at frequent intervals. producer->poll(0);
        return err;
    }

    int flush_and_destroy()
    {
        /* Wait for final messages to be delivered or fail. */
        std::cout << "% Flushing final messages..." << std::endl;
        producer->flush(10 * 1000 /* wait for max 10 seconds */);

        if (producer->outq_len() > 0)
            std::cerr << "% " << producer->outq_len()
                      << "message(s) were not delivered" << std::endl;

        delete producer;
    }
};