#include <iostream>

struct crypto_t {
    int unix_val;
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
    int searchio;
    int start_date;
    int end_date;
    int user_id;
    int resolution;
    double thresh;
};
