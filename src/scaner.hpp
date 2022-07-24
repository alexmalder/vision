#pragma once

#include <fstream>
#include "kafka.hpp"
#include "nlohmann/json.hpp"

struct crypto_t {
	int unix_val;
	std::string symbol;
	double open;
	double high;
	double low;
	double close;
	double volume;
};

class scaner {
public:
	std::vector<crypto_t> crypto_data;
    
	void scan_data()
	{
		std::ifstream jsonfile("/etc/data/data.json");
		nlohmann::json data = nlohmann::json::parse(jsonfile);
		for (auto row : data) {
			crypto_t c;
			c.unix_val = row["unix_val"].get<int>();
			c.symbol = row["symbol"].get<std::string>();
			c.open = row["open"].get<double>();
			c.high = row["high"].get<double>();
			c.low = row["low"].get<double>();
			c.close = row["close"].get<double>();
			c.volume = row["volume"].get<double>();
			crypto_data.push_back(c);
		}
	}

	void construct_origin(std::vector<std::string> &messages)
	{
		for (auto item : crypto_data) {
			nlohmann::json j;
			j["unix_val"] = item.unix_val;
			j["symbol"] = item.symbol;
			j["open"] = item.open;
			j["high"] = item.high;
			j["low"] = item.low;
			j["close"] = item.close;
			j["volume"] = item.volume;
			messages.push_back(j.dump());
		}
	}

	void push_data_queue(std::vector<std::string> &messages)
	{
		this->construct_origin(messages);
		Kafka *kafka = new Kafka("data");
		for (std::string message : messages) {
			kafka->produce(message);
		}
		kafka->flush_and_destroy();
		delete kafka;
	}

	void print_data_std()
	{
		for (auto item : crypto_data) {
			printf("%d %s %lf %lf %lf %lf %lf\n", item.unix_val, item.symbol.data(),
			       item.open, item.high, item.low, item.close, item.volume);
		}
	}
};
