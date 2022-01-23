#include "src/vision.h"

int main()
{
    struct query_t *query = malloc(sizeof(struct query_t));
    query_init(query, 1, 1630454400, 1638316800, 1);
    search_similarity(query);
    zmq_listen();
    return 0;
}
