#pragma once

#include <nlohmann/json.hpp>
#include <msgpack.hpp>
#include "structs.hpp"

using tup = std::tuple<int, double, double, double, double, double, double>;
using vec = std::vector<tup>;

void vec_construct_origin(std::vector<std::string> &messages, std::vector<crypto_t> &crypto_data)
{
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
        messages.push_back(j.dump());
    }
}

void vec_construct_result(std::vector<std::string> &messages, std::vector<crypto_t> &result, int x, int y, double sim)
{
    for (auto item : result) {
        nlohmann::json j;
        // custom
        j["x"] = x;
        j["y"] = y;
        j["sim"] = sim;
        // basic
        j["unix_val"] = item.unix_val;
        //j["datetime"] = item.datetime;
        j["symbol"] = item.symbol;
        j["open"] = item.open;
        j["high"] = item.high;
        j["low"] = item.low;
        j["close"] = item.close;
        j["volume_original"] = item.volume_original;
        j["volume_usd"] = item.volume_usd;
        messages.push_back(j.dump());
    }
}

void serialize(std::vector<crypto_t> &crypto_data)
{
    vec unix;
    std::stringstream ss;
    for (auto item : crypto_data) {
        tup val = { item.unix_val, item.open, item.high, item.low, item.close, item.volume_original, item.volume_usd };
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
