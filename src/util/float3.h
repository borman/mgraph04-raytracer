#ifndef FLOAT3_H
#define FLOAT3_H

// Config:

// Use SSE-based hand optimized version
#define USE_SSE_VECTORS


// ---------------------------------

#ifdef USE_SSE_VECTORS
// Use SSE4 instructions
# define USE_SSE4

# include "float3_sse.h"
#else // USE_SSE_VECTORS
// Use double precision
//# define USE_DOUBLE

# include "v3_gen.h"
#  ifdef USE_DOUBLE
#   define float double
# endif
typedef v3<float> float3;
#endif // USE_SSE_VECTORS


// Implementation-agnostic functions

static inline float3 vbound(float3 min, float3 v, float3 max) { return vmin(vmax(min, v), max); }

template<typename T>
static inline T sqr(T u) { return u*u; }

#endif // FLOAT3_H
