#ifndef FLOAT3_H
#define FLOAT3_H

// Config:

// Use SSE-based hand optimized version
#define USE_SSE_VECTORS


// ---------------------------------

#ifdef USE_SSE_VECTORS
// Use SSE4 instructions
//# define USE_SSE4

# include "float3_sse.h"

#else // USE_SSE_VECTORS
// Use double precision
//# define USE_DOUBLE

# include "v3_gen.h"
# ifdef USE_DOUBLE
#   define float double
# endif
typedef v3<float> float3;

#endif // USE_SSE_VECTORS


// Implementation-agnostic functions

static inline float3 vbound(float3r min, float3r v, float3r max) { return vmin(vmax(min, v), max); }
static inline float3 vsqlength(float3r v) { return vdot(v, v); }

static inline float3 vreflect(float3r ray, float3r normal)
{
  return ray - (float(2.0)*vdot(ray, normal))*normal;
}

template<typename T>
static inline T sqr(T u) { return u*u; }

#endif // FLOAT3_H
