#include <stdio.h>
#include <stdlib.h>

#define COUNT   500

int main(int argc, char **argv) {
    int id;
    size_t i;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <id>\n", argv[0]);
        return EXIT_FAILURE;
    }

    id = atoi(argv[1]);

    for (i = 0; i < COUNT; ++i) {
        fprintf(stdout, "Thread %d: %zu\n", id, i);
    }

    return EXIT_SUCCESS;
}
