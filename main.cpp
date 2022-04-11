#include "src/lib.hpp"

#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class CSVRow {
public:
    std::string_view operator[](std::size_t index) const
    {
        return std::string_view(&m_line[m_data[index] + 1],
                                m_data[index + 1] - (m_data[index] + 1));
    }
    std::size_t size() const
    {
        return m_data.size() - 1;
    }
    void readNextRow(std::istream &str)
    {
        std::getline(str, m_line);

        m_data.clear();
        m_data.emplace_back(-1);
        std::string::size_type pos = 0;
        while ((pos = m_line.find(',', pos)) != std::string::npos) {
            m_data.emplace_back(pos);
            ++pos;
        }
        // This checks for a trailing comma with no data after it.
        pos = m_line.size();
        m_data.emplace_back(pos);
    }

private:
    std::string m_line;
    std::vector<int> m_data;
};

std::istream &operator>>(std::istream &str, CSVRow &data)
{
    data.readNextRow(str);
    return str;
}

int main()
{
    std::ifstream file("../data/Bitstamp_BTCUSD_1h.csv");

    CSVRow row;
    std::vector<crypto_t> crypto;
    while (file >> row) {
        crypto_t c;
        c.unix_val = std::stoi(std::string(row[0]));
        c.datetime = row[1];
        c.symbol = row[2];
        c.open = std::stof(std::string(row[3]));
        c.high = std::stof(std::string(row[4]));
        c.low = std::stof(std::string(row[5]));
        c.close = std::stof(std::string(row[6]));
        c.volume_original = std::stof(std::string(row[7]));
        c.volume_usd = std::stof(std::string(row[8]));
        crypto.push_back(c);
    }
    for (auto item : crypto) {
        printf("%ld %s %s %lf %lf %lf %lf %lf %lf\n", item.unix_val,
               item.datetime.data(), item.symbol.data(), item.open, item.high,
               item.low, item.close, item.volume_original, item.volume_usd);
    }
    query_t *query = new query_t();
    query->searchio = 3;
    query->start_date = 1577883661;
    query->end_date = 1578003661;
    query->user_id = 1;
    vec_search(query, crypto);
}
