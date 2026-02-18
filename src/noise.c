#include "noise.h"
#include <math.h>
#include <stdint.h>

static uint32_t hash_u32(uint32_t x){
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

static float rand01(int xi, int yi){
    uint32_t h = hash_u32((uint32_t)xi*374761393u + (uint32_t)yi*668265263u);
    return (h & 0xffffff) / 16777215.0f;
}

static float lerp(float a, float b, float t){ return a + (b-a)*t; }
static float fade(float t){ return t*t*(3.0f-2.0f*t); }

float noise2(float x, float y){
    int x0 = (int)floorf(x);
    int y0 = (int)floorf(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float sx = fade(x - (float)x0);
    float sy = fade(y - (float)y0);

    float n00 = rand01(x0,y0);
    float n10 = rand01(x1,y0);
    float n01 = rand01(x0,y1);
    float n11 = rand01(x1,y1);

    float ix0 = lerp(n00, n10, sx);
    float ix1 = lerp(n01, n11, sx);
    return lerp(ix0, ix1, sy);
}

float fbm2(float x, float y, int octaves){
    float sum=0.0f, amp=0.5f, f=1.0f;
    for(int i=0;i<octaves;i++){
        sum += amp * noise2(x*f, y*f);
        f *= 2.0f;
        amp *= 0.5f;
    }
    return sum;
}
