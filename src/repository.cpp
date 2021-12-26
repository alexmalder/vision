#include "../vision.hpp"

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
    W.exec(this->config->create_account);
    W.exec(this->config->create_crypto);
    W.commit();
    // prepare statements
    C.prepare("select_account", this->config->select_account);
    C.prepare("select_crypto", this->config->select_crypto);
    C.prepare("insert_account", this->config->insert_account);
    C.prepare("insert_crypto", this->config->insert_crypto);
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

pqxx::result Repository::insert_account(Account &data)
{
    pqxx::work W{ C };
    pqxx::result result =
        W.exec_prepared("insert_account", data.email, data.username, data.password);
    W.commit();
    return result;
}

pqxx::result Repository::select_account(Account &data)
{
    pqxx::work W{ C };
    pqxx::result result = W.exec_prepared("select_account", data.username);
    return result;
}
