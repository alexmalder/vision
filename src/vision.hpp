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
	int step;
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
			if (crypto_data[i].unix_val > query->start_date && crypto_data[i].unix_val < query->end_date) {
				dest.push_back(crypto_data[i]);
			}
		}
	}

	std::pair<std::vector<crypto_t>, std::vector<double> > vec_iter(int start, int end)
	{
		std::vector<crypto_t> dest_full;
		std::vector<double> dest_double;
		for (int i = start; i < end; i++) {
			dest_full.push_back(crypto_data[i]);
			dest_double.push_back(crypto_data[i].open);
		}
		return { dest_full, dest_double };
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

	void vec_processing(int x, int y)
	{
		int step = this->q->step;
		std::pair<std::vector<crypto_t>, std::vector<double> > source = vec_iter(x - step, y - step);
		std::pair<std::vector<crypto_t>, std::vector<double> > target = vec_iter(x, y);
		double sim = vec_similarity(source.second, target.second);
		if (sim > this->q->thresh && sim < 1) {
			//vec_up_to_date(dest, target);
			//vec_construct_result(messages, target, x, y, sim);
			vec_debug(source.second, target.second, sim);
		}
	}

	int vec_search(query_t *query)
	{
		// static values
		const int ssize = crypto_data.size();
		std::cout << "crypto_data size: " << ssize << std::endl;
		// iteration values
		int x = 0 + this->q->step;
		int y = this->q->step + this->q->step;
		while (x < ssize) {
			while (y < ssize) {
				this->vec_processing(x, y);
				y += query->resolution, x += query->resolution;
			}
			x++;
		}
		return 0;
	}

	vision(scaner *s, query_t *q)
	{
		this->s = s;
		this->q = q;
		this->crypto_data = s->crypto_data;
	}

private:
	scaner *s;
	query_t *q;
	std::vector<crypto_t> crypto_data;
	std::vector<double> destination;
};
