#include "vision.hpp"

Response_t Account::sign_in(string body)
{
    Response_t response;
    // destruct request
    nlohmann::json req_json;
    auto data = req_json.parse(body);
    string username = data["username"].get<string>();
    string password = data["password"].get<string>();
    cout << "username: " << username << " password: " << password << endl;

    // get account by username
    Account_t account;
    account.username = username;
    pqxx::result result = rep->select_account(account);
    // destruct result
    nlohmann::json res_json;
    if (!result.empty()) {
        string saved_pwd = result[0]["password"].as<string>();
        if (BCrypt::validatePassword(password, saved_pwd)) {
            res_json["token"] = account.username;
            response.status = 200;
        } else {
            response.status = 401;
            res_json["message"] = "unauthorized";
        }
    } else {
        response.status = 401;
        res_json["message"] = "unauthorized";
    }
    response.body = res_json.dump();
    return response;
}

Response_t Account::sign_up(string body)
{
    Response_t response;
    // destruct request
    nlohmann::json req_json;
    auto data = req_json.parse(body);
    string email = data["email"].get<string>();
    string username = data["username"].get<string>();
    string password = data["password"].get<string>();
    // construct response
    Account_t account;
    account.email = email;
    account.username = username;
    account.password = BCrypt::generateHash(password);
    pqxx::result result = rep->insert_account(account);
    nlohmann::json res_json;
    string uid = to_string(result[0]["id"].as<int>());
    res_json["messge"] = "Account id: " + uid;
    response.status = 200;
    response.body = res_json.dump();
    return response;
}

Account::Account(Repository *rep)
{
    this->rep = rep;
}
