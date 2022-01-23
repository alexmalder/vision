#include "vision.h"
#include <stdint.h>

void debug_iteration(struct query_t *query, uint64_t founded_start_date, uint64_t founded_end_date, uint64_t ssize, uint64_t slide, double distance,
                     uint64_t x, uint64_t y, double sim, struct row_t *source, struct row_t *target)
{
    char buffer[4096 * 3];
    char source_buffer[4096];
    sprintf(source_buffer, "[ ");
    for (uint64_t i = 0; i < ssize; i++) {
        if (i == ssize - 1) {
            sprintf(source_buffer + strlen(source_buffer), "%lf", source[i].value);
        } else {
            sprintf(source_buffer + strlen(source_buffer), "%lf,", source[i].value);
        }
    }
    sprintf(source_buffer + strlen(source_buffer), "]");
    char target_buffer[4096];
    sprintf(target_buffer, "[");
    for (uint64_t i = 0; i < ssize; i++) {
        if (i == ssize - 1) {
            sprintf(target_buffer + strlen(target_buffer), "%lf", target[i].value);
        } else {
            sprintf(target_buffer + strlen(target_buffer), "%lf,", target[i].value);
        }
    }
    sprintf(target_buffer + strlen(target_buffer), "]");
    sprintf(
        buffer,
        "{\"symbol\": %lld, \"query_start_date\": %lld, \"query_end_date\": %lld, \"query_user_id\": %lld, \"founded_start_date\": %lld, \"founded_end_date\": %lld, \"ssize\": %lld, \"slide\": %lld, \"distance\": %lf, \"x\": %lld, \"y\": %lld, \"similarity\": %lf, \"source\": %s, \"target\": %s}\n",
        query->searchio, query->start_date, query->end_date, query->user_id, founded_start_date, founded_end_date, ssize, slide, distance, x, y, sim, source_buffer, target_buffer);
    int status = produce(buffer);
    printf("%s\n", buffer);
    printf("[status: %d]\n", status);
}