#ifndef FLOAT3_H
#define FLOAT3_H

#define USE_SSE_VECTORS

#ifdef USE_SSE_VECTORS

#include "float3_sse.h"

#else

#include "v3_gen.h"
typedef v3<float> float3;

#endif // USE_SSE_VECTORS

static inline float3 vbound(float3 min, float3 v, float3 max) { return vmin(vmax(min, v), max); }

#endif // FLOAT3_H
