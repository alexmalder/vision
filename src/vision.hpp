#pragma once

#include <iostream>
#include <vector>
#include <NumCpp.hpp>
#include <nlohmann/json.hpp>
#include "kafka.hpp"
#include "scaner.hpp"

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
	double vec_similarity(std::vector<double> a, std::vector<double> b)
	{
		const nc::NdArray<double> a_nc = a;
		const nc::NdArray<double> b_nc = b;
		nc::NdArray<double> comp = nc::dot(a_nc, b_nc) / (nc::norm(a_nc) * nc::norm(b_nc));
		return comp[0];
	}

	void vec_filter(std::vector<crypto_t> &dest, query_t *query)
	{
		for (uint64_t i = 0; i < crypto_data.size(); i++) {
			if (crypto_data[i].unix_val > query->start_date &&
			    crypto_data[i].unix_val < query->end_date) {
				dest.push_back(crypto_data[i]);
			}
		}
	}

	std::vector<double> vec_iteration(int start, int end)
	{
        std::vector<double> dest;
		for (int i = start; i < end; i++) {
			dest.push_back(crypto_data[i].open);
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

	void vec_debug(std::vector<double> &dest, std::vector<double> &target, double sim)
	{
		nlohmann::json j0;
		std::vector<double> i0;
		std::vector<double> i1;

		for (auto item : dest) {
			i0.push_back(item);
		}

		for (auto item : dest) {
			i1.push_back(item);
		}
		j0["src"] = i0;
		j0["dest"] = i1;
		j0["sim"] = sim;

		std::cout << j0.dump() << std::endl;
	}

	int vec_search(query_t *query)
	{
		// static values
		const int ssize = crypto_data.size();
		std::cout << "crypto_data size: " << ssize << std::endl;
		// iteration values
		int x = 0 + 31;
		int y = 31 + 31;
		double sim;
		while (x < ssize) {
			while (y < ssize) {
				std::vector<double> source = vec_iteration(x - 31, y - 31);
				std::vector<double> target = vec_iteration(x, y);
				sim = vec_similarity(source, target);
				if (sim > query->thresh && sim < 1) {
					//vec_up_to_date(dest, target);
					//vec_construct_result(messages, target, x, y, sim);
					vec_debug(source, target, sim);
				}
				y += query->resolution, x += query->resolution;
			}
			x++;
		}
		return 0;
	}

	vision(scaner *s)
	{
		this->s = s;
		this->crypto_data = s->crypto_data;
	}

private:
	scaner *s;
	std::vector<crypto_t> crypto_data;
	std::vector<double> destination;
};
