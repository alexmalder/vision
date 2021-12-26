#ifndef LIBVISION_H
#define LIBVISION_H

#include <cstdlib>
#include <cmath>
#include <iterator>
#include <fstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <string>
#include <pqxx/pqxx>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>
#include <openssl/sha.h>
#include <httpserver.hpp>
#include <bcrypt/BCrypt.hpp>

using namespace std;
using namespace httpserver;

struct CryptoData {
    uint64_t unix;
    string datetime;
    string symbol;
    double open;
    double high;
    double low;
    double close;
    double volume_original;
    double volume_usd;
};

struct Config {
    Config(string configPath);
    string create_crypto;
    string insert_crypto;
    string select_crypto;
    string create_account;
    string insert_account;
    string select_account;
    string pg_conn;
};

struct Account {
    string email;
    string username;
    string password;
};

struct Query {
    string symbol;
    string start_date;
    string end_date;
};

class Repository {
public:
    Repository();
    Repository(Config *config);
    void init();
    void insert_crypto(vector<CryptoData> &data);
    pqxx::result select_crypto(Query &data);
    pqxx::result insert_account(Account &data);
    pqxx::result select_account(Account &data);

private:
    string pg_user = getenv("POSTGRES_USER");
    string pg_pass = getenv("POSTGRES_PASSWORD");
    string pg_host = getenv("POSTGRES_HOST");
    string pg_port = getenv("POSTGRES_PORT");
    string pg_db = getenv("POSTGRES_DB");
    string pg_conn = "postgresql://" + pg_user + ":" + pg_pass + "@" + pg_host +
                     ":" + pg_port + "/" + pg_db;
    pqxx::connection C{ pg_conn };
    Config *config;
};

class Visioner {
public:
    Visioner(vector<CryptoData> vcd);
    double cosine_similarity(vector<double> A, vector<double> B,
                             unsigned int Vector_Length);
    void crossvalidation();

private:
    vector<CryptoData> vcd;
};

class SignInAPI : public http_resource {
    Repository *rep;

public:
    SignInAPI(Repository *repository);
    const std::shared_ptr<http_response> render_POST(const http_request &req);
};

class SignUpAPI : public http_resource {
    Repository *rep;

public:
    SignUpAPI(Repository *repository);
    const std::shared_ptr<http_response> render_POST(const http_request &req);
};

class CryptoAPI : public http_resource {
    Repository *rep;

public:
    CryptoAPI(Repository *repository);
    const std::shared_ptr<http_response> render_GET(const http_request &req);
    const std::shared_ptr<http_response> render_POST(const http_request &req);
};

#endif
