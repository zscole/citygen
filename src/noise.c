
#include "noise.h"
#include <stdlib.h>
#include <math.h>


double fake_noise(double x, double y) {
    // Very basic and deterministic placeholder noise
    return fmod(sin(x * 12.9898 + y * 78.233) * 43758.5453, 1.0);
}
