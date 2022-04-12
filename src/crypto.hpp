#include <cstdint>
#include <iostream>
#include <vector>
#include <math.h>
#include "csv.hpp"
#include "structs.hpp"
#include <limits>

double vec_similarity(std::vector<double> a, std::vector<double> b,
                      uint64_t end)
{
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
    for (uint64_t i = 0; i < end; i++) {
        dot += a[i] * b[i];
        denom_a += a[i] * a[i];
        denom_b += b[i] * b[i];
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b));
}

void vec_filter(std::vector<crypto_t> &src, std::vector<double> &dest,
                query_t *query)
{
    for (uint64_t i = 0; i < src.size(); i++) {
        if (src[i].unix_val > query->start_date &&
            src[i].unix_val < query->end_date) {
            dest.push_back(src[i].open);
        }
    }
}

std::vector<double> vec_iteration(std::vector<crypto_t> &src,
                                  std::vector<double> &dest, int start, int end)
{
    for (int i = start; i < end; i++) {
        dest.push_back(src[i].open);
    }
    return dest;
}

int vec_search(std::vector<crypto_t> &src, query_t *query)
{
    // declave new vector
    std::vector<double> dest;
    // make filter
    vec_filter(src, dest, query);
    // static values
    const int resolution = 3;
    const double thresh = 0.999;
    const int ssize = src.size();
    // debug values
    std::cout << "src size: " << src.size() << std::endl;
    std::cout << "dest size: " << dest.size() << std::endl;
    // iteration values
    int x = 0;
    int y = dest.size();
    float sim;
    while (x < ssize) {
        while (y < ssize) {
            std::vector<double> target;
            vec_iteration(src, target, x, y);
            sim = vec_similarity(dest, target, dest.size());
            if (sim > thresh && sim < 1) {
                std::cout << "X: " << x << " Y: " << y
                          << " interval: " << (y - x) << " similarity: " << sim
                          << std::endl;
            }
            y += resolution, x += resolution;
        }
        x++;
    }
    return 0;
}