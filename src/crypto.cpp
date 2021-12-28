#include "vision.hpp"
#include <regex>

Query::Query(string sym, string start, string end)
{
    this->symbol = sym;
    this->start_date = start;
    this->end_date = end;
}

Crypto::Crypto(Repository *rep)
{
    this->rep = rep;
}

Response_t Crypto::get(string token, Query query)
{
    Response_t response;
    Account_t account;
    account.username = token;
    pqxx::result result = rep->select_account(account);
    nlohmann::json res_json;
    if (!result.empty()) {
        pqxx::result rows = rep->select_crypto(query);
        for (auto row : rows) {
            nlohmann::json j;
            //j["unix"] = row["unix"].as<uint64_t>();
            j["datetime"] = row["datetime"].as<string>();
            //j["symbol"] = row["symbol"].as<string>();
            j["open"] = row["open"].as<double>();
            //j["high"] = row["high"].as<double>();
            //j["low"] = row["low"].as<double>();
            j["close"] = row["close"].as<double>();
            j["volume_original"] = row["volume_original"].as<double>();
            j["volume_usd"] = row["volume_usd"].as<double>();
            res_json.push_back(j);
        }
        response.status = 200;
    } else {
        response.status = 401;
        res_json["message"] = "unauthorized";
    }
    response.body = res_json.dump();
    return response;
}

Response_t Crypto::post(string token, string body)
{
    Response_t response;
    Account_t account;
    account.username = token;
    pqxx::result result = rep->select_account(account);
    nlohmann::json res_json;
    if (!result.empty()) {
        nlohmann::json json;
        vector<CryptoData> vcd;
        auto data = json.parse(body);
        for (auto item : data) {
            std::cout << item << std::endl;
            vcd.push_back(
                { item["unix"].get<uint64_t>(), item["datetime"].get<string>(),
                  item["symbol"].get<string>(), item["open"].get<double>(),
                  item["high"].get<double>(), item["low"].get<double>(),
                  item["close"].get<double>(),
                  item["volume_original"].get<double>(),
                  item["volume_usd"].get<double>() });
        }
        rep->insert_crypto(vcd);
        response.status = 200;
        res_json["message"] = "You'r data inserted";
    } else {
        response.status = 401;
        res_json["message"] = "unauthorized";
    }
    response.body = res_json.dump();
    return response;
}
