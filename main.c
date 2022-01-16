#include "src/vision.h"

int main()
{
    struct query_t *q = malloc(sizeof(struct query_t));
    query_init(q, 2, 1630454400, 1638316800, 1);
    search_similarity(q);
    zmq_listen();
    return 0;
}
