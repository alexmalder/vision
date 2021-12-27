#include "vision.hpp"

int main(int argc, char **argv)
{
    using namespace httplib;
    std::string configPath("config.yml");
    Config *config = new Config(configPath);
    Repository *rep = new Repository(config);
    rep->init();

    Server srv;

    srv.Post("/integrate", [&rep](const Request &req, Response &res) {
        cout << req.body << endl;
        res.set_content("ok", "text/plain");
    });

    srv.Post("/sign-in", [&rep](const Request &req, Response &res) {
        // destruct request
        nlohmann::json req_json;
        auto data = req_json.parse(req.body);
        string username = data["username"].get<string>();
        string password = data["password"].get<string>();
        cout << "username: " << username << " password: " << password << endl;

        // get account by username
        Account account;
        account.username = username;
        pqxx::result result = rep->select_account(account);
        // destruct result
        nlohmann::json res_json;
        if (!result.empty()) {
            string saved_pwd = result[0]["password"].as<string>();
            if (BCrypt::validatePassword(password, saved_pwd)) {
                res_json["token"] = account.username;
                res.status = 200;
            } else {
                res.status = 401;
                res_json["message"] = "unauthorized";
            }
        } else {
            res.status = 401;
            res_json["message"] = "unauthorized";
        }
        res.set_content(res_json.dump(), "application/json");
    });

    srv.Post("/sign-up", [&rep](const Request &req, Response &res) {
        // destruct request
        nlohmann::json req_json;
        auto data = req_json.parse(req.body);
        string email = data["email"].get<string>();
        string username = data["username"].get<string>();
        string password = data["password"].get<string>();
        // construct response
        Account account;
        account.email = email;
        account.username = username;
        account.password = BCrypt::generateHash(password);
        pqxx::result result = rep->insert_account(account);
        nlohmann::json res_json;
        string uid = to_string(result[0]["id"].as<int>());
        res_json["messge"] = "Account id: " + uid;
        res.status = 200;
        res.set_content(res_json.dump(), "application/json");
    });

    srv.Get("/crypto", [&rep](const Request &req, Response &res) {
        string username = req.get_header_value("authorization");
        Account account;
        account.username = username;
        pqxx::result result = rep->select_account(account);
        nlohmann::json res_json;
        if (!result.empty()) {
            Query query;
            query.symbol = req.get_param_value("symbol");
            query.start_date = req.get_param_value("start_date");
            query.end_date = req.get_param_value("end_date");
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
            res.status = 200;
        } else {
            res.status = 401;
            res_json["message"] = "unauthorized";
        }
        res.set_content(res_json.dump(), "application/json");
    });

    srv.Post("/crypto", [&rep](const Request &req, Response &res) {
        string username = req.get_header_value("authorization");
        Account account;
        account.username = username;
        pqxx::result result = rep->select_account(account);
        nlohmann::json res_json;
        if (!result.empty()) {
            nlohmann::json json;
            vector<CryptoData> vcd;
            auto data = json.parse(req.body);
            for (auto item : data) {
                std::cout << item << std::endl;
                vcd.push_back(
                    { item["unix"].get<uint64_t>(),
                      item["datetime"].get<string>(),
                      item["symbol"].get<string>(), item["open"].get<double>(),
                      item["high"].get<double>(), item["low"].get<double>(),
                      item["close"].get<double>(),
                      item["volume_original"].get<double>(),
                      item["volume_usd"].get<double>() });
            }
            rep->insert_crypto(vcd);
            res.status = 200;
            res_json["message"] = "You'r data inserted";
        } else {
            res.status = 401;
            res_json["message"] = "unauthorized";
        }
        res.set_content(res_json.dump(), "application/json");
    });
    cout << "server listening port 5000" << endl;
    srv.listen("0.0.0.0", 5000);

    return 0;
}
