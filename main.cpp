#include "src/vision.hpp"
#include <httpserver.h>
#include <stdlib.h>

int c_plus_plus_version(int argc, char **argv)
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

void handle_request(struct http_request_s *request)
{
    struct http_response_s *response = http_response_init();
    http_response_status(response, 200);
    http_response_header(response, "Content-Type", "application/json");
    char response_body[] = "{\"status\":\"ok\"}";
    http_response_body(response, response_body, sizeof(response_body) - 1);
    http_respond(request, response);
}

int main(int argc, char **argv)
{
    struct http_server_s *server = http_server_init(5000, handle_request);
    printf("%s\n", "server listening on port 5000");
    http_server_listen(server);
    return 0;
}
