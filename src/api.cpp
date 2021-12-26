#include "../vision.hpp"

AccountAPI::AccountAPI(Repository *rep)
{
    this->rep = rep;
}
const std::shared_ptr<http_response>
AccountAPI::render_GET(const http_request &req)
{
    // destruct request
    string email = req.get_arg("email");
    string password = req.get_arg("password");
    cout << "email: " << email << " password: " << password << endl;

    // get account by email
    Account account;
    account.email = email;
    pqxx::result result = rep->select_account(account);
    // destruct result
    nlohmann::json res_json;
    if (!result.empty()) {
        string saved_pwd = result[0]["password"].as<string>();
        if (BCrypt::validatePassword(password, saved_pwd)) {
            res_json["token"] = account.email;
            return shared_ptr<http_response>(
                new string_response(res_json.dump(), 200, "application/json"));
        } else {
            return shared_ptr<http_response>(
                new string_response("unauthorized", 401, "application/json"));
        }
    } else {
        return shared_ptr<http_response>(
            new string_response("unauthorized", 401, "application/json"));
    }
}

const std::shared_ptr<http_response>
AccountAPI::render_POST(const http_request &req)
{
    // destruct request
    nlohmann::json req_json;
    auto data = req_json.parse(req.get_content());
    string email = data["email"].get<string>();
    string password = data["password"].get<string>();
    // construct response
    Account account;
    account.email = email;
    account.password = BCrypt::generateHash(password);
    pqxx::result result = rep->insert_account(account);
    nlohmann::json res_json;
    string suid = to_string(result[0]["id"].as<int>());
    res_json["messge"] = "Account id: " + suid;
    return shared_ptr<http_response>(
        new string_response(res_json.dump(), 200, "application/json"));
}

CryptoAPI::CryptoAPI(Repository *rep)
{
    this->rep = rep;
}
const std::shared_ptr<http_response>
CryptoAPI::render_GET(const http_request &req)
{
    string email = req.get_header("authorization");
    Account account;
    account.email = email;
    pqxx::result result = rep->select_account(account);
    if (!result.empty()) {
        nlohmann::json res_json;
        Query query;
        query.symbol = req.get_arg("symbol");
        query.start_date = req.get_arg("start_date");
        query.end_date = req.get_arg("end_date");
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
        return shared_ptr<http_response>(
            new string_response(res_json.dump(), 200, "application/json"));
    } else {
        nlohmann::json res_json;
        res_json["message"] = "unauthorized";
        return shared_ptr<http_response>(
            new string_response(res_json.dump(), 401, "application/json"));
    }
}

const std::shared_ptr<http_response>
CryptoAPI::render_POST(const http_request &req)
{
    string email = req.get_header("authorization");
    Account account;
    account.email = email;
    pqxx::result result = rep->select_account(account);
    if (!result.empty()) {
        nlohmann::json json;
        vector<CryptoData> vcd;
        auto data = json.parse(req.get_content());
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
        nlohmann::json res_json;
        res_json["data"] = "You'r data inserted";
        return shared_ptr<http_response>(
            new string_response(res_json.dump(), 200, "application/json"));
    } else {
        nlohmann::json res_json;
        res_json["message"] = "unauthorized";
        return shared_ptr<http_response>(
            new string_response(res_json.dump(), 401, "application/json"));
    }
}
