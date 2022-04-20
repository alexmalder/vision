#include "src/crypto.hpp"

#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <msgpack.hpp>

using tup = std::tuple<int, double, double, double, double, double, double>;
using vec = std::vector<tup>;

void serialize(std::vector<crypto_t> &crypto_data)
{
    vec unix;
    std::stringstream ss;
    for (auto item : crypto_data) {
        tup val = { item.unix_val,  item.open,  item.high,
                    item.low,       item.close, item.volume_original,
                    item.volume_usd };
        unix.push_back(val);
    }
    msgpack::pack(ss, unix);
    std::ofstream outfile("saved.msgpack");
    outfile << ss.str() << std::endl;
}

void deserialize()
{
    std::ifstream t("saved.msgpack");
    std::stringstream buffer;
    buffer << t.rdbuf();
    vec unix;
    auto const &str = buffer.str();
    auto oh = msgpack::unpack(str.data(), str.size());
    auto obj = oh.get();
    std::cout << obj << std::endl;
    //assert(obj.as<decltype(unix)>() == unix);

    vec dst;
    obj.convert(dst);
    for (int i = 0; i < dst.size(); i++) {
        //std::get<int>();
        //std::cout << std::get<0>(dst[i]) << " " << std::get<1>(dst[i]) << "\n";
    }
}

int main()
{
    std::vector<crypto_t> crypto_data;
    std::ifstream file("../data/Bitstamp_BTCUSD_d.csv");
    CSVRow row;
    while (file >> row) {
        crypto_t c;
        c.unix_val = std::stoi(std::string(row[0]));
        c.datetime = row[1];
        c.symbol = row[2];
        c.open = std::stof(std::string(row[3]));
        c.high = std::stof(std::string(row[4]));
        c.low = std::stof(std::string(row[5]));
        c.close = std::stof(std::string(row[6]));
        c.volume_original = std::stof(std::string(row[7]));
        c.volume_usd = std::stof(std::string(row[8]));
        crypto_data.push_back(c);
    }

    std::string brokers = "192.168.15.5:9092";
    std::string topic = "data";

    Kafka kafka = Kafka(brokers, topic);

    for (auto item : crypto_data) {
        nlohmann::json j;
        j["unix_val"] = item.unix_val;
        j["datetime"] = item.datetime;
        j["symbol"] = item.symbol;
        j["open"] = item.open;
        j["high"] = item.high;
        j["low"] = item.low;
        j["close"] = item.close;
        j["volume_original"] = item.volume_original;
        j["volume_usd"] = item.volume_usd;
        std::string message = j.dump();
        kafka.produce(message);
    }
    kafka.flush_and_destroy();

    /*
    for (auto item : crypto_data) {
        printf("%ld %s %s %lf %lf %lf %lf %lf %lf\n", item.unix_val,
               item.datetime.data(), item.symbol.data(), item.open, item.high,
               item.low, item.close, item.volume_original, item.volume_usd);
    }
    */
    query_t *query = new query_t();
    query->searchio = 3;
    query->start_date = 1641987866;
    query->end_date = 1644666266;
    query->user_id = 1;
    //vec_search(crypto_data, query);
    //serialize(crypto_data);
    //deserialize();
}
