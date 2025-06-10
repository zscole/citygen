
#ifndef SEGMENT_H
#define SEGMENT_H

#include "math_utils.h"

typedef struct Segment {
    Point start;
    Point end;
    int id;
    int highway;
    double length;

    struct Segment** connections;
    int connection_count;
} Segment;

Segment* segment_create(Point start, Point end, int highway, int id);
void segment_add_connection(Segment* from, Segment* to);
void segment_print(Segment* s);

#endif
