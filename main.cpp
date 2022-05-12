#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <httplib.h>

#include "src/serdes.hpp"
#include "src/crypto.hpp"


void test_query() {
    std::vector<crypto_t> crypto_data;
    scan_data(crypto_data);
    std::vector<std::string> messages;
    vec_construct_origin(messages, crypto_data);
    //push_data(messages, crypto_data);
    query_t *query = new query_t();
    query->searchio = 3;
    query->start_date = 1641987866;
    query->end_date = 1644666266;
    query->user_id = 1;
    query->resolution = 3;
    query->thresh = 0.998;
    vec_search(crypto_data, query);
}

int main()
{
    httplib::Server svr;
    std::vector<crypto_t> crypto_data;
    scan_data(crypto_data);
    svr.Post("/api/crypto", [&crypto_data](const httplib::Request &req, httplib::Response &res) {
        nlohmann::json data = nlohmann::json::parse(req.body);
        query_t *query = new query_t();
        query->searchio = data["searchio"].get<int>();
        query->start_date = data["start_date"].get<int>();
        query->end_date = data["end_date"].get<int>();
        query->user_id = data["user_id"].get<int>();
        query->resolution = data["resolution"].get<int>();
        query->thresh = data["thresh"].get<double>();
        vec_search(crypto_data, query);
        res.set_content("ok", "text/plain");
    });
    std::cout << "httplib server listening on port 9000" << std::endl;
    svr.listen("0.0.0.0", 9000);
}
