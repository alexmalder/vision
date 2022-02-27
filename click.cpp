#include <clickhouse/client.h>
#include <iostream>
#include <vector>
//
#include <string>
#include <iterator>
#include <algorithm>
#include <functional>
#include <numeric>
#include <thread>
#include <chrono>

static uint64_t resolution = 10;
static double thresh = 0.995;

struct row_t {
    uint64_t unix_val;
    double value;
};

struct crypto_t {
    uint64_t unix_val;
    std::string datetime;
    uint64_t symbol;
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

void debug_iteration(query_t *query, uint64_t founded_start_date,
                     uint64_t founded_end_date, uint64_t ssize, uint64_t slide,
                     double distance, uint64_t x, uint64_t y, double sim,
                     std::vector<row_t> source, std::vector<row_t> target)
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
        R"({"symbol": %ld, "query_start_date": %ld, "query_end_date": %ld, "query_user_id": %ld, "founded_start_date": %ld, "founded_end_date": %ld, "ssize": %ld, "slide": %ld, "distance": %lf, "x": %lld, "y": %lld, "similarity": %lf, "source": %s, "target": %s}
)",
        query->searchio, query->start_date, query->end_date, query->user_id,
        founded_start_date, founded_end_date, ssize, slide, distance, x, y, sim,
        source_buffer, target_buffer);
    //int status = produce(buffer);
    printf("%s\n", buffer);
    //printf("[status: %d]\n", status);
}

int vec_up(std::vector<row_t> &source, uint64_t end, double distance)
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

double vec_similarity(std::vector<row_t> a, std::vector<row_t> b, uint64_t end)
{
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
    for (uint64_t i = 0; i < end; i++) {
        dot += a[i].value * b[i].value;
        denom_a += a[i].value * a[i].value;
        denom_b += b[i].value * b[i].value;
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b));
}

uint64_t calculate_size(query_t *query)
{
    uint64_t day_unix = 86400; // one day in unix format: constant
    uint64_t interval = query->end_date - query->start_date; // get interval
    uint64_t ssize = interval / day_unix; // size of array
    return ssize;
}

double vectors_distance(const std::vector<row_t> &a,
                        const std::vector<row_t> &b)
{
    std::vector<double> auxiliary;

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(auxiliary), //
                   [](row_t element1, row_t element2) {
                       return pow((element1.value - element2.value), 2);
                   });
    auxiliary.shrink_to_fit();
    return std::sqrt(std::accumulate(auxiliary.begin(), auxiliary.end(), 0.0));
}

int main()
{
    /// Initialize client connection.
    std::this_thread::sleep_for(std::chrono::milliseconds(7000));
    clickhouse::Client client(clickhouse::ClientOptions()
                                  .SetHost(std::getenv("CH_HOST"))
                                  .SetUser(std::getenv("CH_USER"))
                                  .SetPassword(std::getenv("CH_PASSWORD"))
                                  .SetDefaultDatabase(std::getenv("CH_DB")));

    std::vector<crypto_t> crypto_data;
    std::string select("select * from crypto where Symbol = 3");

    /// Select values inserted in the previous step.
    client.Select(select, [&crypto_data](const clickhouse::Block &block) {
        for (size_t i = 0; i < block.GetRowCount(); ++i) {
            crypto_t cr;
            cr.unix_val = block[0]->As<clickhouse::ColumnUInt64>()->At(i);
            cr.datetime = block[1]->As<clickhouse::ColumnString>()->At(i);
            cr.symbol = block[2]->As<clickhouse::ColumnUInt64>()->At(i);
            cr.open = block[3]->As<clickhouse::ColumnFloat64>()->At(i);
            cr.high = block[4]->As<clickhouse::ColumnFloat64>()->At(i);
            cr.low = block[5]->As<clickhouse::ColumnFloat64>()->At(i);
            cr.close = block[6]->As<clickhouse::ColumnFloat64>()->At(i);
            crypto_data.push_back(cr);
        }
    });

    query_t *query = new query_t();
    query->searchio = 3;
    query->start_date = 1630454400;
    query->end_date = 1638316800;
    query->user_id = 1;

    // extract by range, extract by currency type
    uint64_t x = 0;
    uint64_t ssize = calculate_size(query);
    uint64_t ssize_fork = ssize;
    double sim;
    // src
    std::vector<row_t> source;
    for (uint64_t i = 0; i < crypto_data.size(); i++) {
        if (crypto_data[i].unix_val >= query->start_date &&
            crypto_data[i].unix_val < query->end_date) {
            row_t row_i;
            row_i.unix_val = crypto_data[i].unix_val;
            row_i.value = crypto_data[i].close;
            source.push_back(row_i);
        }
    }
    while (x < crypto_data.size()) {
        std::vector<row_t> target;
        std::vector<row_t> result;
        if (x % resolution == 0) {
            uint64_t y = x;
            while (y < crypto_data.size()) {
                row_t row_y;
                row_y.unix_val = crypto_data[y].unix_val;
                row_y.value = crypto_data[y].close;
                target.push_back(row_y);

                row_t row_z;
                row_z.unix_val = crypto_data[y + ssize].unix_val;
                row_z.value = crypto_data[y + ssize].close;
                result.push_back(row_z);

                if (y % ssize_fork == 0) {
                    sim = vec_similarity(target, source, ssize);
                    if (sim > thresh) {
                        double vd = vectors_distance(source, target) / 1000;
                        std::cout << "similarity:" << sim
                                  << " | distance: " << vd << std::endl;
                        vec_up(target, ssize, vd);
                        debug_iteration(query, result[0].unix_val,
                                        result[ssize].unix_val, ssize,
                                        ssize_fork, vd, x, y, sim, source,
                                        target);
                    }
                    target.clear();
                    // clear all
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
    std::this_thread::sleep_for(std::chrono::milliseconds(20000));
    return 0;
}
