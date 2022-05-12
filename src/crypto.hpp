#include <cstdint>
#include <iostream>
#include <vector>
#include <math.h>
#include <limits>
#include <nlohmann/json.hpp>

#include "csv.hpp"
#include "structs.hpp"
#include "kafka.hpp"
#include "serdes.hpp"

double
vec_similarity(std::vector<crypto_t> a, std::vector<crypto_t> b, uint64_t end)
{
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
    for (uint64_t i = 0; i < end; i++) {
        dot += a[i].open * b[i].open;
        denom_a += a[i].open * a[i].open;
        denom_b += b[i].open * b[i].open;
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b));
}

void vec_filter(std::vector<crypto_t> &src,
                std::vector<crypto_t> &dest,
                query_t *query)
{
    for (uint64_t i = 0; i < src.size(); i++) {
        if (src[i].unix_val > query->start_date &&
            src[i].unix_val < query->end_date) {
            dest.push_back(src[i]);
        }
    }
}

std::vector<crypto_t> vec_iteration(std::vector<crypto_t> &src,
                                    std::vector<crypto_t> &dest,
                                    int start,
                                    int end)
{
    for (int i = start; i < end; i++) {
        dest.push_back(src[i]);
    }
    return dest;
}

void vec_up_to_date(std::vector<crypto_t> &dest, std::vector<crypto_t> &target)
{
    for (int i = 0; i < target.size(); i++) {
        target[i].unix_val = dest[i].unix_val;
        target[i].open = target[i].open += dest[i].open;
    }
}

void vec_debug(std::vector<crypto_t> &dest,
               std::vector<crypto_t> &target,
               double sim)
{
    nlohmann::json j0;
    std::vector<double> i0;
    std::vector<double> i1;

    for (auto item : dest) {
        i0.push_back(item.open);
    }

    for (auto item : dest) {
        i1.push_back(item.open);
    }
    j0["src"] = i0;
    j0["dest"] = i1;
    j0["sim"] = sim;

    std::cout << j0.dump() << std::endl;
}

int vec_search(std::vector<crypto_t> &src, query_t *query)
{
    // declare new vector
    std::vector<crypto_t> dest;
    // make filter
    vec_filter(src, dest, query);
    // static values
    const int ssize = src.size();
    const int dsize = dest.size();
    // debug values
    // std::cout << "src size: " << ssize << " and dest size: " << dsize << std::endl;
    // iteration values
    int x = 0;
    int y = dest.size();
    double sim;
    std::vector<std::string> messages;
    while (x < ssize) {
        while (y < ssize) {
            std::vector<crypto_t> target;
            vec_iteration(src, target, x, y);
            sim = vec_similarity(dest, target, dsize);
            if (sim > query->thresh && sim < 1) {
                //vec_up_to_date(dest, target);
                //vec_construct_result(messages, target, x, y, sim);
                vec_debug(dest, target, sim);
            }
            y += query->resolution, x += query->resolution;
        }
        x++;
    }
    //Kafka *kafka = new Kafka("result");
    //for (std::string message : messages) {
    //    kafka->produce(message);
    //}
    //kafka->flush_and_destroy();
    //delete kafka;
    return 0;
}
