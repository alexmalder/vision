#include "include/vision.h"

int main()
{
    query_t *query = malloc(sizeof(query_t));
    query_t *result = malloc(sizeof(query_t));
    query_init(query, 2, 1630454400, 1638316800, 1);
    vec_search(query, result);
    zmq_listen();
    return 0;
}
