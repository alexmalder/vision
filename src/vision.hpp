#ifndef LIBVISION_H
#define LIBVISION_H

#include <cstdlib>
#include <cmath>
#include <iterator>
#include <fstream>
#include <utility>
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
#include <bcrypt/BCrypt.hpp>
#include <httplib.h>

using namespace std;

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
    vector<pair<string, string> > prepared;
    vector<string> create;
    string pg_conn;
};

struct Response_t {
    int status;
    string body;
};

struct Query {
    string symbol;
    string start_date;
    string end_date;
    Query(string sym, string start, string end);
};

struct Account_t {
    string email;
    string username;
    string password;
};

class Repository {
public:
    Repository();
    Repository(Config *config);
    void init();
    void insert_crypto(vector<CryptoData> &data);
    pqxx::result select_crypto(Query &data);
    pqxx::result select_fields();
    pqxx::result insert_account(Account_t &data);
    pqxx::result select_account(Account_t &data);

private:
    pqxx::connection C{ getenv("POSTGRES_CONN") };
    Config *config;
};

class Account {
public:
    Account(Repository *rep);
    Response_t sign_in(string body);
    Response_t sign_up(string body);

private:
    Repository *rep;
};

class Crypto {
public:
    Crypto(Repository *rep);
    Response_t get(string token, Query query);
    Response_t get_fields();
    Response_t post(string token, string body);

private:
    Repository *rep;
};

class Workflow {
public:
    Workflow(Repository *rep);
    Response_t search(Query &query);
    vector<double> extractVector(Query &query);
    double cosineSimilarity(vector<double> &a, vector<double> &b,
                            unsigned int length);

private:
    Repository *rep;
};

#endif