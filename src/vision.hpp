#pragma once

#include <iostream>
#include <vector>
#include <NumCpp.hpp>
#include <nlohmann/json.hpp>
#include "kafka.hpp"

struct crypto_t {
	int unix_val;
	std::string symbol;
	double open;
	double high;
	double low;
	double close;
	double volume;
};

struct query_t {
	int searchio;
	int start_date;
	int end_date;
	int user_id;
	int resolution;
	double thresh;
};

class vision {
    public:
	void vec_construct_origin(std::vector<std::string> &messages,
				  std::vector<crypto_t> &crypto_data)
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

	double vec_similarity(std::vector<double> a, std::vector<double> b, uint64_t end)
	{
		const nc::NdArray<double> a_nc = a;
		const nc::NdArray<double> b_nc = b;
		nc::NdArray<double> comp = nc::dot(a_nc, b_nc) / (nc::norm(a_nc) * nc::norm(b_nc));
		return comp[0];
	}

	void vec_filter(std::vector<crypto_t> &src, std::vector<crypto_t> &dest, query_t *query)
	{
		for (uint64_t i = 0; i < src.size(); i++) {
			if (src[i].unix_val > query->start_date &&
			    src[i].unix_val < query->end_date) {
				dest.push_back(src[i]);
			}
		}
	}

	std::vector<double> vec_iteration(std::vector<crypto_t> &src, std::vector<double> &dest,
					  int start, int end)
	{
		for (int i = start; i < end; i++) {
			dest.push_back(src[i].close);
		}
		return dest;
	}

	void vec_up_to_date(std::vector<crypto_t> &dest, std::vector<crypto_t> &target)
	{
		for (int i = 0; i < target.size(); i++) {
			target[i].unix_val = dest[i].unix_val;
			target[i].open = target[i].open += dest[i].open;
		}
	}

	void vec_debug(std::vector<crypto_t> &dest, std::vector<crypto_t> &target, double sim)
	{
		nlohmann::json j0;
		std::vector<double> i0;
		std::vector<double> i1;

		for (auto item : dest) {
			i0.push_back(item.open);
		}

		for (auto item : dest) {
			i1.push_back(item.open);
		}
		j0["src"] = i0;
		j0["dest"] = i1;
		j0["sim"] = sim;

		std::cout << j0.dump() << std::endl;
	}

	int vec_search(std::vector<crypto_t> &src, query_t *query)
	{
		// declare new vector
		std::vector<crypto_t> dest;
		// make filter
		vec_filter(src, dest, query);
		// static values
		const int ssize = src.size();
		const int dsize = dest.size();
		// debug values
		// std::cout << "src size: " << ssize << " and dest size: " << dsize << std::endl;
		// iteration values
		int x = 0;
		int y = dest.size();
		double sim;
		std::vector<std::string> messages;
		while (x < ssize) {
			while (y < ssize) {
				std::vector<double> target;
				vec_iteration(src, target, x, y);
				//vec_iteration(dest, target);
				//sim = vec_similarity(dest, target, dsize);
				if (sim > query->thresh && sim < 1) {
					//vec_up_to_date(dest, target);
					//vec_construct_result(messages, target, x, y, sim);
					//vec_debug(dest, target, sim);
				}
				y += query->resolution, x += query->resolution;
			}
			x++;
		}
		//Kafka *kafka = new Kafka("result");
		//for (std::string message : messages) {
		//    kafka->produce(message);
		//}
		//kafka->flush_and_destroy();
		//delete kafka;
		return 0;
	}

	void scan_data(std::vector<crypto_t> &crypto_data)
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
			std::cout << c.unix_val << " | " << c.open << std::endl;
			crypto_data.push_back(c);
		}
	}

	void vec_construct_result(std::vector<std::string> &messages, std::vector<crypto_t> &result,
				  int x, int y, double sim)
	{
		for (auto item : result) {
			nlohmann::json j;
			// custom
			j["x"] = x;
			j["y"] = y;
			j["sim"] = sim;
			// basic
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

	void push_data_queue(std::vector<std::string> &messages, std::vector<crypto_t> &crypto_data)
	{
		vec_construct_origin(messages, crypto_data);
		Kafka *kafka = new Kafka("data");
		for (std::string message : messages) {
			kafka->produce(message);
		}
		kafka->flush_and_destroy();
		delete kafka;
	}

	void print_data_std(std::vector<crypto_t> &crypto_data)
	{
		for (auto item : crypto_data) {
			printf("%d %s %lf %lf %lf %lf %lf\n", item.unix_val, item.symbol.data(),
			       item.open, item.high, item.low, item.close, item.volume);
		}
	}
};
