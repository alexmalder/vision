#include "vision.hpp"

vector<WorkflowResponse> Workflow::extractVector(WorkflowQuery &query)
{
    pqxx::result result = rep->select_crypto(query);
    vector<WorkflowResponse> data;
    for (unsigned int i = 0; i < result.size(); i++) {
        string datetime = result[i]["datetime"].as<string>();
        double target = result[i][query.field_name].as<double>();
        data.push_back({ datetime, target });
    }
    return data;
}

nlohmann::json extractCompare(vector<WorkflowResponse> compareData)
{
    nlohmann::json json;
    for (WorkflowResponse wr : compareData) {
        nlohmann::json j;
        j["datetime"] = wr.datetime;
        j["target"] = wr.target;
        json.push_back(j);
    }
    return json;
}

double Workflow::cosineSimilarity(vector<WorkflowResponse> &a,
                                  vector<WorkflowResponse> &b,
                                  unsigned int length)
{
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
    for (unsigned int i = 0u; i < length; i++) {
        dot += a[i].target * b[i].target;
        denom_a += a[i].target * a[i].target;
        denom_b += b[i].target * b[i].target;
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b));
}

Response_t Workflow::search(string token, WorkflowQuery &query)
{
    Response_t response;
    Account_t account;
    account.username = token;
    pqxx::result dbacc = rep->select_account(account);
    nlohmann::json res_json;
    if (!dbacc.empty()) {
        WorkflowQuery vision;
        vision.symbol = query.symbol;
        vision.start_date = "2012-01-01";
        vision.end_date = "2022-01-01";
        vision.field_name = query.field_name;

        vector<WorkflowResponse> visionedData = extractVector(vision);
        vector<WorkflowResponse> searchData = extractVector(query);
        uint64_t vSize = visionedData.size();
        uint64_t sSize = searchData.size();

        unsigned int start = 0;
        unsigned int stop = vSize;
        unsigned int step = 10;

        cout << "vSize: " << vSize << endl;
        cout << "sSize: " << sSize << endl;

        vector<WorkflowResponse> compareData;
        for (unsigned int x = start; x < stop; x++) {
            compareData.push_back(visionedData[x]);
            if (x % sSize == 0) {
                //cout << "x: " << x << endl;
                double result =
                    cosineSimilarity(searchData, compareData, sSize);
                double thresh = 0.99;
                if (result > thresh) {
                    std::cout << result << std::endl;
                    nlohmann::json j = extractCompare(compareData);
                    res_json.push_back(j);
                }
                //start += step;
                //vSize += step;
                compareData.clear();
            }
        }
        response.status = 200;
    } else {
        response.status = 401;
        res_json["message"] = "unauthorized";
    }
    response.body = res_json.dump();
    return response;
}

Response_t Workflow::getFields(string token)
{
    Response_t response;
    Account_t account;
    account.username = token;
    pqxx::result dbacc = rep->select_account(account);
    nlohmann::json res_json;
    if (!dbacc.empty()) {
        pqxx::result result = rep->select_fields();
        for (auto row : result) {
            nlohmann::json j;
            j["table_name"] = row["table_name"].as<string>();
            j["column_name"] = row["column_name"].as<string>();
            j["data_type"] = row["data_type"].as<string>();
            res_json.push_back(j);
        }
        response.status = 200;
    } else {
        response.status = 401;
        res_json["message"] = "unauthorized";
    }

    response.body = res_json.dump();
    return response;
}

Workflow::Workflow(Repository *rep)
{
    this->rep = rep;
}
