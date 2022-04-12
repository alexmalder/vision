#include <iostream>

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