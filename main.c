#include "src/vision.h"

int main()
{
    struct query_t *query = malloc(sizeof(struct query_t));
    struct query_t *result = malloc(sizeof(struct query_t));
    query_init(query, 2, 1630454400, 1638316800, 1);
    search_similarity(query, result);
    zmq_listen();
    return 0;
}
