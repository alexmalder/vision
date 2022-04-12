#include <cstdint>
#include <iostream>
#include <vector>
#include <math.h>
#include <limits>
#include <nlohmann/json.hpp>

#include "csv.hpp"
#include "structs.hpp"
#include "kafka.hpp"

double vec_similarity(std::vector<crypto_t> a, std::vector<crypto_t> b,
                      uint64_t end)
{
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
    for (uint64_t i = 0; i < end; i++) {
        dot += a[i].open * b[i].open;
        denom_a += a[i].open * a[i].open;
        denom_b += b[i].open * b[i].open;
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b));
}

void vec_filter(std::vector<crypto_t> &src, std::vector<crypto_t> &dest,
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
                                    std::vector<crypto_t> &dest, int start,
                                    int end)
{
    for (int i = start; i < end; i++) {
        dest.push_back(src[i]);
    }
    return dest;
}

void debug_result(int x, int y, double sim, std::vector<crypto_t> &target)
{
    nlohmann::json result;
    result["x"] = x;
    result["y"] = y;
    result["sim"] = sim;
    nlohmann::json obj;
    for (auto item : target) {
        nlohmann::json j;
        j["open"] = item.open;
        j["close"] = item.close;
        obj.push_back(j);
    }

    result["obj"] = obj;
    produce(result.dump());
}

int vec_search(std::vector<crypto_t> &src, query_t *query)
{
    // declave new vector
    std::vector<crypto_t> dest;
    // make filter
    vec_filter(src, dest, query);
    // static values
    const int resolution = 3;
    const double thresh = 0.998;
    const int ssize = src.size();
    // debug values
    std::cout << "src size: " << src.size() << std::endl;
    std::cout << "dest size: " << dest.size() << std::endl;
    // iteration values
    int x = 0;
    int y = dest.size();
    double sim;
    while (x < ssize) {
        while (y < ssize) {
            std::vector<crypto_t> target;
            vec_iteration(src, target, x, y);
            sim = vec_similarity(dest, target, dest.size());
            if (sim > thresh && sim < 1) {
                debug_result(x, y, sim, target);
            }
            y += resolution, x += resolution;
        }
        x++;
    }
    return 0;
}