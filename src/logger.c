#include "vision.h"

void debug_iteration(double *a, double *b, double sim, uint64_t ssize,
                     uint64_t slide, double distance, uint64_t x, uint64_t y)
{
    char buffer[4096 * 3];
    char source_buffer[4096];
    sprintf(source_buffer, "[ ");
    for (uint64_t i = 0; i < ssize; i++) {
        if (i == ssize - 1) {
            sprintf(source_buffer + strlen(source_buffer), "%lf", a[i]);
        } else {
            sprintf(source_buffer + strlen(source_buffer), "%lf,", a[i]);
        }
    }
    sprintf(source_buffer + strlen(source_buffer), "]");
    char target_buffer[4096];
    sprintf(target_buffer, "[");
    for (uint64_t i = 0; i < ssize; i++) {
        if (i == ssize - 1) {
            sprintf(target_buffer + strlen(target_buffer), "%lf", a[i]);
        } else {
            sprintf(target_buffer + strlen(target_buffer), "%lf,", a[i]);
        }
    }
    sprintf(target_buffer + strlen(target_buffer), "]");
    sprintf(
        buffer,
        "{\"ssize\": %lld, \"slide\": %lld, \"distance\": %lf, \"x\": %lld, \"y\": %lld, \"similarity\": %lf, \"source\": %s, \"target\": %s}\n",
        ssize, slide, distance, x, y, sim, source_buffer, target_buffer);
    int status = produce(buffer);
    printf("%s\n", buffer);
    printf("[status: %d]\n", status);
}
