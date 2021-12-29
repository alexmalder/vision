#include "src/vision.hpp"

int main(int argc, char **argv)
{
    using namespace httplib;
    Repository *rep = new Repository(new Config("config.yml"));
    rep->init();

    Server srv;

    srv.Post("/integrate", [&rep](const Request &req, Response &res) {
        cout << req.body << endl;
        res.set_content("ok", "text/plain");
    });

    srv.Post("/sign-in", [&rep](const Request &req, Response &res) {
        Account *acc = new Account(rep);
        Response_t r = acc->sign_in(req.body);
        delete acc;
        res.status = r.status;
        res.set_content(r.body, "application/json");
    });

    srv.Post("/sign-up", [&rep](const Request &req, Response &res) {
        Account *acc = new Account(rep);
        Response_t r = acc->sign_up(req.body);
        delete acc;
        res.status = r.status;
        res.set_content(r.body, "application/json");
    });

    srv.Get("/fields", [&rep](const Request &req, Response &res) {
        Response_t r;
        nlohmann::json res_json;
        pqxx::result result = rep->select_fields();
        for (auto row : result) {
            nlohmann::json j;
            j["table_name"] = row["table_name"].as<string>();
            j["column_name"] = row["column_name"].as<string>();
            j["data_type"] = row["data_type"].as<string>();
            res_json.push_back(j);
        }
        r.body = res_json.dump();
        r.status = 200;
        res.status = r.status;
        res.set_content(r.body, "application/json");
    });

    srv.Get("/crypto", [&rep](const Request &req, Response &res) {
        string token = req.get_header_value("authorization");
        string symbol = req.get_param_value("symbol");
        string start_date = req.get_param_value("start_date");
        string end_date = req.get_param_value("end_date");
        Query query(symbol, start_date, end_date);

        Crypto *crypto = new Crypto(rep);
        Response_t r = crypto->get(token, query);
        delete crypto;
        res.status = r.status;
        res.set_content(r.body, "application/json");
    });

    srv.Post("/crypto", [&rep](const Request &req, Response &res) {
        string token = req.get_header_value("authorization");
        Crypto *crypto = new Crypto(rep);
        Response_t r = crypto->post(token, req.body);
        delete crypto;
        res.set_content(r.body, "application/json");
    });

    srv.Get("/workflow", [&rep](const Request &req, Response &res) {
        //string token = req.get_header_value("authorization");
        string symbol = req.get_param_value("symbol");
        string start_date = req.get_param_value("start_date");
        string end_date = req.get_param_value("end_date");

        Query query(symbol, start_date, end_date);
        Workflow *workflow = new Workflow(rep);
        Response_t response = workflow->search(query);
        delete workflow;
        res.status = response.status;
        res.set_content(response.body, "text/plain");
    });
    cout << "server listening port 5000" << endl;
    srv.listen("0.0.0.0", 5000);

    return 0;
}
