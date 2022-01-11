#include "src/vision.h"

int main()
{
    struct query_t *q = malloc(sizeof(struct query_t));
    query_init(q, 2, 1599004800, 1606694400, 1);
    search_similarity(q);
    zmq_listen();
    return 0;
}
