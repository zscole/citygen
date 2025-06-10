
#include "quadtree.h"
#include <stdlib.h>
#include <string.h>

static int aabb_overlap(AABB a, AABB b) {
    return (a.x < b.x + b.width && a.x + a.width > b.x &&
            a.y < b.y + b.height && a.y + a.height > b.y);
}

static void split(Quadtree* qt) {
    double hw = qt->bounds.width / 2.0;
    double hh = qt->bounds.height / 2.0;
    double x = qt->bounds.x;
    double y = qt->bounds.y;

    qt->children[0] = qt_create(qt->level + 1, (AABB){x + hw, y, hw, hh});
    qt->children[1] = qt_create(qt->level + 1, (AABB){x, y, hw, hh});
    qt->children[2] = qt_create(qt->level + 1, (AABB){x, y + hh, hw, hh});
    qt->children[3] = qt_create(qt->level + 1, (AABB){x + hw, y + hh, hw, hh});
}

static int getIndex(AABB parent, AABB box) {
    double verticalMid = parent.x + parent.width / 2.0;
    double horizontalMid = parent.y + parent.height / 2.0;

    int top = box.y + box.height < horizontalMid;
    int bottom = box.y > horizontalMid;
    int left = box.x + box.width < verticalMid;
    int right = box.x > verticalMid;

    if (top && right) return 0;
    if (top && left) return 1;
    if (bottom && left) return 2;
    if (bottom && right) return 3;
    return -1;
}

Quadtree* qt_create(int level, AABB bounds) {
    Quadtree* qt = malloc(sizeof(Quadtree));
    qt->level = level;
    qt->bounds = bounds;
    qt->object_count = 0;
    memset(qt->children, 0, sizeof(qt->children));
    return qt;
}

void qt_insert(Quadtree* qt, QTObject obj) {
    if (!qt) return;

    if (qt->children[0]) {
        int idx = getIndex(qt->bounds, obj.bounds);
        if (idx >= 0 && idx < 4 && qt->children[idx]) {
            qt_insert(qt->children[idx], obj);
            return;
        }
    }

    if (qt->object_count < QT_MAX_OBJECTS) {
        qt->objects[qt->object_count++] = obj;
    }

    if (qt->object_count >= QT_MAX_OBJECTS && qt->level < QT_MAX_LEVELS) {
        if (!qt->children[0]) split(qt);

        for (int i = 0; i < qt->object_count;) {
            int idx = getIndex(qt->bounds, qt->objects[i].bounds);
            if (idx >= 0 && idx < 4 && qt->children[idx]) {
                qt_insert(qt->children[idx], qt->objects[i]);
                qt->objects[i] = qt->objects[--qt->object_count];
            } else {
                ++i;
            }
        }
    }
}

void qt_retrieve(Quadtree* qt, AABB area, struct Segment** out, int* count, int max_count) {
    if (!qt || !out || !count) return;

    for (int i = 0; i < qt->object_count && *count < max_count; ++i) {
        if (aabb_overlap(area, qt->objects[i].bounds)) {
            out[(*count)++] = qt->objects[i].object;
        }
    }

    if (!qt->children[0]) return;

    for (int i = 0; i < 4; ++i) {
        if (qt->children[i]) {
            qt_retrieve(qt->children[i], area, out, count, max_count);
        }
    }
}

void qt_clear(Quadtree* qt) {
    if (!qt) return;
    qt->object_count = 0;
    for (int i = 0; i < 4; ++i) {
        if (qt->children[i]) {
            qt_clear(qt->children[i]);
            qt_free(qt->children[i]);
            qt->children[i] = NULL;
        }
    }
}

void qt_free(Quadtree* qt) {
    if (qt) free(qt);
}
