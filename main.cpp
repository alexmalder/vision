#include "src/vision.hpp"

int main(int argc, char **argv)
{
    using namespace httplib;
    std::string configPath("config.yml");
    Config *config = new Config(configPath);
    Repository *rep = new Repository(config);
    rep->init();

    Server srv;

    srv.Post("/integrate", [&rep](const Request &req, Response &res) {
        cout << req.body << endl;
        res.set_content("ok", "text/plain");
    });

    srv.Post("/sign-in", [&rep](const Request &req, Response &res) {
        Account *acc = new Account(rep);
        Response_t r = acc->sign_in(req.body);
        delete acc;
        res.status = r.status;
        res.set_content(r.body, "application/json");
    });

    srv.Post("/sign-up", [&rep](const Request &req, Response &res) {
        Account *acc = new Account(rep);
        Response_t r = acc->sign_up(req.body);
        delete acc;
        res.status = r.status;
        res.set_content(r.body, "application/json");
    });

    srv.Get("/crypto", [&rep](const Request &req, Response &res) {
        string token = req.get_header_value("authorization");
        string symbol = req.get_param_value("symbol");
        string start_date = req.get_param_value("start_date");
        string end_date = req.get_param_value("end_date");
        Query query(symbol, start_date, end_date);

        Crypto *crypto = new Crypto(rep);
        Response_t r = crypto->get(token, query);
        delete crypto;
        res.status = r.status;
        res.set_content(r.body, "application/json");
    });

    srv.Post("/crypto", [&rep](const Request &req, Response &res) {
        string token = req.get_header_value("authorization");
        Crypto *crypto = new Crypto(rep);
        Response_t r = crypto->post(token, req.body);
        delete crypto;
        res.set_content(r.body, "application/json");
    });
    srv.Get("/workflow", [&rep](const Request &req, Response &res) {
        //string token = req.get_header_value("authorization");
        string symbol = req.get_param_value("symbol");
        string start_date = req.get_param_value("start_date");
        string end_date = req.get_param_value("end_date");
        string full_start = "2011-01-01";
        string full_end = "2021-01-01";
        Query query(symbol, start_date, end_date);

        Query vision(symbol, full_start, full_end);

        pqxx::result vResult = rep->select_crypto(vision);
        unsigned int vSize = vResult.size();
        //vector<CryptoData> vData;
        vector<double> vData;
        for (unsigned int i = 0; i < vSize; i++) {
            /*
            CryptoData vItem;
            vItem.open = vResult[i]["open"].as<double>();
            vItem.datetime = vResult[i]["datetime"].as<string>();
            vItem.close = vResult[i]["close"].as<double>();
            vItem.volume_original = vResult[i]["volume_original"].as<double>();
            vItem.volume_usd = vResult[i]["volume_usd"].as<double>();
            vData.push_back(vItem);
            */
            double close = vResult[i]["close"].as<double>();
            vData.push_back(close);
        }

        pqxx::result sResult = rep->select_crypto(query);
        unsigned int sSize = sResult.size();
        //vector<CryptoData> sData;
        vector<double> sData;
        for (unsigned int i = 0; i < sSize; i++) {
            /*
            CryptoData sItem;
            sItem.open = vResult[i]["open"].as<double>();
            sItem.datetime = vResult[i]["datetime"].as<string>();
            sItem.close = vResult[i]["close"].as<double>();
            sItem.volume_original = vResult[i]["volume_original"].as<double>();
            sItem.volume_usd = vResult[i]["volume_usd"].as<double>();
            sData.push_back(sItem);
            */
            double close = sResult[i]["close"].as<double>();
            sData.push_back(close);
        }

        unsigned int start = 0;
        unsigned int stop = vSize;
        unsigned int step = 10;
        //unsigned int end = 0 + step;

        /**
         * X : iterate by small step
         * Y :
         * - fill array by small step size 
         * - check similarity
         * - if found return filled array
         */

        cout << "vSize: " << vSize << endl;
        cout << "sSize: " << sSize << endl;

        vector<double> data;
        for (unsigned int x = start; x < stop; x++) {
            data.push_back(vData[x]);
            if (x % sResult.size() == 0) {
                //cout << "x: " << x << endl;
                double result = cosine_similarity(sData, data, sResult.size());
                double thresh = 0.99;
                if (result > thresh) {
                    std::cout << result << std::endl;
                }
                //start += step;
                //vSize += step;
                data.clear();
            }
        }

        res.set_content("ok", "text/plain");
    });
    cout << "server listening port 5000" << endl;
    srv.listen("0.0.0.0", 5000);

    return 0;
}
