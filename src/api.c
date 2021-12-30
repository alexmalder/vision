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

        struct json_object *parsed_json;
        struct json_object *array_length;
        struct json_object *json_items;
        struct json_object *json_item;
        // fields
        struct json_object *json_unix;
        struct json_object *json_datetime;
        struct json_object *json_symbol;
        struct json_object *json_open;
        struct json_object *json_high;
        struct json_object *json_low;
        struct json_object *json_close;
        struct json_object *json_volume_original;
        struct json_object *json_volume_usd;

        parsed_json = json_tokener_parse(body.buf);
        json_object_object_get_ex(parsed_json, "length", &array_length);
        json_object_object_get_ex(parsed_json, "items", &json_items);
        uint64_t length = json_object_get_uint64(array_length);
        struct crypto_data *cd = malloc(sizeof(struct crypto_data) * length);

        printf("array_length: %lld", length);
        for (uint64_t i = 0; i < length; i++) {
            // get
            json_item = json_object_array_get_idx(json_items, i);
            json_unix = json_object_object_get(json_item, "unix");
            json_datetime = json_object_object_get(json_item, "datetime");
            json_symbol = json_object_object_get(json_item, "symbol");
            json_open = json_object_object_get(json_item, "open");
            json_high = json_object_object_get(json_item, "high");
            json_low = json_object_object_get(json_item, "low");
            json_close = json_object_object_get(json_item, "close");
            json_volume_original =
                json_object_object_get(json_item, "volume_original");
            json_volume_usd = json_object_object_get(json_item, "volume_usd");
            // fill
            cd[i].unix = json_object_get_uint64(json_unix);
            cd[i].datetime = json_object_get_string(json_datetime);
            cd[i].symbol = json_object_get_string(json_symbol);
            cd[i].open = json_object_get_double(json_open);
            cd[i].high = json_object_get_double(json_high);
            cd[i].low = json_object_get_double(json_low);
            cd[i].close = json_object_get_double(json_close);
            cd[i].volume_original =
                json_object_get_double(json_volume_original);
            cd[i].volume_usd = json_object_get_double(json_volume_usd);
            printf("open=%lf, high: %lf, low: %lf, close: %lf\n", cd->open,
                   cd->high, cd->low, cd->close);
        }
        tarantool_insert(cd, length);
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
