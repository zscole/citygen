
#ifndef STB_PERLIN_H
#define STB_PERLIN_H

#ifdef __cplusplus
extern "C" {
#endif

float stb_perlin_noise3(float x, float y, float z, int x_wrap, int y_wrap, int z_wrap);
float stb_perlin_fbm_noise3(float x, float y, float z, float lacunarity, float gain, int octaves);

#ifdef STB_PERLIN_IMPLEMENTATION

#include <math.h>

static int stb__perlin_randtab[512] = {
   23,199,115,250,216,154,39,97,35,254,233,200,6,207,246,77,
   30,70,198,103,44,75,189,43,195,57,98,76,100,134,63,145,
   116,90,167,64,230,197,53,169,36,216,49,203,6,255,237,14,
   122,169,76,229,165,74,222,229,108,194,248,215,78,221,13,251,
   112,4,206,196,205,128,237,219,129,158,16,255,203,237,36,34,
   188,237,14,57,72,38,187,26,99,41,197,44,186,187,56,140,
   84,212,134,158,104,149,250,138,190,17,134,248,93,28,23,106,
   213,22,27,112,28,105,248,56,46,245,121,127,251,43,105,146,
   74,133,141,244,196,64,43,125,167,37,190,137,30,157,148,104,
   204,218,1,59,55,36,153,9,254,195,65,154,87,168,174,187,
   190,234,88,56,59,195,89,201,208,184,149,91,53,104,43,137,
   142,18,50,91,165,193,72,30,198,179,118,23,157,69,205,243,
   112,239,24,90,190,122,148,241,168,170,139,26,219,127,81,170,
   125,245,96,153,151,240,62,242,158,151,222,96,240,58,27,178,
   102,190,122,47,105,95,91,17,182,65,202,243,197,121,103,240,
   89,169,175,160,136,218,14,23,178,68,217,134,152,248,100,8,

   23,199,115,250,216,154,39,97,35,254,233,200,6,207,246,77,
   30,70,198,103,44,75,189,43,195,57,98,76,100,134,63,145,
   116,90,167,64,230,197,53,169,36,216,49,203,6,255,237,14,
   122,169,76,229,165,74,222,229,108,194,248,215,78,221,13,251,
   112,4,206,196,205,128,237,219,129,158,16,255,203,237,36,34,
   188,237,14,57,72,38,187,26,99,41,197,44,186,187,56,140,
   84,212,134,158,104,149,250,138,190,17,134,248,93,28,23,106,
   213,22,27,112,28,105,248,56,46,245,121,127,251,43,105,146,
   74,133,141,244,196,64,43,125,167,37,190,137,30,157,148,104,
   204,218,1,59,55,36,153,9,254,195,65,154,87,168,174,187,
   190,234,88,56,59,195,89,201,208,184,149,91,53,104,43,137,
   142,18,50,91,165,193,72,30,198,179,118,23,157,69,205,243,
   112,239,24,90,190,122,148,241,168,170,139,26,219,127,81,170,
   125,245,96,153,151,240,62,242,158,151,222,96,240,58,27,178,
   102,190,122,47,105,95,91,17,182,65,202,243,197,121,103,240,
   89,169,175,160,136,218,14,23,178,68,217,134,152,248,100,8
};

static float stb__perlin_grad(int hash, float x, float y, float z)
{
   int h = hash & 15;
   float u = h<8 ? x : y;
   float v = h<4 ? y : h==12||h==14 ? x : z;
   return ((h&1)? -u : u) + ((h&2)? -v : v);
}

float stb_perlin_noise3(float x, float y, float z, int x_wrap, int y_wrap, int z_wrap)
{
   int xi = (int)floor(x) & 255;
   int yi = (int)floor(y) & 255;
   int zi = (int)floor(z) & 255;
   x -= floor(x);
   y -= floor(y);
   z -= floor(z);
   float u = x*x*x*(x*(x*6-15)+10);
   float v = y*y*y*(y*(y*6-15)+10);
   float w = z*z*z*(z*(z*6-15)+10);
   int aaa, aba, aab, abb, baa, bba, bab, bbb;
   aaa = stb__perlin_randtab[stb__perlin_randtab[stb__perlin_randtab[xi]+yi]+zi];
   aba = stb__perlin_randtab[stb__perlin_randtab[stb__perlin_randtab[xi]+yi+1]+zi];
   aab = stb__perlin_randtab[stb__perlin_randtab[stb__perlin_randtab[xi]+yi]+zi+1];
   abb = stb__perlin_randtab[stb__perlin_randtab[stb__perlin_randtab[xi]+yi+1]+zi+1];
   baa = stb__perlin_randtab[stb__perlin_randtab[stb__perlin_randtab[xi+1]+yi]+zi];
   bba = stb__perlin_randtab[stb__perlin_randtab[stb__perlin_randtab[xi+1]+yi+1]+zi];
   bab = stb__perlin_randtab[stb__perlin_randtab[stb__perlin_randtab[xi+1]+yi]+zi+1];
   bbb = stb__perlin_randtab[stb__perlin_randtab[stb__perlin_randtab[xi+1]+yi+1]+zi+1];

   float x1, x2, y1, y2;
   x1 = stb__perlin_grad(aaa, x  , y  , z   );
   x2 = stb__perlin_grad(baa, x-1, y  , z   );
   y1 = x1 + u*(x2-x1);

   x1 = stb__perlin_grad(aba, x  , y-1, z   );
   x2 = stb__perlin_grad(bba, x-1, y-1, z   );
   y2 = x1 + u*(x2-x1);
   float z1 = y1 + v*(y2-y1);

   x1 = stb__perlin_grad(aab, x  , y  , z-1 );
   x2 = stb__perlin_grad(bab, x-1, y  , z-1 );
   y1 = x1 + u*(x2-x1);

   x1 = stb__perlin_grad(abb, x  , y-1, z-1 );
   x2 = stb__perlin_grad(bbb, x-1, y-1, z-1 );
   y2 = x1 + u*(x2-x1);
   float z2 = y1 + v*(y2-y1);

   return z1 + w*(z2-z1);
}

float stb_perlin_fbm_noise3(float x, float y, float z, float lacunarity, float gain, int octaves)
{
   float sum = 0;
   float amp = 1.0f;
   for (int i = 0; i < octaves; ++i) {
      sum += stb_perlin_noise3(x, y, z, 0, 0, 0) * amp;
      x *= lacunarity;
      y *= lacunarity;
      z *= lacunarity;
      amp *= gain;
   }
   return sum;
}

#endif // STB_PERLIN_IMPLEMENTATION

#endif // STB_PERLIN_H
