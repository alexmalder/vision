#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "src/vision.hpp"

int main(void)
{
    vision *v = new vision();
	v->scan_data();
	v->print_data_std();
	//push_data(messages, crypto_data);
	query_t *query = new query_t();
	query->searchio = 3;
	query->start_date = 1641987866;
	query->end_date = 1644666266;
	query->user_id = 1;
	query->resolution = 3;
	query->thresh = 0.998;
	return 0;
}
