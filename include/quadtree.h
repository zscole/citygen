
#ifndef QUADTREE_H
#define QUADTREE_H

#include "math_utils.h"

#define QT_MAX_OBJECTS 10
#define QT_MAX_LEVELS 5

typedef struct {
    double x, y, width, height;
} AABB;

struct Segment;

typedef struct {
    AABB bounds;
    struct Segment* object;
} QTObject;

typedef struct Quadtree {
    int level;
    AABB bounds;

    QTObject objects[QT_MAX_OBJECTS];
    int object_count;

    struct Quadtree* children[4];
} Quadtree;

Quadtree* qt_create(int level, AABB bounds);
void qt_insert(Quadtree* qt, QTObject obj);
void qt_retrieve(Quadtree* qt, AABB area, struct Segment** out, int* count, int max_count);
void qt_clear(Quadtree* qt);
void qt_free(Quadtree* qt);

#endif
