
#include "segment.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Segment* segment_create(Point start, Point end, int highway, int id) {
    Segment* s = malloc(sizeof(Segment));
    s->start = start;
    s->end = end;
    s->id = id;
    s->highway = highway;
    s->connections = NULL;
    s->connection_count = 0;

    double dx = end.x - start.x;
    double dy = end.y - start.y;
    s->length = sqrt(dx * dx + dy * dy);

    return s;
}

void segment_add_connection(Segment* from, Segment* to) {
    from->connections = realloc(from->connections, sizeof(Segment*) * (from->connection_count + 1));
    from->connections[from->connection_count++] = to;
}

void segment_print(Segment* s) {
    printf("Segment %d: (%.1f, %.1f) -> (%.1f, %.1f), highway=%d, links=%d\n",
        s->id, s->start.x, s->start.y, s->end.x, s->end.y, s->highway, s->connection_count);
}
