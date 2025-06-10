
#include "segment.h"
#include "generation.h"
#include <stdio.h>

void generate_city(void) {
    printf("Generating city...\n");

    Point a = {0, 0};
    Point b = {200, 0};
    Segment* root = segment_create(a, b, 1, 0);
    segment_print(root);

    Point c = {b.x, b.y};
    Point d = {c.x + 100, c.y + 50};
    Segment* branch = segment_create(c, d, 0, 1);
    segment_print(branch);
}
