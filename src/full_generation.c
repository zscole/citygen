
#include "segment.h"
#include "math_utils.h"
#include "noise.h"
#include "config.h"
#include "full_generation.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int SEGMENT_LIMIT = 2048;
#define MAX_SEGMENTS 4096
#define MAX_QUEUE 2048
#define MAX_NEIGHBORS 32

typedef struct {
    Segment* segment;
    double t;
} QueueItem;

static Segment* segments[MAX_SEGMENTS];
static int segment_count = 0;

static QueueItem queue[MAX_QUEUE];
static int queue_size = 0;

static int compare_queue(const void* a, const void* b) {
    double ta = ((QueueItem*)a)->t;
    double tb = ((QueueItem*)b)->t;
    return (ta > tb) - (ta < tb);
}

static void enqueue(Segment* s, double t) {
    if (queue_size < MAX_QUEUE) {
        queue[queue_size].segment = s;
        queue[queue_size].t = t;
        queue_size++;
    }
}

static Segment* dequeue() {
    if (queue_size == 0) return NULL;
    qsort(queue, queue_size, sizeof(QueueItem), compare_queue);
    return queue[--queue_size].segment;
}

static void add_segment(Segment* s) {
    if (segment_count < MAX_SEGMENTS) {
        segments[segment_count++] = s;
    }
}

static int too_close(Point a, Point b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return dx * dx + dy * dy < (SNAP_DISTANCE * SNAP_DISTANCE);
}

static int local_constraints(Segment* s) {
    for (int i = 0; i < segment_count; ++i) {
        Segment* other = segments[i];
        if (too_close(s->end, other->end)) {
            s->end = other->end; // snap to endpoint
            return 1;
        }
    }
    return 1;
}

static double population_density(Point p) {
    double x = p.x / 1000.0;
    double y = p.y / 1000.0;
    double v = fake_noise(x, y);
    return (v + 1.0) * 0.5;
}

static void global_goals(Segment* s, Segment** out, int* out_count) {
    double base_dir = atan2(s->end.y - s->start.y, s->end.x - s->start.x);
    double pop = population_density(s->end);
    *out_count = 0;

    Point start = s->end;
    double length = s->highway ? HIGHWAY_SEGMENT_LENGTH : SEGMENT_LENGTH;
    double deviation = 0.15; // ~8.5 degrees

    out[(*out_count)++] = segment_create(start, (Point){
        start.x + length * cos(base_dir + deviation),
        start.y + length * sin(base_dir + deviation)
    }, s->highway, segment_count + *out_count);

    out[(*out_count)++] = segment_create(start, (Point){
        start.x + length * cos(base_dir - deviation),
        start.y + length * sin(base_dir - deviation)
    }, s->highway, segment_count + *out_count);

    if (!s->highway && pop > 0.5) {
        out[(*out_count)++] = segment_create(start, (Point){
            start.x + length * cos(base_dir + 0.5),
            start.y + length * sin(base_dir + 0.5)
        }, 0, segment_count + *out_count);

        out[(*out_count)++] = segment_create(start, (Point){
            start.x + length * cos(base_dir - 0.5),
            start.y + length * sin(base_dir - 0.5)
        }, 0, segment_count + *out_count);
    }
}

void full_generate_city(void) {
    printf("Starting full city generation\n");

    Point a = {0, 0};
    Point b = {HIGHWAY_SEGMENT_LENGTH, 0};
    Segment* root = segment_create(a, b, 1, 0);
    enqueue(root, 0);

    while (queue_size > 0 && segment_count < SEGMENT_LIMIT) {
        Segment* current = dequeue();
        if (!current) break;

        if (local_constraints(current)) {
            add_segment(current);
            for (int i = 0; i < current->connection_count; ++i) segment_add_connection(current->connections[i], current);

            Segment* branches[8];
            int branch_count = 0;
            global_goals(current, branches, &branch_count);

            for (int i = 0; i < branch_count; ++i) {
                segment_add_connection(current, branches[i]);
                enqueue(branches[i], current->id + 1);
            }
        } else {
            free(current);
        }
    }

    for (int i = 0; i < segment_count; ++i) {
        segment_print(segments[i]);
    }

    printf("Generated %d segments.\n", segment_count);
}
