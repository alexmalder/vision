#include <iostream>
#include <vector>
#include <math.h>

static uint64_t resolution = 10;
static double thresh = 0.995;

struct crypto_t {
    uint64_t unix_val;
    std::string datetime;
    std::string symbol;
    double open;
    double high;
    double low;
    double close;
    double volume_original;
    double volume_usd;
};

struct query_t {
    uint64_t searchio;
    uint64_t start_date;
    uint64_t end_date;
    uint64_t user_id;
};

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

int vec_search(query_t *query, std::vector<crypto_t> &crypto_data)
{
    // extract by range, extract by currency type
    uint64_t x = 0;
    double sim;
    // src
    std::vector<double> source;
    for (uint64_t i = 0; i < crypto_data.size(); i++) {
        if (crypto_data[i].unix_val > query->start_date &&
            crypto_data[i].unix_val < query->end_date) {
            source.push_back(crypto_data[i].close);
        }
    }
    uint64_t ssize = source.size();
    uint64_t ssize_fork = ssize;
    std::cout << "size: " << crypto_data.size() << std::endl;
    while (x < crypto_data.size()) {
        std::vector<double> target;
        std::vector<double> result;
        if (x % resolution == 0) {
            uint64_t y = x;
            while (y < crypto_data.size()) {
                std::vector<double> items(ssize);
                target.push_back(crypto_data[y].close);
                result.push_back(crypto_data[y + ssize].close);
                if (y % ssize_fork == 0) {
                    sim = vec_similarity(target, source, ssize);
                    if (sim > thresh) {
                        //double vd = calculate_distance(source, target);
                        //debug_iteration(query, result[0], result[ssize], ssize, ssize_fork, 11, x, y, sim, source, target);
                        std::cout << "X: " << x << " Y: " << y
                                  << " interval: " << (y - x)
                                  << " similarity: " << sim << std::endl;
                    }
                    target.clear();
                }
                y++;
            }
            ssize_fork += resolution;
            x += resolution;
        }
        x++;
        target.clear();
        result.clear();
    }
    // clear crypto_data
    crypto_data.clear();
    return 0;
}
