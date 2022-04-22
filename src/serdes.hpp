#pragma once

#include <nlohmann/json.hpp>
#include "structs.hpp"

void vec_construct_origin(std::vector<std::string> &messages,
                          std::vector<crypto_t> &crypto_data)
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

void vec_construct_result(std::vector<std::string> &messages,
                          std::vector<crypto_t> &result, int x, int y,
                          double sim)
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
