#include "src/vision.hpp"

int main(int argc, char **argv)
{
    using namespace httplib;
    Repository *rep = new Repository(new Config("config.yml"));
    rep->init();

    Server srv;

    srv.Post("/v1/integrate", [&rep](const Request &req, Response &res) {
        cout << req.body << endl;
        res.set_content("ok", "text/plain");
    });

    srv.Post("/v1/user/sign-in", [&rep](const Request &req, Response &res) {
        Account *acc = new Account(rep);
        Response_t r = acc->sign_in(req.body);
        delete acc;
        res.status = r.status;
        res.set_content(r.body, "application/json");
    });

    srv.Post("/v1/user/sign-up", [&rep](const Request &req, Response &res) {
        Account *acc = new Account(rep);
        Response_t r = acc->sign_up(req.body);
        delete acc;
        res.status = r.status;
        res.set_content(r.body, "application/json");
    });

    srv.Get("/v1/crypto", [&rep](const Request &req, Response &res) {
        string token = req.get_header_value("authorization");
        WorkflowQuery query;
        query.symbol = req.get_param_value("symbol");
        query.start_date = req.get_param_value("start_date");
        query.end_date = req.get_param_value("end_date");
        query.field_name = req.get_param_value("field_name");

        Crypto *crypto = new Crypto(rep);
        Response_t r = crypto->get(token, query);
        delete crypto;
        res.status = r.status;
        res.set_content(r.body, "application/json");
    });

    srv.Post("/v1/crypto", [&rep](const Request &req, Response &res) {
        string token = req.get_header_value("authorization");
        Crypto *crypto = new Crypto(rep);
        Response_t r = crypto->post(token, req.body);
        delete crypto;
        res.set_content(r.body, "application/json");
    });

    srv.Get("/v1/workflow", [&rep](const Request &req, Response &res) {
        string token = req.get_header_value("authorization");
        WorkflowQuery query;
        query.symbol = req.get_param_value("symbol");
        query.start_date = req.get_param_value("start_date");
        query.end_date = req.get_param_value("end_date");
        query.field_name = req.get_param_value("field_name");

        Workflow *workflow = new Workflow(rep);
        Response_t response = workflow->search(token, query);
        delete workflow;
        res.status = response.status;
        res.set_content(response.body, "application/json");
    });

    srv.Get("/v1/workflow/fields", [&rep](const Request &req, Response &res) {
        string token = req.get_header_value("authorization");
        Workflow *workflow = new Workflow(rep);
        Response_t response = workflow->getFields(token);
        delete workflow;
        res.status = response.status;
        res.set_content(response.body, "application/json");
    });

    cout << "server listening port 5000" << endl;
    srv.listen("0.0.0.0", 5000);

    return 0;
}
