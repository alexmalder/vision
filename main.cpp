#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <fstream>
#include <src/scaner.hpp>
#include <sstream>
#include <vector>
#include <string>
#include "src/vision.hpp"
#include <pqxx/pqxx>

int main(void)
{
	scaner *s = new scaner();
	s->scan_data();
	s->print_data_std();
	query_t *query = new query_t();
	query->searchio = 3;
	query->start_date = 1641987866;
	query->end_date = 1644666266;
	query->user_id = 1;
	query->resolution = 3;
	query->thresh = 0.998;
    query->step = 7;
	vision *v = new vision(s, query);
	v->vec_search(query);
	return 0;
}
