
#include "full_generation.h"
#include <stdlib.h>
#include <stdio.h>

extern int SEGMENT_LIMIT;

int main(int argc, char** argv) {
    if (argc > 1) {
        SEGMENT_LIMIT = atoi(argv[1]);
        if (SEGMENT_LIMIT <= 0) {
            printf("Invalid segment limit. Must be > 0\n");
            return 1;
        }
    }

    full_generate_city();
    return 0;
}
