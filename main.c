#define HTTPSERVER_IMPL

#include <httpserver.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#define RESPONSE "VNMNTN"

int request_target_is(struct http_request_s *request, char const *target)
{
    http_string_t url = http_request_target(request);
    int len = strlen(target);
    return len == url.len && memcmp(url.buf, target, url.len) == 0;
}

void handle_request(struct http_request_s *request)
{
    http_request_connection(request, HTTP_AUTOMATIC);
    struct http_response_s *response = http_response_init();
    http_response_status(response, 200);
    if (request_target_is(request, "/echo")) {
        http_string_t body = http_request_body(request);
        http_response_header(response, "Content-Type", "application/json");
        http_response_body(response, body.buf, body.len);
    } else {
        http_response_header(response, "Content-Type", "application/json");
        http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
    }
    http_respond(request, response);
}

struct http_server_s *server;

void handle_sigterm(int signum)
{
    (void)signum;
    free(server);
    exit(0);
}

int main()
{
    signal(SIGTERM, handle_sigterm);
    server = http_server_init(8080, handle_request);
    http_server_listen(server);
}
