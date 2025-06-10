
#include "noise.h"
#include <math.h>

#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

double population_noise(double x, double y) {
    return stb_perlin_fbm_noise3((float)x, (float)y, 0.0f, 2.0f, 0.5f, 6);
}
