
#include "segment.h"
#include "math_utils.h"
#include "noise.h"
#include "config.h"
#include "full_generation.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_SEGMENTS 4096
#define MAX_QUEUE 2048
#define MAX_BRANCHES 4

typedef struct {
    Segment* segment;
    double t;
} QueueItem;

static Segment* segments[MAX_SEGMENTS];
static int segment_count = 0;

static QueueItem queue[MAX_QUEUE];
static int queue_size = 0;

int SEGMENT_LIMIT = 2048;

static void enqueue(Segment* s, double t) {
    if (queue_size < MAX_QUEUE) {
        queue[queue_size].segment = s;
        queue[queue_size].t = t;
        queue_size++;
    }
}

static Segment* dequeue() {
    if (queue_size == 0) return NULL;
    int min_i = 0;
    double min_t = queue[0].t;
    for (int i = 1; i < queue_size; ++i) {
        if (queue[i].t < min_t) {
            min_i = i;
            min_t = queue[i].t;
        }
    }
    Segment* result = queue[min_i].segment;
    queue[min_i] = queue[--queue_size];
    return result;
}

static void add_segment(Segment* s) {
    if (segment_count < MAX_SEGMENTS) {
        segments[segment_count++] = s;
    }
}

static int too_close(Point a, Point b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return dx * dx + dy * dy < SNAP_DISTANCE * SNAP_DISTANCE;
}

static int local_constraints(Segment* s) {
    for (int i = 0; i < segment_count; ++i) {
        Segment* other = segments[i];
        if (too_close(s->end, other->end)) {
            s->end = other->end;
            return 1;
        }
    }
    return 1;
}

static void global_goals(Segment* s, Segment** out, int* out_count) {
    double base_dir = atan2(s->end.y - s->start.y, s->end.x - s->start.x);
    double pop = population_noise(s->end.x / 1000.0, s->end.y / 1000.0);
    *out_count = 0;

    Point start = s->end;
    double length = s->highway ? HIGHWAY_SEGMENT_LENGTH : SEGMENT_LENGTH;
    double deviation = 0.1;

    // Always continue straight
    Segment* forward = segment_create(start, (Point){
        start.x + length * cos(base_dir),
        start.y + length * sin(base_dir)
    }, s->highway, segment_count + *out_count);
    out[(*out_count)++] = forward;

    if (s->highway) {
        if (pop > 0.6) {
            if ((rand() % 100) < (HIGHWAY_BRANCH_PROBABILITY * 100)) {
                Segment* left = segment_create(start, (Point){
                    start.x + length * cos(base_dir - M_PI / 4.0 + deviation),
                    start.y + length * sin(base_dir - M_PI / 4.0 + deviation)
                }, 0, segment_count + *out_count);
                out[(*out_count)++] = left;
            }
            if ((rand() % 100) < (HIGHWAY_BRANCH_PROBABILITY * 100)) {
                Segment* right = segment_create(start, (Point){
                    start.x + length * cos(base_dir + M_PI / 4.0 - deviation),
                    start.y + length * sin(base_dir + M_PI / 4.0 - deviation)
                }, 0, segment_count + *out_count);
                out[(*out_count)++] = right;
            }
        }
    } else {
        if (pop > 0.4) {
            if ((rand() % 100) < (DEFAULT_BRANCH_PROBABILITY * 100)) {
                Segment* left = segment_create(start, (Point){
                    start.x + length * cos(base_dir - M_PI / 4.0 + deviation),
                    start.y + length * sin(base_dir - M_PI / 4.0 + deviation)
                }, 0, segment_count + *out_count);
                out[(*out_count)++] = left;
            }
            if ((rand() % 100) < (DEFAULT_BRANCH_PROBABILITY * 100)) {
                Segment* right = segment_create(start, (Point){
                    start.x + length * cos(base_dir + M_PI / 4.0 - deviation),
                    start.y + length * sin(base_dir + M_PI / 4.0 - deviation)
                }, 0, segment_count + *out_count);
                out[(*out_count)++] = right;
            }
        }
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

            Segment* branches[MAX_BRANCHES];
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
