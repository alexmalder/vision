#include "vision.hpp"

Repository::Repository()
{
}

Repository::Repository(Config *conf)
{
    this->config = conf;
}

void Repository::init()
{
    // create tables
    pqxx::work W{ C };
    for (auto create_table : this->config->create) {
        W.exec(create_table);
    }
    W.commit();
    // prepared statements
    for (auto prepared : this->config->prepared) {
        C.prepare(prepared.first, prepared.second);
    }
}

void Repository::insert_crypto(vector<CryptoData> &data)
{
    pqxx::work W{ C };

    for (CryptoData cd : data) {
        W.exec_prepared("insert_crypto", cd.unix, cd.datetime, cd.symbol,
                        cd.open, cd.high, cd.low, cd.close, cd.volume_original,
                        cd.volume_usd);
    }
    W.commit();
}

pqxx::result Repository::select_crypto(Query &data)
{
    pqxx::work W{ C };
    pqxx::result result = W.exec_prepared("select_crypto", data.symbol,
                                          data.start_date, data.end_date);
    return result;
}

pqxx::result Repository::insert_account(Account_t &data)
{
    pqxx::work W{ C };
    pqxx::result result = W.exec_prepared("insert_account", data.email,
                                          data.username, data.password);
    W.commit();
    return result;
}

pqxx::result Repository::select_account(Account_t &data)
{
    pqxx::work W{ C };
    pqxx::result result = W.exec_prepared("select_account", data.username);
    return result;
}
