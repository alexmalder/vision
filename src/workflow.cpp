#include "vision.hpp"

vector<double> Workflow::extractVector(Query &query)
{
    pqxx::result result = rep->select_crypto(query);
    vector<double> data;
    for (unsigned int i = 0; i < result.size(); i++) {
        double target = result[i]["close"].as<double>();
        data.push_back(target);
    }
    return data;
}

double Workflow::cosineSimilarity(vector<double> &a, vector<double> &b,
                                  unsigned int length)
{
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
    for (unsigned int i = 0u; i < length; i++) {
        dot += a[i] * b[i];
        denom_a += a[i] * a[i];
        denom_b += b[i] * b[i];
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b));
}

Response_t Workflow::search(Query &query)
{
    Response_t response;
    Query vision(query.symbol, "2011-01-01", "2021-01-01");

    vector<double> vData = extractVector(vision);
    vector<double> sData = extractVector(query);
    uint64_t vSize = vData.size();
    uint64_t sSize = sData.size();

    unsigned int start = 0;
    unsigned int stop = vSize;
    unsigned int step = 10;

    cout << "vSize: " << vSize << endl;
    cout << "sSize: " << sSize << endl;

    vector<double> cData;
    for (unsigned int x = start; x < stop; x++) {
        cData.push_back(vData[x]);
        if (x % sSize == 0) {
            //cout << "x: " << x << endl;
            double result = cosineSimilarity(sData, cData, sSize);
            double thresh = 0.99;
            if (result > thresh) {
                std::cout << result << std::endl;
            }
            //start += step;
            //vSize += step;
            cData.clear();
        }
    }
    response.body = "ok";
    response.status = 201;
    return response;
}
/*
    X : iterate by small step
    Y :
    - fill array by small step size 
    - check similarity
    - if found return filled array
*/
Workflow::Workflow(Repository *rep)
{
    this->rep = rep;
}
