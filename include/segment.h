
#ifndef SEGMENT_H
#define SEGMENT_H

#include "math_utils.h"

typedef struct Segment {
    Point start;
    Point end;
    int highway;
    int id;
} Segment;

Segment* segment_create(Point start, Point end, int highway, int id);
void segment_print(Segment* s);

#endif
