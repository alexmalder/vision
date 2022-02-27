#include "include/vision.h"

int main()
{
    char *tnt_user = getenv("TNT_USER");
    char *tnt_pwd = getenv("TNT_PASSWORD");
    char *tnt_host = getenv("TNT_HOST");
    char *tnt_port = getenv("TNT_PORT");
    char conn_str[128];
    sprintf(conn_str, "%s:%s@%s:%s", tnt_user, tnt_pwd, tnt_host, tnt_port);
    query_t *query = malloc(sizeof(query_t));
    query_t *result = malloc(sizeof(query_t));
    query_init(query, 2, 1630454400, 1638316800, 1);
    vec_search(conn_str, query);
    zmq_listen(conn_str);
    return 0;
}
