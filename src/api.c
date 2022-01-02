#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HTTPSERVER_IMPL
#define RESPONSE "VNMNTN"
#define CONTENT_TYPE "Content-Type"
#define APP_JSON "application/json"
#define TEXT_PLAIN "text/plain"

#include "vision.h"

int request_target_is(struct http_request_s *request, char const *target)
{
    http_string_t url = http_request_target(request);
    int len = strlen(target);
    return len == url.len && memcmp(url.buf, target, url.len) == 0;
}

void query_init(struct query_t *query, const char *buffer)
{
    // create json objects
    struct json_object *parsed_json;
    struct json_object *start_date;
    struct json_object *end_date;
    struct json_object *symbol;
    struct json_object *field_name;
    // fill json objects
    parsed_json = json_tokener_parse(buffer);
    json_object_object_get_ex(parsed_json, "start_date", &start_date);
    json_object_object_get_ex(parsed_json, "end_date", &end_date);
    json_object_object_get_ex(parsed_json, "symbol", &symbol);
    json_object_object_get_ex(parsed_json, "field_name", &field_name);
    // fill data structure
    query->start_date = json_object_get_uint64(start_date);
    query->end_date = json_object_get_uint64(end_date);
    query->symbol = json_object_get_uint64(symbol);
    //query->field_name = json_object_get_string(field_name);
    // free json objects
    json_object_put(parsed_json);
    json_object_put(start_date);
    json_object_put(end_date);
    json_object_put(symbol);
    json_object_put(field_name);
}

void handle_request(struct http_request_s *request)
{
    http_request_connection(request, HTTP_AUTOMATIC);
    struct http_response_s *response = http_response_init();
    http_response_status(response, 200);
    if (request_target_is(request, "/v1/crypto")) {
        http_string_t body = http_request_body(request);
        struct query_t *q = malloc(sizeof(struct query_t));
        query_init(q, body.buf);
        // extract header
        struct http_string_s ss = http_request_header(request, "authorization");
        if (ss.len != 0) {
            char *token = ss.buf;
            token[ss.len] = '\0';
            printf("%s", token);
        }
        // debug
        printf("[%lld %lld %lld]\n", q->start_date, q->end_date, q->symbol);
        fflush(stdout);
        // select
        struct crypto_data *cd = malloc(sizeof(struct crypto_data) * 2048);
        int tuple_count = tarantool_select(q, cd);
        printf("Tuple count: %d", tuple_count);

        // creating json array
        json_object *jarray = json_object_new_array_ext(tuple_count);
        json_object *junix;
        //json_object *jdatetime = json_object_new_string(cd[i].datetime);
        json_object *jsymbol;
        json_object *jopen;
        json_object *jhigh;
        json_object *jlow;
        json_object *jclose;
        json_object *jvolume_original;
        json_object *jvolume_usd;
        for (int i = 0; i < tuple_count; i++) {
            printf("staring json object create with iterator: %d\n", i);
            json_object *junix = json_object_new_uint64(cd[i].unix);
            //json_object *jdatetime = json_object_new_string(cd[i].datetime);
            jsymbol = json_object_new_uint64(cd[i].symbol);
            jopen = json_object_new_double(cd[i].open);
            jhigh = json_object_new_double(cd[i].high);
            jlow = json_object_new_double(cd[i].low);
            jclose = json_object_new_double(cd[i].close);
            jvolume_original = json_object_new_double(cd[i].volume_original);
            jvolume_usd = json_object_new_double(cd[i].volume_usd);

            printf(
                "staring json object add with iterator %d and open value %f\n",
                i, cd[i].open);
            json_object *jobj = json_object_new_object();
            json_object_object_add(jobj, "unix", junix);
            //json_object_object_add(jobj, "datetime", jdatetime);
            json_object_object_add(jobj, "symbol", jsymbol);
            json_object_object_add(jobj, "open", jopen);
            json_object_object_add(jobj, "high", jhigh);
            json_object_object_add(jobj, "low", jlow);
            json_object_object_add(jobj, "close", jclose);
            json_object_object_add(jobj, "volume_original", jvolume_original);
            json_object_object_add(jobj, "volume_usd", jvolume_usd);
            json_object_array_add(jarray, jobj);
            fflush(stdout);
        }

        // make response
        const char *resp = json_object_to_json_string(jarray);
        http_response_header(response, CONTENT_TYPE, APP_JSON);
        http_response_body(response, resp, strlen(resp));
        http_respond(request, response);
        // free memory
        free(q);
        free(cd);
        json_object_put(jarray);
        json_object_put(junix);
        json_object_put(jsymbol);
        json_object_put(jopen);
        json_object_put(jhigh);
        json_object_put(jlow);
        json_object_put(jclose);
        json_object_put(jvolume_original);
        json_object_put(jvolume_usd);
    } else if (request_target_is(request, "/v1/workflow")) {
        http_response_header(response, CONTENT_TYPE, APP_JSON);
        http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
        http_respond(request, response);
    } else {
        http_response_header(response, CONTENT_TYPE, APP_JSON);
        http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
        http_respond(request, response);
    }
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
