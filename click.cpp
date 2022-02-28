#include <clickhouse/client.h>
#include <iostream>
#include <vector>
//
#include <string>
#include <iterator>
#include <algorithm>
#include <functional>
#include <numeric>

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
                     std::vector<double> source, std::vector<double> target)
{
    char buffer[4096 * 3];
    char source_buffer[4096];
    sprintf(source_buffer, "[ ");
    for (uint64_t i = 0; i < ssize; i++) {
        if (i == ssize - 1) {
            sprintf(source_buffer + strlen(source_buffer), "%lf", source[i]);
        } else {
            sprintf(source_buffer + strlen(source_buffer), "%lf,", source[i]);
        }
    }
    sprintf(source_buffer + strlen(source_buffer), "]");
    char target_buffer[4096];
    sprintf(target_buffer, "[");
    for (uint64_t i = 0; i < ssize; i++) {
        if (i == ssize - 1) {
            sprintf(target_buffer + strlen(target_buffer), "%lf", target[i]);
        } else {
            sprintf(target_buffer + strlen(target_buffer), "%lf,", target[i]);
        }
    }
    sprintf(target_buffer + strlen(target_buffer), "]");
    sprintf(
        buffer,
        R"({"symbol": %ld, "query_start_date": %ld, "query_end_date": %ld, "query_user_id": %ld, "founded_start_date": %ld, "founded_end_date": %ld, "ssize": %ld, "slide": %ld, "distance": %lf, "x": %ld, "y": %ld, "similarity": %lf, "source": %s, "target": %s}
)",
        query->searchio, query->start_date, query->end_date, query->user_id,
        founded_start_date, founded_end_date, ssize, slide, distance, x, y, sim,
        source_buffer, target_buffer);
    //int status = produce(buffer);
    printf("%s\n", buffer);
    //printf("[status: %d]\n", status);
}

int vec_up(std::vector<double> &source, uint64_t end, double distance)
{
    uint64_t i;
    for (i = 0; i < end; i++) {
        source[i] *= distance;
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

uint64_t calculate_size(query_t *query)
{
    uint64_t day_unix = 86400; // one day in unix format: constant
    uint64_t interval = query->end_date - query->start_date; // get interval
    uint64_t ssize = interval / day_unix; // size of array
    return ssize;
}

double calculate_distance(std::vector<double> &v1, std::vector<double> &v2)
{
    double distance;
    std::vector<double> distances;
    for (int x = 0; x < v1.size(); x++) {
        for (int y = 0; y < v2.size(); y++) {
            double diffY = v1[y] - v2[y];
            double diffX = v1[x] - v2[x];
            distances.push_back(sqrt((diffY * diffY) + (diffX * diffX)));
        }
    }

    for (int i = 0; i < distances.size(); i++) {
        distance += distances[i];
    }
    return (distance / distances.size());
}

int select_crypto(clickhouse::Client &client,
                  std::vector<crypto_t> &crypto_data)
{
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
    return 0;
}

int vec_search(query_t *query, std::vector<crypto_t> &crypto_data)
{
    // extract by range, extract by currency type
    uint64_t x = 0;
    uint64_t ssize = calculate_size(query);
    uint64_t ssize_fork = ssize;
    double sim;
    // src
    std::vector<double> source;
    for (uint64_t i = 0; i < crypto_data.size(); i++) {
        if (crypto_data[i].unix_val > query->start_date &&
            crypto_data[i].unix_val < query->end_date) {
            source.push_back(crypto_data[i].close);
        }
    }
    while (x < crypto_data.size()) {
        std::vector<double> target;
        std::vector<double> result;
        if (x % resolution == 0) {
            uint64_t y = x;
            while (y < crypto_data.size()) {
                target.push_back(crypto_data[y].close);
                result.push_back(crypto_data[y + ssize].close);

                if (y % ssize_fork == 0) {
                    //std::cout << "source size: " << source.size() << " | target size: " << target.size() << std::endl;
                    sim = vec_similarity(target, source, ssize);
                    if (sim > thresh) {
                        double vd = calculate_distance(source, target) / 200;
                        std::cout << "similarity: " << sim
                                  << " | distance: " << vd << std::endl;
                        //vec_up(target, ssize, vd);
                        debug_iteration(query, result[0], result[ssize], ssize,
                                        ssize_fork, vd, x, y, sim, source,
                                        target);
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

int main()
{
    /// Initialize client connection.
    clickhouse::Client client(clickhouse::ClientOptions()
                                  .SetHost(std::getenv("CH_HOST"))
                                  .SetUser(std::getenv("CH_USER"))
                                  .SetPassword(std::getenv("CH_PASSWORD"))
                                  .SetDefaultDatabase(std::getenv("CH_DB")));
    std::vector<crypto_t> crypto_data;
    select_crypto(client, crypto_data);
    query_t *query = new query_t();
    query->searchio = 3;
    query->start_date = 1577883661;
    query->end_date = 1585746061;
    query->user_id = 1;
    vec_search(query, crypto_data);
    return 0;
}
