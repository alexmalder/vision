#include <cstdint>
#include <iostream>
#include <vector>
#include <math.h>
#include "csv.hpp"
#include "structs.hpp"

class crypto {
    std::vector<crypto_t> crypto_data;
    std::vector<double> b;

public:
    crypto(std::vector<crypto_t> &data)
    {
        crypto_data = data;
    }

    double vec_similarity(std::vector<double> a, uint64_t end)
    {
        double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
        for (uint64_t i = 0; i < end; i++) {
            dot += a[i] * b[i];
            denom_a += a[i] * a[i];
            denom_b += b[i] * b[i];
        }
        return dot / (sqrt(denom_a) * sqrt(denom_b));
    }

    void vec_filter(query_t *query)
    {
        for (uint64_t i = 0; i < crypto_data.size(); i++) {
            if (crypto_data[i].unix_val > query->start_date &&
                crypto_data[i].unix_val < query->end_date) {
                b.push_back(crypto_data[i].open);
            }
        }
    }

    std::vector<double> vec_iteration(std::vector<double> &src,
                                      std::vector<double> &dest, int start,
                                      int end)
    {
        for (int i = start; i < end; i++) {
            dest.push_back(src[i]);
        }
        return dest;
    }

    int vec_search(query_t *query)
    {
        // make filter
        vec_filter(query);
        // static values
        const int resolution = 10;
        const double thresh = 0.8;
        const int ssize = crypto_data.size();
        // debug values
        std::cout << "data size: " << crypto_data.size() << std::endl;
        std::cout << "b size: " << b.size() << std::endl;
        // iteration values
        int x = 0;
        int y = b.size();
        double sim;
        while (x < ssize) {
            while (y < ssize) {
                y += resolution, x += resolution;
                std::vector<double> dest;
                vec_iteration(b, dest, x, y);
                sim = vec_similarity(dest, dest.size());
                if (sim > thresh) {
                    std::cout << "X: " << x << " Y: " << y
                              << " interval: " << (y - x)
                              << " similarity: " << sim << std::endl;
                }
                b.clear();
            }
            x += resolution;
        }
        return 0;
    }
};
