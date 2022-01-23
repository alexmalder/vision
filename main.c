#include "src/vision.h"

int main()
{
    struct query_t *query = malloc(sizeof(struct query_t));
    for (int i = 1; i < 6; i++) {
        query_init(query, i, 1630454400, 1638316800, 1);
        search_similarity(query);
    }
    zmq_listen();
    return 0;
}
