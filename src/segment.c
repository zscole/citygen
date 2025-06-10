
#include "segment.h"
#include <stdio.h>
#include <stdlib.h>

Segment* segment_create(Point start, Point end, int highway, int id) {
    Segment* s = malloc(sizeof(Segment));
    s->start = start;
    s->end = end;
    s->highway = highway;
    s->id = id;
    return s;
}

void segment_print(Segment* s) {
    printf("Segment %d: (%.1f, %.1f) -> (%.1f, %.1f), highway=%d\n",
        s->id, s->start.x, s->start.y, s->end.x, s->end.y, s->highway);
}
