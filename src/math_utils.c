
#include "math_utils.h"
#include <math.h>

double angle_between(Point a, Point b) {
    return atan2(b.y - a.y, b.x - a.x) * 180.0 / M_PI;
}
