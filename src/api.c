#include <stdint.h>
#include <stdlib.h>
#define HTTPSERVER_IMPL

#define RESPONSE "VNMNTN"
#define CONTENT_TYPE "Content-Type"
#define APP_JSON "application/json"

#include "vision.h"

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
        http_response_header(response, CONTENT_TYPE, APP_JSON);
        http_response_body(response, body.buf, body.len);
    } else if (request_target_is(request, "/v1/crypto")) {
        http_string_t body = http_request_body(request);
        http_response_header(response, CONTENT_TYPE, APP_JSON);
        struct crypto_data *cd = malloc(sizeof(struct crypto_data));
        tarantool_insert(cd);
        fflush(stdout);
        http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
    } else if (request_target_is(request, "/v1/query")) {
        http_string_t body = http_request_body(request);
        struct query_t *query = malloc(sizeof(struct query_t));

        struct json_object *parsed_json;
        struct json_object *start_date;
        struct json_object *end_date;
        struct json_object *symbol;
        struct json_object *field_name;

        parsed_json = json_tokener_parse(body.buf);
        json_object_object_get_ex(parsed_json, "start_date", &start_date);
        json_object_object_get_ex(parsed_json, "end_date", &end_date);
        json_object_object_get_ex(parsed_json, "symbol", &symbol);
        json_object_object_get_ex(parsed_json, "field_name", &field_name);

        query->start_date = json_object_get_string(start_date);
        query->end_date = json_object_get_string(end_date);
        query->symbol = json_object_get_string(symbol);
        query->field_name = json_object_get_string(field_name);

        struct http_string_s ss = http_request_header(request, "authorization");
        char *token = ss.buf;
        token[ss.len] = '\0';
        printf("%s", token);

        //printf("%s %s %s %s\n", query->start_date, query->end_date, query->symbol, query->field_name);
        fflush(stdout);

        http_response_header(response, CONTENT_TYPE, APP_JSON);
        http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
        free(query);
        json_object_put(parsed_json);
        json_object_put(start_date);
        json_object_put(end_date);
        json_object_put(symbol);
        json_object_put(field_name);
    } else {
        http_response_header(response, CONTENT_TYPE, APP_JSON);
        http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
    }
    http_respond(request, response);
}

int cosine_similarity_test()
{
    double a[] = { 0.11, 0.12, 0.13 };
    double b[] = { 0.11, 0.12, 0.14 };
    double r = cosine_similarity(a, b, 3);
    printf("cosine_similarity test result: %lf\n", r);
    fflush(stdout);
    return 0;
}
