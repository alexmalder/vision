#include "src/vision.h"

struct http_server_s *server;

void handle_sigterm(int signum)
{
    free(server);
    exit(0);
}

int main()
{
    signal(SIGTERM, handle_sigterm);
    cosine_similarity_test();
    server = http_server_init(5000, handle_request);
    printf("httpserver listening on port 5000...\n");
    fflush(stdout);
    http_server_listen(server);
    return 0;
}
